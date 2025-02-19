#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

WebRoute *CreateRoute(const char *n, const char *p, void *handler) {
    WebRoute *w = (WebRoute *)malloc(sizeof(WebRoute));
    *w = (WebRoute){
        .Name = (char *)n,
        .Path = (char *)p,
        .Handler = handler,
<<<<<<< Updated upstream
=======
        .Controls = (Control **)malloc(sizeof(Control *) * 1),
        .ControlCount = 0,
>>>>>>> Stashed changes
        .Destruct = DestroyWebRoute,

        .ConstructCHT = ConstructTemplate
    };

    return w;
}

int AppendParentControl(WebRoute *route, Control *new_c) {
    if(!route)
        return 0;

    route->Controls[route->ControlCount] = new_c;
    route->ControlCount++;
    route->Controls = (Control **)malloc(sizeof(Control *) * (route->ControlCount + 1));
    return 1;
}

int SetReadOnly(WebRoute *w, const char *data) {
    if(!w || !data)
        return 0;

    w->Template = strdup(data);
    w->ReadOnly = 1;

    return 1;
}

char *ConstructCSS(WebRoute *route) {
    if(!route->CSS)
        return NULL;

    String BUFFER = NewString("<style>\n");
    int i = 0, css_idx = 0;

    while(route->CSS[i] != NULL) {
        if(route->CSS[i]->Selector)
            BUFFER.AppendString(&BUFFER, ".");

        BUFFER.AppendArray(&BUFFER, (const char *[]){route->CSS[i]->Class, " {\n", NULL});

        css_idx = 0;
        while((const char **)route->CSS[i]->Data[css_idx] != NULL) {
            BUFFER.AppendArray(&BUFFER, (const char *[]){route->CSS[i]->Data[css_idx], ";", NULL});;
            css_idx++;
        }

        BUFFER.AppendString(&BUFFER, "\n}\n\0");
        i++;
    }

    BUFFER.AppendString(&BUFFER, "</style>\n");
    BUFFER.data[BUFFER.idx] = '\0';
    
    char *BUFF = strdup(BUFFER.data);
    BUFFER.Destruct(&BUFFER);

    return BUFF;
}

int ConstructTemplate(WebRoute *route, Control **controls, CSS **styles) {
    route->Controls = controls;
    route->CSS = styles;
    String template = NewString("<html>\n");

    int i = 0;
    if(!route->Controls || !route->Controls[0])
        return 0;

    if(route->Controls[0]->Tag == HEAD_TAG) {
        String header = ConstructControl(route->Controls[0], 0);
        template.AppendArray(&template, (const char *[]){header.data, "\n\n", NULL});
        header.Destruct(&header);

        if(styles != NULL && route->CSS != NULL) {
            char *data = ConstructCSS(route);
            template.AppendArray(&template, (const char *[]){data, "\n", NULL});
            free(data);
        }

        i = 1;
    }

    while(route->Controls[i] != NULL) {
        String new = ConstructControl(route->Controls[i], 0);
        template.AppendArray(&template, (const char *[]){new.data, "\n", NULL});
        i++;
    }
    
    template.AppendString(&template, "</html>\n\n\n");
    template.data[template.idx] = '\0';
    if(route->Template)
        free(route->Template);

    route->Template = strdup(template.data);
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
        for(int i = 0; i < w->CSS_Count; i++)
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