#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

char *WS_JS_HANDLER = "const parseForm = (formId) => Array.from(document.getElementById(formId).querySelectorAll('[id]')).reduce((formData, element) => ({ ...formData, [element.id]: element.value || element.innerText || '' }), {});\nconst submitForm = async (formId) => (await (async (jsonData) => await fetch(window.location.href, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(jsonData) }))(parseForm(formId))).ok ? console.log('Form submitted successfully:', await response.json()) : console.error('Form submission failed:', response.statusText);";

/* 
    TODO: 

        - Create a dynamic event handler.
            WebJS Function to handle actions such as retrieve elements,
            values, etc

        - Add this dynamic event handler after adding routes

        - A set of WebJS function tools will also be added with this
            - MouseControl.js   (Handle Mouse Events)
            - ClickControl.js   (Handle Click Events)
            - ElementControl.js (Add/Remove elements && Update element size, location, color etc)
            - CustomEventCustomizer.js

    NOTE:

        - This route will retrieve something similiar to the content below
        {
            "url": "",
            "path": "",
            "action": "click",
            "element": {
                "tag": "input"
                "id": "submit_btn",
                "class": "red_submit_btn"
            }
            "timestamp": ""
        }
           
*/
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
        neww->Destruct      = DestroyWebRoute;
        neww->ConstructCHT  = ConstructTemplate;
    }

    web->CFG.Routes[web->CFG.RouteCount] = neww;
    web->CFG.RouteCount++;
    web->CFG.Routes = (WebRoute **)realloc(web->CFG.Routes, sizeof(WebRoute *) * (web->CFG.RouteCount + 1));

    
    web->CFG.Routes[web->CFG.RouteCount] = NULL;
    return 1;
}

int AddRoutePtr(cWS *web, WebRoute *route) {
    if(!web || !route->Name)
        return 0;

    web->CFG.Routes[web->CFG.RouteCount] = route;
    web->CFG.RouteCount++;
    web->CFG.Routes = (WebRoute **)realloc(web->CFG.Routes, sizeof(WebRoute *) * (web->CFG.RouteCount + 1));

    
    web->CFG.Routes[web->CFG.RouteCount] = NULL;
    return 1;
}

String control2str(Control *p) {
    if(!p)
        return ((String){});

    String info = NewString("{ ");

    if((int)p->Tag != 0) {
        char *Find = strdup(FindTag(p));
        info.AppendArray(&info, (const char *[]){"Tag=\"", Find, "\" ", NULL});
        free(Find);
    }

    if(p->ID)
        info.AppendArray(&info, (const char *[]){"id=\"", p->ID, "\" ", NULL});

    if(p->Type)
        info.AppendArray(&info, (const char *[]){"type=\"", p->Type, "\" ", NULL});

    if(p->Class)
        info.AppendArray(&info, (const char *[]){"class=\"", p->Class, "\" ", NULL});

    if(p->href)
        info.AppendArray(&info, (const char *[]){"href=\"", p->href, "\" ", NULL});

    if(p->Text)
        info.AppendArray(&info, (const char *[]){"Text=\"", p->Text, "\" ", NULL});

    if(p->Data)
        info.AppendString(&info, p->Data);

    if(p->Script) {
        info.AppendArray(&info, (const char *[]){"onclick=\"", p->Script, "\" ", NULL});
    }
    
    info.AppendString(&info, "}");
    info.data[info.idx] = '\0';
    return info;
}

void DestroyCfg(WebServerConfig *cfg) {
    if(cfg->Routes) {
        for(int i = 0; i < cfg->RouteCount; i++) {
            cfg->Routes[i]->Destruct(cfg->Routes[i]);
        }
        
        free(cfg->Routes);
    }

    if(cfg->Index)
        free(cfg->Index);
}