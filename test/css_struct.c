#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <Net/web.h>

const char *HTML_PAGE = "<html>\n"
                  "<head><title>Hello World</title></head>\n"
                  "<body>\n"
                  "   <p>Hello World</p>\n"
                  "</body>\n"
                  "</html>\n\n";

Control Head = (Control){ .Tag = HEAD_TAG, .SubControlCount = 1, SubControl = (void *[]){
    &(Control){ .Tag = TITLE_TAG, .Text = "Hello World Webapp" },
    NULL
}};

Control Body = (Control){ .Tag = BODY_TAG, .SubControlCount = 1, .SubControl = (void *[]){
    &(Control){ .Tag = P_TAG, .Text = "Hello World" },
    NULL
}};

Control *IndexTemplate[] = {
    Head,
    Body,
    NULL
};

// Selector field is for non-html-tag selectors ( selector_name / .selector_name )
// css lines do not need an ending semi-colon. It gets added upon constructing
CSS BodyStyle = (CSS){ .Class = "body", .Selector = 0, .Data = (char *[]){
    "background-color: #000", // Black background
    "color: #fff", // White text color
    NULL
}};

CSS *IndexStyle[] = {
    BodyStyle,
    // .. the rest of your css selector(s)
    NULL
};

// Demostrating template creation using CSS struct with the readonly
int main() {
    cWS *server = StartWebServer(NewString(""), 80, 0);
    if(!server)
    {
        printf("[ - ] Error, Unable to start web-server....!\n");
        return 1;
    }

    WebRoute *route_test = CreateRoute("index", "/", NULL);

    /* Provide the style to the html constructor */
    char *temaplte = ConstructTemplate(IndexTemplate, IndexStyle, 0, 0, 0, 0, 0, 0)
    if(!template) {
        printf("[ - ] Error, Unable to construct template....!\n");
        
        // use backup template or exit
        return 1;
    }

    int __t = SetReadOnly(route_test, template);

    /*
        AddRoute(WebRoute *r, WebRoute route);
        AddRoutes(WebRoute *r, WebRoute **routes);
        AddRoutePtr(WebRoute *r, WebRoute *route);
    */
    int route = AddRoutePtr(server, route_test);
    if(!route)
    {
        printf("[ - ] Error, Unable to add web route...!\n");
        return 1;
    }

    server->Run(server, 999, 0);
    return 0;
}