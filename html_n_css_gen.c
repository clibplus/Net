/*
*      [ Websign's Browser SDK (The Magic Browser Lib) ]
*
* - Serving as a browser SDK to generate template supporting
*   everything a browser takes
*
* - ALL HTML Tag as ENUM since no often HTML updates and//or changes
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

#define HTML_TAGS_COUNT 14
#define WS_TAGS_COUNT 3

char *JS_HANDLER_FORMAT = "<script src=\"ws_form_handler.js\"></script><br /><script>document.getElementById('[SUBMIT_BUTTON]').addEventListener('click', () => submitForm('[FORM_ID]'));\n</script>";
char *JS_HANDLER[] = {"(()=>(f=new FormData(document.getElementById('", "')),d={},f.forEach((v,k)=>d[k]=v),fetch(window.location.href,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(d)}).then(r=>r.json()).then(r=>{console.log('Response:',r);alert('POST Request successful!')}).catch(e=>{console.error('Error:',e);alert('POST Request failed!')})))();"};

void *HTML_TAGS[][2] = {
    { (void *)HTML_TAG,     "html" },
    { (void *)HEAD_TAG,     "head"},
    { (void *)TITLE_TAG,    "title" },
    { (void *)BODY_TAG,     "body" },
    { (void *)DIV_TAG,      "div" },
    { (void *)H1_TAG,       "h1" },
    { (void *)H2_TAG,       "h2" },
    { (void *)H3_TAG,       "h3" },
    { (void *)P_TAG,        "p" },
    { (void *)A_TAG,        "a" },
    { (void *)PRE_TAG,      "pre" },
    { (void *)BUTTON_TAG,   "button" },
    { (void *)INPUT_TAG,    "input" },
    { (void *)FORM_TAG,     "form" }
};

char *FindTag(Control *control) {
    for(int i = 0; i < HTML_TAGS_COUNT; i++)
        if((ControlTag)control->Tag == (ControlTag)HTML_TAGS[i][0])
            return HTML_TAGS[i][1];

    return NULL;
}

char *ConstructCSS(WebRoute *route) {
    if(!route->CSS)
        return NULL;

    String BUFFER = NewString("<style>\n");

    int i = 0;
    while(route->CSS[i] != NULL) {
        if(route->CSS[i]->Selector)
            BUFFER.AppendString(&BUFFER, ".");

        BUFFER.AppendArray(&BUFFER, (const char *[]){route->CSS[i]->Class, " {\n", NULL});
        int css_idx = 0;
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
    printf("Adding CSS....\n");
    if(!route->Controls || !route->Controls[0]) {
        printf("ERROR\n");
        return 0;
    }

    if(route->Controls[0]->Tag == HEAD_TAG) {
        String header = ConstructParent(route->Controls[0], 0);
        template.AppendArray(&template, (const char *[]){header.data, "\n\n", NULL});
        header.Destruct(&header);

        char *data = ConstructCSS(route);
        template.AppendArray(&template, (const char *[]){data, "\n", NULL});
        free(data);

        i = 1;
    }

    printf("Adding controls...\n");
    while(route->Controls[i] != NULL) {
        String new = ConstructParent(route->Controls[i], 0);
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

String ConstructParent(Control *p, int sub) {
    if(!p)
        return ((String){});

        
    /* Start w/ <html> and the main tag */
    String design = (sub == 0 ? NewString("<") : NewString(NULL));

    /* Main Tag */
    char *main_tag = FindTag(p);

    /* Construct Parent Control */
    if(sub == 0) {
        design.AppendArray(&design, (const char *[]){main_tag, " ", NULL});

        {
            /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
            if(p->ID)
                design.AppendArray(&design, (const char *[]){ " id=\"", p->ID, "\" ", NULL});

            if(p->Type)
                design.AppendArray(&design, (const char *[]){" type=\"", p->Type, "\"", NULL});

            if(p->Data)
                design.AppendArray(&design, (const char *[]){" ", p->Data, NULL});

            if(p->Class) 
                design.AppendArray(&design, (const char *[]){" class=\"", p->Class, "\"", NULL});

            if(p->href)
                design.AppendArray(&design, (const char *[]){" href=\"", p->href, "\"", NULL});
            
            if(p->CSS) {
                design.AppendString(&design, " style=\"");
                design.AppendArray(&design, (const char **)p->CSS);
                design.AppendString(&design, "\"");
            }

            if(p->OnClick && p->FormID) {
                // add here
                String JS_CODE = NewString(JS_HANDLER[0]);
                JS_CODE.AppendArray(&JS_CODE,  (const char *[]){p->FormID, JS_HANDLER[1], NULL});

                JS_CODE.data[JS_CODE.idx] = '\0';
                design.AppendArray(&design, (const char *[]){"onclick=\"", JS_CODE.data, "\" ", NULL});
                JS_CODE.Destruct(&JS_CODE);
            }

            design.AppendString(&design, p->Tag == INPUT_TAG ? "/>\n" : ">\n");
        }
    }

    if(p->Text)
        design.AppendString(&design, p->Text);

    /* Construct SubControls */
    char *sub_tag = NULL;
    if (p->SubControls != NULL) {
        for (int i = 0; p->SubControls[i] != NULL; i++) {
            Control *subControl = (Control *)p->SubControls[i];
            design.AppendString(&design, "<");
            sub_tag = FindTag(subControl) ? FindTag(subControl) : FindWSTag(subControl);
            if (!sub_tag) continue; 

            design.AppendString(&design, sub_tag);

            if(subControl->ID)
                design.AppendArray(&design, (const char *[]){" id=\"", subControl->ID, "\" ", NULL});

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
                // add here
                String JS_CODE = NewString(JS_HANDLER[0]);
                JS_CODE.AppendArray(&JS_CODE,  (const char *[]){subControl->FormID, JS_HANDLER[1], NULL});
                JS_CODE.data[JS_CODE.idx] = '\0';

                design.AppendArray(&design, (const char *[]){"onclick=\"", JS_CODE.data, "\" ", NULL});
                JS_CODE.Destruct(&JS_CODE);
            }

            design.AppendString(&design, subControl->Tag == INPUT_TAG ? "/>\n" : ">\n");

            if (subControl->Text)
                design.AppendArray(&design, (const char *[]){subControl->Text, "\n", "</", sub_tag, ">\n", NULL});
            if (subControl->SubControls != NULL) {
                String n = ConstructParent(subControl, 1);
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