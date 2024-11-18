#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

#define HTML_TAGS_COUNT 3
void *HTML_TAGS[][3] = {
    { (void *)HEAD_TAG, "head"},
    { (void *)BODY_TAG, "body" },
    { (void *)P_TAG, "p" },
};

char *FindTag(Control *control) {
    for(int i = 0; i < HTML_TAGS_COUNT; i++) {
        if((ControlTag)control->Tag == (ControlTag)HTML_TAGS[i][0]) {
            return HTML_TAGS[i][1];
        }
    }

    return NULL;
}

char *ExecuteConstructor(Control *control) {
    if(!control)
        return NULL;

    for(int i = 0; i < HTML_TAGS_COUNT; i++) {
        if((ControlTag)control->Tag == (ControlTag)HTML_TAGS[i][0])
            return ConstructTag(control);
    }

    return NULL;
}

String ConstructDesign(Control **controls) {
    if(!controls)
        return ((String){});

    /* Start w/ <html> and the main tag */
    String design = NewString("<");

    /* Main Tag */
    char *main_tag = FindTag(controls[0]);
    design.AppendString(&design, main_tag);

    if(controls[0]->Data)
        design.AppendArray(&design, (const char *[]){" ", controls[0]->Data, NULL});
    
    if(controls[0]->CSS) {
        design.AppendString(&design, " style=\"");
        design.AppendArray(&design, (const char **)controls[0]->CSS);
        design.AppendString(&design, "\"");
    }

    design.AppendString(&design, ">\n");

    /* Start Iterating Controls */
    int i = 1;

    while(controls[i] != NULL) {
        char *data = ExecuteConstructor(controls[i]);
        if(!data)
            continue;

        design.AppendArray(&design, (const char *[]){data, NULL});
        i++;
    }


    design.AppendArray(&design, (const char *[]){"</", main_tag, ">", NULL});
    design.data[design.idx] = '\0';
    if(design.idx > 0)
        return design;

    return ((String){});
}

char *ConstructTag(Control *control) {
    String buffer = NewString("<");
    
    char *tag = FindTag(control);
    buffer.AppendString(&buffer, tag);

    if(control->Data)
        buffer.AppendArray(&buffer, (const char *[]){" ", control->Data, NULL});
    
    if(control->CSS) {
        buffer.AppendString(&buffer, " style=\"");
        buffer.AppendArray(&buffer, (const char **)control->CSS);
        buffer.AppendString(&buffer, "\"");
    }
    
    buffer.AppendString(&buffer, ">");

    if(control->Text)
        buffer.AppendArray(&buffer, (const char *[]){"\n", control->Text, NULL});

    buffer.AppendArray(&buffer, (const char *[]){"\n</", tag, ">\n", NULL});

    char *output = strdup(buffer.data);
    buffer.Destruct(&buffer);

    return output;
}