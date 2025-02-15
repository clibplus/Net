/*
*      [ Websign's Browser SDK (The Magic Browser Lib) ]
*
* - Serving as a browser SDK to generate template supporting
*   everything a browser takes
*
* - ALL HTML Tag as ENUM since no often HTML updates and//or changes
*
*
* - Install Commands
*
* gcc -c web.c web_config.c html_n_css_gen.c html_parser.c wjs_gen.c ws_css.c web_route.c -lstr -larr -lmap -lpthread -g -g1
* ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

#define HTML_TAGS_COUNT 15
#define WS_TAGS_COUNT 3

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
    { (void *)FORM_TAG,     "form" },
    { (void *)IMG_TAG,      "img" },
    NULL
};

char *FindTag(Control *control) {
    for(int i = 0; i < HTML_TAGS_COUNT; i++)
        if((ControlTag)control->Tag == (ControlTag)HTML_TAGS[i][0])
            return HTML_TAGS[i][1];

    return NULL;
}

ControlTag FindTagType(const char *data) {
    if(!data)
        return 0;
        
    for(int i = 0; i < HTML_TAGS_COUNT; i++)
        if(!strcmp(data, (char *)HTML_TAGS[i][1]))
            return (ControlTag)HTML_TAGS[i][0];

    return 0;
}

// Moved to `web_route.c`
// char *ConstructCSS(WebRoute *route) {
//     if(!route->CSS)
//         return NULL;

//     String BUFFER = NewString("<style>\n");
//     int i = 0, css_idx = 0;

//     while(route->CSS[i] != NULL) {
//         if(route->CSS[i]->Selector)
//             BUFFER.AppendString(&BUFFER, ".");

//         BUFFER.AppendArray(&BUFFER, (const char *[]){route->CSS[i]->Class, " {\n", NULL});

//         css_idx = 0;
//         while((const char **)route->CSS[i]->Data[css_idx] != NULL) {
//             BUFFER.AppendArray(&BUFFER, (const char *[]){route->CSS[i]->Data[css_idx], ";", NULL});;
//             css_idx++;
//         }

//         BUFFER.AppendString(&BUFFER, "\n}\n\0");
//         i++;
//     }

//     BUFFER.AppendString(&BUFFER, "</style>\n");
//     BUFFER.data[BUFFER.idx] = '\0';
    
//     char *BUFF = strdup(BUFFER.data);
//     BUFFER.Destruct(&BUFFER);

//     return BUFF;
// }

// // Moved to `web_route.c`
// int ConstructTemplate(WebRoute *route, Control **controls, CSS **styles) {
//     route->Controls = controls;
//     route->CSS = styles;
//     String template = NewString("<html>\n");

//     int i = 0;
//     if(!route->Controls || !route->Controls[0])
//         return 0;

//     if(route->Controls[0]->Tag == HEAD_TAG) {
//         String header = ConstructParent(route->Controls[0], 0);
//         template.AppendArray(&template, (const char *[]){header.data, "\n\n", NULL});
//         header.Destruct(&header);

//         if(styles != NULL && route->CSS != NULL) {
//             char *data = ConstructCSS(route);
//             template.AppendArray(&template, (const char *[]){data, "\n", NULL});
//             free(data);
//         }

//         i = 1;
//     }

//     while(route->Controls[i] != NULL) {
//         String new = ConstructParent(route->Controls[i], 0);
//         template.AppendArray(&template, (const char *[]){new.data, "\n", NULL});
//         i++;
//     }
    
//     template.AppendString(&template, "</html>\n\n\n");
//     template.data[template.idx] = '\0';
//     if(route->Template)
//         free(route->Template);

//     route->Template = strdup(template.data);
//     return 1;
// }


// // Moved to `controls.c`
// String ConstructParent(Control *p, int sub) {
//     if(!p)
//         return ((String){});

        
//     /* Start w/ <html> and the main tag */
//     String design = (sub == 0 ? NewString("<") : NewString(NULL));

//     /* Main Tag */
//     char *main_tag = FindTag(p);

//     /* Construct Parent Control */
//     if(sub == 0) {
//         design.AppendArray(&design, (const char *[]){main_tag, " ", NULL});

//         {
//             /* Construct The Parent Control (<html>\n\n</html> // <head>\n\n</head> // <body>\n\n</body>) */
//             if(p->ID)
//                 design.AppendArray(&design, (const char *[]){ " id=\"", p->ID, "\" ", NULL});

//             if(p->Type)
//                 design.AppendArray(&design, (const char *[]){" type=\"", p->Type, "\"", NULL});

//             if(p->Data)
//                 design.AppendArray(&design, (const char *[]){" ", p->Data, NULL});

//             if(p->Class) 
//                 design.AppendArray(&design, (const char *[]){" class=\"", p->Class, "\"", NULL});

//             if(p->href)
//                 design.AppendArray(&design, (const char *[]){" href=\"", p->href, "\"", NULL});
            
//             if(p->CSS) {
//                 design.AppendString(&design, " style=\"");
//                 design.AppendArray(&design, (const char **)p->CSS);
//                 design.AppendString(&design, "\"");
//             }

//             if(p->OnClick || p->FormID) {
//                 String onclick_js = ConstructOnClickForm(p);
//                 design.AppendArray(&design, (const char *[]){"onclick=\"", onclick_js.data, "\" ", NULL});
//             }
            
//             design.AppendString(&design, p->Tag == INPUT_TAG ? "/>\n" : ">\n");
//         }
//     }

//     if(p->Text)
//         design.AppendString(&design, p->Text);

//     /* Construct SubControls */
//     char *sub_tag = NULL;
//     if (p->SubControlCount > 0) {
//         for (int i = 0; p->SubControls[i] != NULL; i++) {
//             Control *subControl = (Control *)p->SubControls[i];
//             design.AppendString(&design, "<");
//             sub_tag = FindTag(subControl);
//             if (!sub_tag) break; 

//             design.AppendString(&design, sub_tag);

//             if(subControl->ID)
//                 design.AppendArray(&design, (const char *[]){" id=\"", subControl->ID, "\" ", NULL});

//             if (subControl->Type)
//                 design.AppendArray(&design, (const char *[]){" type=\"", subControl->Type, "\"", NULL});

//             if (subControl->Data)
//                 design.AppendArray(&design, (const char *[]){" ", subControl->Data, NULL});

//             if(subControl->Class) 
//                 design.AppendArray(&design, (const char *[]){" class=\"", subControl->Class, "\"", NULL});

//             if(subControl->href)
//                 design.AppendArray(&design, (const char *[]){" href=\"", subControl->href, "\"", NULL});

//             if (subControl->CSS) {
//                 design.AppendString(&design, " style=\"");
//                 design.AppendArray(&design, (const char **)subControl->CSS);
//                 design.AppendString(&design, "\"");
//             }

//             if(subControl->OnClick && subControl->FormID) {
//                 String onclick_js = ConstructOnClickForm(subControl);
//                 design.AppendArray(&design, (const char *[]){" onclick=\"", onclick_js.data, "\" ", NULL});
//             }

//             design.AppendString(&design, subControl->Tag == INPUT_TAG ? "/>\n" : ">\n");

//             if (subControl->Text)
//                 design.AppendArray(&design, (const char *[]){subControl->Text, "\n", "</", sub_tag, ">\n", NULL});
//             if (subControl->SubControlCount > 0) {
//                 String n = ConstructParent(subControl, 1);
//                 design.AppendArray(&design, (const char *[]){n.data, NULL});
//                 n.Destruct(&n);
//             }
//         }
//     }

//     /* End Parent Control */
//     design.AppendArray(&design, (const char *[]){"</", main_tag, ">\n", NULL});

//     design.data[design.idx] = '\0';
//     if(design.idx > 0)
//         return design;

//     return ((String){});
// }