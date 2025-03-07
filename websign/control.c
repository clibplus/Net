#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../web.h"

/*

Control *Controls[] = {
    CreateControl(HEAD_TAG, NULL, NULL, NULL, {
        CreateControl(TITLE_TAG, NULL, NULL, "My Website", NULL),
        NULL
    }),
    CreateControl(BODY_TAG, NULL, NULL, NULL, {
        CreateControl(P_TAG, NULL, NULL, "Hi", NULL),
        NULL
    }),
    NULL
});

*/
Control *CreateControl(ControlTag tag, const char *sclass, const char *id, const char *text, Control **subcontrols) {
    Control *c = (Control *)malloc(sizeof(Control));
    *c = (Control){
        .Tag                = tag,
        .ID                 = (!id ? NULL : strdup(id)),
        .Class              = (!sclass ? NULL : strdup(sclass)),
        .Text               = (!text ? NULL : strdup(text)),
        .SubControls        = (void **)malloc(sizeof(void *) * 1),
        .SubControlCount    = 0,
        .CSS                = (char **)malloc(sizeof(char *) * 1),
        .CSSCount           = 0,

        .sAppend            = Control__AppendStackControl,
        .Append             = AppendControl,
        .AppendAt           = Control__AppendControlAt,
        .AppendCSS          = AppendCSS,
        .Construct          = ConstructControl,
        .Destruct           = DestructControl
    };

    memset(c->SubControls, '\0', sizeof(void *) * 1);
    memset(c->CSS, '\0', sizeof(void *) * 1);
    if(!subcontrols)
        return c;

    while(subcontrols[c->SubControlCount] != NULL)
        AppendControl(c, subcontrols[c->SubControlCount]);

    return c;
}

int Control__SetBuffer(Control *c, char *BUFF) {
    if(!c || !BUFF)
        return 0;

    c->Text = BUFF;
    return 1;
}

int SetStyle(Control *c, char **style) {
    if(!c || !style)
        return 0;

    c->CSS = style;
    return 1;
}

int AppendControl(Control *c, Control *new_control) {
    if(!c || !new_control)
        return 0;

    c->SubControls[c->SubControlCount] = new_control;
    c->SubControlCount++;
    c->SubControls = (void **)realloc(c->SubControls, sizeof(void *) * (c->SubControlCount + 1));
    c->SubControls[c->SubControlCount] = NULL;

    return 1;
}

int AppendCSS(Control *c, char *new_css) {
    if(!c || !new_css)
        return 0;
    
    c->CSS[c->CSSCount] = strdup(new_css);
    c->CSSCount++;
    c->CSS = (char **)realloc(c->CSS, sizeof(char *) * (c->CSSCount + 1));
    c->CSS[c->CSSCount] = NULL;

    return 1;
}

int Control__AppendStackControl(Control *c, Control new_control) {
    if(!c)
        return 0;

    Control *newc = (Control *)malloc(sizeof(Control));
    memset(newc, '\0', sizeof(Control));

    newc->Parent            = new_control.Parent;
    newc->Tag               = new_control.Tag;
    newc->ID                = (!new_control.ID ? NULL : strdup(new_control.ID));
    newc->Name              = (!new_control.Name ? NULL : strdup(new_control.Name));
    newc->Type              = (!new_control.Type ? NULL : strdup(new_control.Type));
    newc->Text              = (!new_control.Text ? NULL : strdup(new_control.Text));
    newc->Class             = (!new_control.Class ? NULL : strdup(new_control.Class));
    newc->href              = (!new_control.href ? NULL : strdup(new_control.href));
    newc->CSS               = new_control.CSS;
    newc->CSSCount          = new_control.CSSCount;
    newc->Data              = (!new_control.Data ? NULL : strdup(new_control.Data));
    newc->OnClick           = new_control.OnClick;
    newc->OnClickJS         = (!new_control.OnClickJS ? NULL : strdup(new_control.OnClickJS));
    newc->FormID            = (!new_control.FormID ? NULL : strdup(new_control.FormID));
    newc->DisplayID         = (!new_control.DisplayID ? NULL : strdup(new_control.DisplayID));
    newc->SubControls       = new_control.SubControls;
    newc->SubControlCount   = new_control.SubControlCount;

    AppendControl(c, newc);
    return 1;
}

int Control__AppendControlAt(Control *c, int pos, Control *new_control) {
    Control **arr = (Control **)malloc(sizeof(Control *) * 1);
    int idx = 0;

    for(int i = 0; i < c->SubControlCount; i++) {
        if(!c->SubControls[i])
            break;

        if(i == pos) {
            arr[idx] = new_control;
            idx++;
            arr = (Control **)realloc(arr, sizeof(Control *) * (idx + 1));
        }

        arr[idx] = c->SubControls[i];
        idx++;
        arr = (Control **)realloc(arr, sizeof(Control *) * (idx + 1));
        arr[idx] = NULL;
    }

    if(c->SubControls != NULL)
        free(c->SubControls);
        
    c->SubControls = (void **)arr;
    c->SubControlCount = idx;

    return 1;
}

char *create_index_line(int len) {
    if(len == 0)
        return NULL;

    char *BUFF = (char *)malloc(len + 1);
    memset(BUFF, '\0', len + 1);

    for(int i = 0; i < len; i++)
        strcat(BUFF, "\t");

    BUFF[strlen(BUFF) - 1] = '\0';
    return BUFF;
}

String ConstructControl(Control *c, int sub) {
    if(!c)
        return ((String){});

    /* Start w/ <html> and the main tag */
    String design = (sub == 0 ? NewString(create_index_line(sub)) : NewString(NULL));
    sub == 0 ? design.AppendString(&design, "<") : 0;

    /* Main Tag */
    char *main_tag = FindTag(c);

    /* Construct Parent Control */
    if(sub == 0) {
        design.AppendArray(&design, (const char *[]){main_tag, " ", NULL});
        {
            /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
            if(c->ID)
                design.AppendArray(&design, (const char *[]){ " id=\"", c->ID, "\" ", NULL});
                
            if(c->Name)
                design.AppendArray(&design, (const char *[]){ " name=\"", c->Name, "\" ", NULL});

            if(c->Type)
                design.AppendArray(&design, (const char *[]){" type=\"", c->Type, "\"", NULL});

            if(c->Data)
                design.AppendArray(&design, (const char *[]){" ", c->Data, NULL});

            if(c->Class) 
                design.AppendArray(&design, (const char *[]){" class=\"", c->Class, "\"", NULL});

            if(c->href)
                design.AppendArray(&design, (const char *[]){" href=\"", c->href, "\"", NULL});
            
            if(c->CSS) {
                design.AppendString(&design, " style=\"");
                design.AppendArray(&design, (const char **)c->CSS);
                design.AppendString(&design, "\"");
            }

            if(c->OnClick || c->FormID) {
                String onclick_js = ConstructOnClickForm(c);
                design.AppendArray(&design, (const char *[]){"onclick=\"", onclick_js.data, "\" ", NULL});
            }
            
            design.AppendString(&design, c->Tag == INPUT_TAG ? "/>\n" : ">\n");
        }
    }

    if(c->Text)
        design.AppendString(&design, c->Text);

    /* Construct SubControls */
    char *sub_tag = NULL;
    if (c->SubControlCount > 0) {
        for (int i = 0; c->SubControls[i] != NULL; i++) {
            Control *subControl = (Control *)c->SubControls[i];
            design.AppendString(&design, create_index_line(sub + 1));
            design.AppendString(&design, "<");
            sub_tag = FindTag(subControl);
            if (!sub_tag) break; 

            design.AppendString(&design, sub_tag);

            if(subControl->ID)
                design.AppendArray(&design, (const char *[]){" id=\"", subControl->ID, "\" ", NULL});

            if(subControl->Name)
                design.AppendArray(&design, (const char *[]){" name=\"", subControl->Name, "\" ", NULL});

            if (subControl->Type)
                design.AppendArray(&design, (const char *[]){" type=\"", subControl->Type, "\"", NULL});

            if (subControl->Data)
                design.AppendArray(&design, (const char *[]){" ", subControl->Data, NULL});

            if(subControl->Class) 
                design.AppendArray(&design, (const char *[]){" class=\"", subControl->Class, "\"", NULL});

            if(subControl->href)
                design.AppendArray(&design, (const char *[]){" href=\"", subControl->href, "\"", NULL});

            if (subControl->CSS) {
                design.AppendString(&design, " style=\"");
                design.AppendArray(&design, (const char **)subControl->CSS);
                design.AppendString(&design, "\"");
            }

            if(subControl->OnClick && subControl->FormID) {
                String onclick_js = ConstructOnClickForm(subControl);
                design.AppendArray(&design, (const char *[]){" onclick=\"", onclick_js.data, "\" ", NULL});
            }

            design.AppendString(&design, subControl->Tag == INPUT_TAG ? "/>\n" : ">\n");

            if (subControl->Text)
                design.AppendArray(&design, (const char *[]){create_index_line(sub + 2), subControl->Text, "\n", create_index_line(sub + 1), "</", sub_tag, ">\n", NULL});
            if (subControl->SubControlCount > 0) {
                String n = ConstructControl(subControl, sub + 1);
                design.AppendArray(&design, (const char *[]){n.data, NULL});
                n.Destruct(&n);
            }
        }
    }

    /* End Parent Control */
    design.AppendArray(&design, (const char *[]){create_index_line(sub), "</", main_tag, ">\n", NULL});

    design.data[design.idx] = '\0';
    if(design.idx > 0)
        return design;

    return ((String){});
}

void DestructControl(Control *c, int del_control, int del_styles) {
    if(!c)
        return;

    if(c->ID)
        free(c->ID);

    if(c->Name)
        free(c->Name);

    if(c->Type)
        free(c->Type);

    if(c->Text)
        free(c->Text);

    if(c->Class)
        free(c->Class);

    if(c->href)
        free(c->href);

    if(c->Data)
        free(c->Data);

    if(c->OnClickJS)
        free(c->OnClickJS);

    if(c->FormID)
        free(c->FormID);

    if(c->DisplayID)
        free(c->DisplayID);

    if(del_styles && c->CSS) {
        for(int i = 0; i< c->CSSCount; i++) {
            free(c->CSS[i]);
            c->CSS[i] = NULL;
        }
        free(c->CSS);
    }

    free(c);
}