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

int main() {
    cWS *server = StartWebServer(NewString(""), 80, 0);
    if(!server)
    {
        printf("[ - ] Error, Unable to start web-server....!\n");
        return 1;
    }

    WebRoute *route_test = CreateRoute("index", "/", NULL);
    int __t = SetReadOnly(route_test, HTML_PAGE);

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