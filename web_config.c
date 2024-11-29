#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

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

int AddCSS(WebRoute *route, char *name, void *css_function) {
    if(!route || !name || !css_function)
        return 0;

    route->CSS[route->CSS_Count] = (char **)malloc(sizeof(char *) * 2);
    memset(route->CSS[route->CSS_Count], '\0', sizeof(char *) * 2);
    route->CSS[route->CSS_Count][0] = strdup(name);
    route->CSS[route->CSS_Count][1] = css_function;
    
    return 1;
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

int AddRoute(cWS *web, WebRoute route) {
    if(!web || !route.Name)
        return 0;

    WebRoute *neww = (WebRoute *)malloc(sizeof(WebRoute));
    *neww = route;

    neww->Destruct = DestroyRoute;

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

    if(r->Path)
        free(r->Path);

    if(r->Output)
        free(r->Output);

    if(r->Controls) {
        for(int i = 0; i < r->ControlCount; i++) {
            free(r->Controls[i]);
            r->Controls[i] = NULL; // No dangling pointers
        }

        free(r->Controls);
    }

    free(r);
}