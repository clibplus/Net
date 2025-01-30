#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

WebRoute *CreateRoute(const char *n, const char *p, void *handler) {
    WebRoute *w = (WebRoute *)malloc(sizeof(WebRoute));
    *w = (WebRoute){
        .Name = n,
        .Path = p,
        .Handler = handler,
        .Destruct = DestroyWebRoute
    };

    return w;
}

int SetReadOnly(WebRoute *w, const char *data) {
    if(!w || !data)
        return 0;

    w->Template = strdup(data);
    w->ReadOnly = 1;

    return 1;
}

void DestroyWebRoute(WebRoute *w) {
    if(w->Name)
        free(w->Name);

    if(w->Path)
        free(w->Path);

    if(w->Template)
        free(w->Template);

    if(w->CSS) {
        for(int i = 0; i < CSS_Count; i++)
            if(w->CSS[i])
                free(w->CSS[i]);

        free(w->CSS);
    }
    
    if(w->Controls) {
        for(int i = 0; i < w->ControlCount; i++)
            if(w->Controls[i])
                free(w->Controls[i]);

        free(w->Controls);
    }

    free(w);
}