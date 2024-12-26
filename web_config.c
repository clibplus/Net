#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

char *WS_JS_HANDLER = "const parseForm = (formId) => Array.from(document.getElementById(formId).querySelectorAll('[id]')).reduce((formData, element) => ({ ...formData, [element.id]: element.value || element.innerText || '' }), {});\nconst submitForm = async (formId) => (await (async (jsonData) => await fetch(window.location.href, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(jsonData) }))(parseForm(formId))).ok ? console.log('Form submitted successfully:', await response.json()) : console.error('Form submission failed:', response.statusText);";

String DumpControls(Control *controls, int nestingLevel) {
    String ControlList = NewString(NULL);
    
    /* Add Nested Tabs */
    for (int tabs = 0; tabs < nestingLevel; tabs++) {
        ControlList.AppendString(&ControlList, "\t");
    }
     
    String info = control2str(controls);
    ControlList.AppendArray(&ControlList, (const char *[]){info.data, "\n", NULL});
    info.Destruct(&info);
    
    if(!controls->SubControls) {
        ControlList.data[ControlList.idx] = '\0';
        return ControlList;
    }
    
    int idx = 0;
    while(controls->SubControls[idx] != NULL) {
        if(!controls->SubControls[idx])
            break;

        Control *CurrentControl = controls->SubControls[idx];
        
        /* Add Nested Tabs */
        for (int tabs = 0; tabs < nestingLevel; tabs++) {
            ControlList.AppendString(&ControlList, "\t");
        }

        String nest = DumpControls(CurrentControl, nestingLevel + 1);
        if(nest.idx < 1)
            break;

        ControlList.AppendArray(&ControlList, (const char *[]){nest.data, "\n", NULL});
        nest.Destruct(&nest);
        idx++;
    }

    ControlList.data[ControlList.idx] = '\0';
    return ControlList;
}

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

    
    web->CFG.Routes[web->CFG.RouteCount] = NULL;
    return 1;
}

String control2str(Control *p) {
    if(!p)
        return ((String){});

    String info = NewString("{ ");

    if(p->Tag) {
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

    if(p->OnClick && p->FormID) {
        String onclick_js = ConstructOnClickForm(p);
        info.AppendArray(&info, (const char *[]){"onclick=\"", onclick_js.data, "\" ", NULL});
    }

    if(p->OnClickJS) {
        info.AppendArray(&info, (const char *[]){"onclick=\"", p->OnClickJS, "\" ", NULL});
    }
    
    info.AppendString(&info, "}");
    info.data[info.idx] = '\0';
    return info;
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