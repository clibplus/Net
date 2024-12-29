#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.c"

int count_tabs(const char *data) {
    int count = 0;
    
    for(int i = 0; i < strlen(data); i++) {
        if(data[i] != '\t' && data[i] != ' ')
            break;
            
        if(data[i] == '\t')
            count++;
    }

    return count;
}

Control **process_html_line(const char *data) {
    if (!data)
        return NULL;

    String line = NewString(data);
    Array args = NewArray(NULL);

    args.Merge(&args, (void **)line.Split(&line, " "));

    Control **Controls = (Control **)malloc(sizeof(Control *) * 1);
    int idx = 0;
    Control **control_stack = (Control **)malloc(sizeof(Control *) * 1); // Simulated stack
    int stack_size = 0;

    for (int i = 0; i < args.idx; i++) {
        if (!args.arr[i])
            break;

        String element = NewString(args.arr[i]);
        if(element.Is(&element, "") || element.Is(&element, " ")) {
            element.Destruct(&element);
            continue;
        }

        /* Tag Found */
        if (element.Contains(&element, "<")) {
            char *stripped = (element.data[element.idx - 1] == '>' ? 
                              element.GetSubstr(&element, 1, element.idx - 1) : 
                              element.GetSubstr(&element, 1, element.idx - 2)); // Handles <tag> or <tag...
            ControlTag CHECK_TAG = FindTagType(stripped);

            /* Invalid Tag */
            if (!(int)CHECK_TAG) {
                free(stripped);
                element.Destruct(&element);
                continue;
            }

            Control *new_control = (Control *)malloc(sizeof(Control));
            *new_control = (Control){
                .Tag = CHECK_TAG,
                .ID = NULL,
                .Type = NULL,
                .Text = NULL,
                .Class = NULL,
                .href = NULL,
                .CSS = NULL,
                .Data = NULL,
                .OnClick = 0,
                .OnClickJS = NULL,
                .FormID = NULL,
                .DisplayID = NULL,
                .SubControls = malloc(sizeof(Control *) * 1),
                .SubControlCount = 0,
                .Parent = NULL
            };

            if (element.data[element.idx - 1] != '>') {
                while (++i < args.idx && ((char *)args.arr[i])[0] != '<') {
                    String attribute = NewString(args.arr[i]);
                    if (attribute.StartsWith(&attribute, "id=")) {
                        new_control->ID = strdup(attribute.GetSubstr(&attribute, strlen("id="), attribute.idx - 2));
                    } else if (attribute.StartsWith(&attribute, "class=")) {
                        new_control->Class = strdup(attribute.GetSubstr(&attribute, strlen("class="), attribute.idx - 2));
                    } else if (attribute.StartsWith(&attribute, "style=")) {
                        String rCSS = NewString(attribute.GetSubstr(&attribute, strlen("style="), attribute.idx - 2));
                        new_control->CSS = (char **)rCSS.Split(&rCSS, ";");
                        rCSS.Destruct(&rCSS);
                    } else {
                        String new_data = NewString(new_control->Data);
                        new_data.AppendArray(&new_data, (const char *[]){attribute.data, " ", NULL});
                        if (new_control->Data)
                            free(new_control->Data);

                        new_control->Data = strdup(new_data.data);
                        new_data.Destruct(&new_data);
                    }
                    attribute.Destruct(&attribute);
                }
                i--;
            }

            if (stack_size > 0) {
                Control *parent = control_stack[stack_size - 1];

                parent->SubControls = (void **)realloc(parent->SubControls, sizeof(void *) * (parent->SubControlCount + 1));
                parent->SubControls[parent->SubControlCount] = new_control;
                
                new_control->Parent = parent;
                parent->SubControlCount++;
            } else {
                Controls[idx] = new_control;
                idx++;
                Controls = (Control **)realloc(Controls, sizeof(Control *) * (idx + 1));
            }

            control_stack = (Control **)realloc(control_stack, sizeof(Control *) * (stack_size + 1));
            control_stack[stack_size] = new_control;
            stack_size++;
            free(stripped);
        }

        else if (element.data[0] == '<' && element.data[1] == '/') {
            if (stack_size > 0)
                stack_size--;
        }

        element.Destruct(&element);
    }

    Controls[idx] = NULL;
    line.Destruct(&line);
    args.Destruct(&args);
    free(control_stack);
    return Controls;
}

Control **ParseHTMLContent(const char *data) {
    if (!data || strlen(data) == 0) return NULL;

    Control **controls = (Control **)malloc(sizeof(Control *));
    int idx = 0;

    String raw = NewString(data);
    Array lines = NewArray(NULL);
    lines.Merge(&lines, (void **)raw.Split(&raw, "\n"));

    for (int i = 0; i < lines.idx; i++) {
        if (!lines.arr[i] || strlen(lines.arr[i]) == 0) continue;

        Control **line_controls = process_html_line(lines.arr[i]);
        for (int c = 0; line_controls && line_controls[c] != NULL; c++) {
            controls = (Control **)realloc(controls, sizeof(Control *) * (idx + 1));
            controls[idx++] = line_controls[c];
        }
        free(line_controls);
    }

    controls = (Control **)realloc(controls, sizeof(Control *) * (idx + 1));
    controls[idx] = NULL;

    raw.Destruct(&raw);
    lines.Destruct(&lines);
    return controls;
}