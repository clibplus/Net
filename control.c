#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

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
        .Tag = tag,
        .ID = (!id ? NULL : strdup(id)),
        .Class = (!sclass ? NULL : strdup(sclass)),
        .Text = (!text ? NULL : strdup(text)),
        .SubControls = (void **)malloc(sizeof(void *) * 1),
        .SubControlCount = 0
    };

    memset(c->SubControls, '\0', sizeof(void *) * 1);
    if(!subcontrols)
        return c;

    while(subcontrols[c->SubControlCount] != NULL) {
        AppendControl(c, subcontrols[c->SubControlCount]);
    }

    return c;
}

int AppendControl(Control *c, Control *new_control) {
    if(!c || !new_control)
        return 0;

    c->SubControls[c->SubControlCount] = new_control;
    c->SubControlCount++;
    c->SubControls = (void **)realloc(c->SubControls, sizeof(void *) * (c->SubControlCount + 1));

    return 1;
}

String ConstructControl(Control *c) {

}

void DestructControl(Control *c, int del_control, int del_styles) {
    if(!c)
        return;

    if(c->ID)
        free(c->ID);

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
}