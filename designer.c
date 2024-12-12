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

#define HTML_TAGS_COUNT 11
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
    { (void *)PRE_TAG,      "pre" }
};

char *FindTag(Control *control) {
    for(int i = 0; i < HTML_TAGS_COUNT; i++) {
        if((ControlTag)control->Tag == (ControlTag)HTML_TAGS[i][0]) {
            return HTML_TAGS[i][1];
        }
    }

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

int ConstructTemplate(WebRoute *route) {
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
    if(sub == 0) {
        design.AppendString(&design, main_tag);

        {
            /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
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

            design.AppendString(&design, ">\n");
        }
    }

    int count = 0;
    char *sub_tag = NULL;
    if (p->SubControls != NULL) {
        for (int i = 0; p->SubControls[i] != NULL; i++) {
            Control *subControl = (Control *)p->SubControls[i];
            design.AppendString(&design, "<");
            sub_tag = FindTag(subControl);
            if (!sub_tag) continue; 

            design.AppendString(&design, sub_tag);

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
            design.AppendString(&design, ">\n");

            if (subControl->Text)
                design.AppendArray(&design, (const char *[]){subControl->Text, "\n", "</", sub_tag, ">\n", NULL});
            if (subControl->SubControls != NULL) {
                String n = ConstructParent(subControl, 1);
                design.AppendArray(&design, (const char *[]){n.data, NULL});
                n.Destruct(&n);
            }
        }
    }

    design.AppendArray(&design, (const char *[]){"</", main_tag, ">\n", NULL});

    design.data[design.idx] = '\0';
    if(design.idx > 0)
        return design;

    return ((String){});
}