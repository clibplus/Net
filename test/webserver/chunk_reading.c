#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <Net/web.h>

char BUFFER[1024];

void __getreq(cWS *server, cWR *req, WebRoute *route) {
    int bytes = 0;
    if(req->RequestType.Is(&req->RequestType, "POST")) {
        char *filename = req->Queries.InMap(&req->Queries, "filename");
        char *filesize = req->Queries.InMap(&req->Queries, "filesize");
        char *file_content = req->Queries.InMap(&req->Queries, "content");
        while((bytes = read(req->Socket, BUFFER, sizeof(BUFFER)))) {
            printf("%s\n", BUFFER);
            memset(BUFFER, '\0', sizeof(1024));

            // do ur check to ensure u got all file content
        }
    }
}

// Demostrating a route handler with GET and POST retrieving example
int main() {
    cWS *server = StartWebServer(NewString(""), 80, 0);
    if(!server)
    {
        printf("[ - ] Error, Unable to start web-server....!\n");
        return 1;
    }

    WebRoute *route_test = CreateRoute("index", "/", __getreq);
    int route = AddRoutePtr(server, route_test);
    if(!route)
    {
        printf("[ - ] Error, Unable to add web route...!\n");
        return 1;
    }

    server->Run(server, 999, 0);
    return 0;
}