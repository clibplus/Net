#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

char *WS_JS_HANDLER = "const parseForm = (formId) => Array.from(document.getElementById(formId).querySelectorAll('[id]')).reduce((formData, element) => ({ ...formData, [element.id]: element.value || element.innerText || '' }), {});",
    "const submitForm = async (formId) => (await (async (jsonData) => await fetch(window.location.href, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(jsonData) }))(parseForm(formId))).ok ? console.log('Form submitted successfully:', await response.json()) : console.error('Form submission failed:', response.statusText);";

int AddDynamicHandler(cWS *web) {
    if(!web)
        return 0;
        
    AddRoute(web, (WebRoute){
        .Name = "Websign's Event Handler",
        .Path = "/ws_js_handler.js",
        .Template = WS_JS_HANDLER,
        .ReadOnly = 1
    });

    return 1;
}

int SearchRoute(cWS *web, const char *data) {
    if(!data)
        return -1;

    String route = NewString(data);
    for(int i = 0; i < web->CFG.RouteCount; i++) {
        if(!web->CFG.Routes[i])
            break;

        WebRoute *_route = web->CFG.Routes[i];
        if(!strcmp(_route->Path, route.data))
            return i;
    }

    return -1;
}

int AddRoutes(cWS *web, WebRoute **routes) {
    if(!web || !routes)
        return 0;

    int i = 0;
    while(routes[i] != NULL) {
        AddRoute(web, *routes[i]);
        i++;
    }

    return 1;
}

int AddCSS(WebRoute *r, void *arr) {
    if(!r)
        return 0;

    r->CSS[r->CSS_Count] = (void *)malloc(sizeof(void));
    r->CSS[r->CSS_Count] = arr;
    r->CSS_Count++;
    r->CSS = (CSS **)realloc(r->CSS, sizeof(CSS *) * (r->CSS_Count + 1));

    return 1;
}

int AddRoute(cWS *web, WebRoute route) {
    if(!web || !route.Name)
        return 0;

    WebRoute *neww = (WebRoute *)malloc(sizeof(WebRoute));
    *neww = route;

    {
        neww->Template      = (char *)malloc(1);
        neww->CSS           = (CSS **)malloc(sizeof(CSS *) * 1);
        neww->CSS_Count     = 0;
        neww->Destruct      = DestroyRoute;
    }

    web->CFG.Routes[web->CFG.RouteCount] = neww;
    web->CFG.RouteCount++;
    web->CFG.Routes = (WebRoute **)realloc(web->CFG.Routes, sizeof(WebRoute *) * (web->CFG.RouteCount + 1));
    return 1;
}

void DestroyCfg(WebServerConfig *cfg) {
    if(cfg->Routes) {
        for(int i = 0; i < cfg->RouteCount; i++) {
            free(cfg->Routes[i]);
            cfg->Routes[i] = NULL;
        }
        free(cfg->Routes);
    }

    if(cfg->Index)
        free(cfg->Index);

    if(cfg->Err404)
        free(cfg->Err404);
}

void DestroyRoute(WebRoute *r) {
    if(r->Name)
        free(r->Name);

    if(r->Template)
        free(r->Template);

    if(r->Path)
        free(r->Path);

    if(r->Controls) {
        for(int i = 0; i < r->ControlCount; i++) {
            free(r->Controls[i]);
            r->Controls[i] = NULL; // No dangling pointers
        }

        free(r->Controls);
    }

    free(r);
}