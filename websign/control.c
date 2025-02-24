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

        .Construct          = ConstructControl,
        .Destruct           = DestructControl
    };

    memset(c->SubControls, '\0', sizeof(void *) * 1);
    if(!subcontrols)
        return c;

    while(subcontrols[c->SubControlCount] != NULL)
        AppendControl(c, subcontrols[c->SubControlCount]);

    return c;
}

int SetBuffer(Control *c, char *BUFF) {
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

    return 1;
}

String ConstructControl(Control *c, int sub) {
    if(!c)
        return ((String){});

    /* Start w/ <html> and the main tag */
    String design = (sub == 0 ? NewString("<") : NewString(NULL));

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
                design.AppendArray(&design, (const char *[]){ " id=\"", c->Name, "\" ", NULL});

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
            design.AppendString(&design, "<");
            sub_tag = FindTag(subControl);
            if (!sub_tag) break; 

            design.AppendString(&design, sub_tag);

            if(subControl->ID)
                design.AppendArray(&design, (const char *[]){" id=\"", subControl->ID, "\" ", NULL});

            if(subControl->Name)
                design.AppendArray(&design, (const char *[]){" id=\"", subControl->Name, "\" ", NULL});

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
                design.AppendArray(&design, (const char *[]){subControl->Text, "\n", "</", sub_tag, ">\n", NULL});
            if (subControl->SubControlCount > 0) {
                String n = ConstructControl(subControl, 1);
                design.AppendArray(&design, (const char *[]){n.data, NULL});
                n.Destruct(&n);
            }
        }
    }

    /* End Parent Control */
    design.AppendArray(&design, (const char *[]){"</", main_tag, ">\n", NULL});

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

    if(del_control && c->SubControls) {
        for(int i = 0; i < c->SubControlCount; i++) {
            free(c->SubControls[i]);
            c->SubControls[i] = NULL;
        }
        free(c->SubControls);
    }

    if(del_styles && c->CSS) {
        for(int i = 0; i< c->CSSCount; i++) {
            free(c->CSS[i]);
            c->CSS[i] = NULL;
        }
        free(c->CSS);
    }

    free(c);
}