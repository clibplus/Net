#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

#define HTML_TAGS_COUNT 8
void *HTML_TAGS[][2] = {
    { (void *)HTML_TAG,     "html" },
    { (void *)HEAD_TAG,     "head"},
    { (void *)TITLE_TAG,    "title" },
    { (void *)BODY_TAG,     "body" },
    { (void *)DIV_TAG,      "div" },
    { (void *)H1_TAG,       "h1" },
    { (void *)P_TAG,        "p" },
    { (void *)A_TAG,        "a" }
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

    char *BUFF = (char *)malloc(1);
    int idx = 0;

    for (int i = 0; route->CSS[i][0] != NULL; i++) {
        const char *selector = (const char *)route->CSS[i][0];
        const char **styles = (const char **)route->CSS[i][1];
        if(!selector || !styles)
            break;

        idx += 1 + strlen(selector) + 3;
        BUFF = (char *)realloc(BUFF, idx);
        strcat(BUFF, ".");
        strcat(BUFF, selector);
        strcat(BUFF, " { \n");

        for (int j = 0; styles[j] != NULL; j++) {
            idx += 1 + strlen(styles[j]) + 3;
            BUFF = (char *)realloc(BUFF, idx);
            strcat(BUFF, "\t");
            strcat(BUFF, styles[j]);
            strcat(BUFF, ";\n");
        }

        idx += 3;
        BUFF = (char *)realloc(BUFF, idx + strlen("}\n"));
        strcat(BUFF, "}\n");
    }

    BUFF[idx] = '\0';
    return BUFF;
}

int ConstructTemplate(WebRoute *route) {
    if(!route)
        return 0;

    String template = NewString("<html>\n");

    int i = 0;
    while(route->Controls[i] != NULL) {
        String new = ConstructParent(route->Controls[i], 0);
        // if(i == 1) {
        //     char *CSS_SELECTOR = ConstructCSS(route);
        //     template.AppendArray(&template, (const char *[]){CSS_SELECTOR, "\n", NULL});
        //     if(CSS_SELECTOR != NULL)
        //         free(CSS_SELECTOR);
        // }

        template.AppendArray(&template, (const char *[]){new.data, "\n", NULL});
        i++;
    }
    template.AppendString(&template, "</html>\n");
    template.data[template.idx] = '\0';

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
                design.AppendArray(&design, (const char *[]){n.data, "\n", NULL});
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