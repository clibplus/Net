#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

#define HTML_TAGS_COUNT 8
void *HTML_TAGS[][2] = {
    { (void *)HTML_TAG,     "html" },
    { (void *)HEAD_TAG,     "head"},
    { (void *)TITLE_TAG,    "title" },
    { (void *)BODY_TAG,     "body" },
    { (void *)DIV_TAG,      "div" },
    { (void *)H1_TAG,       "h1" },
    { (void *)P_TAG,        "p" },
    { (void *)A_TAG,        "a" }
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

    {
        /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
        if(controls[0]->Type)
            design.AppendArray(&design, (const char *[]){" type=\"", controls[0]->Type, "\"", NULL});

        if(controls[0]->Data)
            design.AppendArray(&design, (const char *[]){" ", controls[0]->Data, NULL});
        
        if(controls[0]->CSS) {
            design.AppendString(&design, " style=\"");
            design.AppendArray(&design, (const char **)controls[0]->CSS);
            design.AppendString(&design, "\"");
        }

        design.AppendString(&design, ">\n");
    }

    /* Start Iterating Controls */
    int i = 1;

    while(controls[i] != NULL) {
        char *data = ExecuteConstructor(controls[i]);
        if(!data)
            continue;

        design.AppendArray(&design, (const char *[]){data, NULL});
        controls[i]->Parent = controls[0];
        i++;
    }


    design.AppendArray(&design, (const char *[]){"</", main_tag, ">\n\n", NULL});
    design.data[design.idx] = '\0';
    if(design.idx > 0)
        return design;

    return ((String){});
}

String ConstructControl(Control *control) {
    if(!control)
        return ((String){});

    /* Start w/ <html> and the main tag */
    String design = NewString("<");

    /* Main Tag */
    char *main_tag = FindTag(control);
    design.AppendString(&design, main_tag);

    {
        /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
        if(control->Type)
            design.AppendArray(&design, (const char *[]){" type=\"", control->Type, "\"", NULL});

        if(control->Data)
            design.AppendArray(&design, (const char *[]){" ", control->Data, NULL});
        
        if(control->CSS) {
            design.AppendString(&design, " style=\"");
            design.AppendArray(&design, (const char **)control->CSS);
            design.AppendString(&design, "\"");
        }

        design.AppendString(&design, ">\n");
    }

    /* Start Iterating Controls */
    int i = 0;
    while(control->SubControls[i] != NULL) {
        char *data = ExecuteConstructor(control->SubControls[i]);
        if(!data)
            continue;

        design.AppendArray(&design, (const char *[]){data, NULL});
        ((Control *)control->SubControls[i])->Parent = control;
        i++;
    }


    design.AppendArray(&design, (const char *[]){"</", main_tag, ">\n\n", NULL});
    design.data[design.idx] = '\0';
    if(design.idx > 0)
        return design;

    return ((String){});
}

String ConstructParent(Control *p) {
    if(!p)
        return ((String){});

        
    /* Start w/ <html> and the main tag */
    String design = NewString("<");

    /* Main Tag */
    char *main_tag = FindTag(p);
    design.AppendString(&design, main_tag);

    {
        /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
        if(p->Type)
            design.AppendArray(&design, (const char *[]){" type=\"", p->Type, "\"", NULL});

        if(p->Data)
            design.AppendArray(&design, (const char *[]){" ", p->Data, NULL});
        
        if(p->CSS) {
            design.AppendString(&design, " style=\"");
            design.AppendArray(&design, (const char **)p->CSS);
            design.AppendString(&design, "\"");
        }

        design.AppendString(&design, ">\n");
    }

    int count = 0;
    if (p->SubControls != NULL) {
        for (int i = 0; p->SubControls[i] != NULL; i++) {
            Control *subControl = (Control *)p->SubControls[i];
            design.AppendString(&design, "<");
            char *sub_tag = FindTag(subControl);
            if (!sub_tag) continue;  // Skip invalid tags.

            design.AppendString(&design, sub_tag);

            // Add attributes for sub-controls
            if (subControl->Type)
                design.AppendArray(&design, (const char *[]){" type=\"", subControl->Type, "\"", NULL});

            if (subControl->Data)
                design.AppendArray(&design, (const char *[]){" ", subControl->Data, NULL});

            if (subControl->CSS) {
                design.AppendString(&design, " style=\"");
                design.AppendArray(&design, (const char **)subControl->CSS);
                design.AppendString(&design, "\"");
            }
            design.AppendString(&design, ">");

            // Add text or recurse
            if (subControl->Text)
                design.AppendArray(&design, (const char *[]){"\n", subControl->Text, "\n", NULL});
            if (subControl->SubControls != NULL) {
                String n = ConstructParent(subControl);
                design.AppendArray(&design, (const char *[]){n.data, NULL});
                n.Destruct(&n);  // Free memory for the temporary String.
            }
        }

        // Close sub-tags
        for (int i = 0; p->SubControls[i] != NULL; i++) {
            char *sub_tag = FindTag(p->SubControls[i]);
            if (sub_tag)
                design.AppendArray(&design, (const char *[]){"</", sub_tag, ">\n", NULL});
        }
    }

    
    design.AppendArray(&design, (const char *[]){"</", main_tag, ">\n\n", NULL});

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
    if(!output)
        return NULL;

    buffer.Destruct(&buffer);

    return output;
}