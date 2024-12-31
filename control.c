#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

Control NewControl(ControlTag tag, const char *id, Control **subcontrols, char **css) {
    Control c = (Control){
        .Tag = tag,
        .ID = id,
        .SubControls = subcontrols,
        .CSS = css
    };

    while(subcontrols[c.SubControlCount] != NULL)
        c.SubControlCount++;

    while(css[c.CSSCount] != NULL)
        c.CSSCount++;

    return c;
}

void DestructControl(Control *c) {
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

    if(c->SubControls) {
        for(int i = 0; i < c->SubControlCount; i++) {
            free(c->SubControls[i]);
            c->SubControls[i] = NULL;
        }
        free(c->SubControls);
    }

    if(c->CSS) {
        for(int i = 0; i< c->CSSCount; i++) {
            free(c->CSS[i]);
            c->CSS[i] = NULL;
        }
        free(c->CSS);
    }
}