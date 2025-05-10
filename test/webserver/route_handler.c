#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <Net/web.h>

void __getreq(cWS *server, cWR *req, WebRoute *route) {
    if(req->Get.idx > 0) {
        int key = req->Get.InMap(&req->Get, "para_name"); // key posistion in map
        if(key == -1) {
            printf("[ - ] Error, Unable to GET parameter....!\n");
            SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "Error, Fill out all parameters to continue");
            return
        }

        // there are GET parameters avaliable
        printf("%s => %s\n", ((jKey *)req->Get.arr[key])->key, ((jKey *)req->Get.arr[key])->value);

        // return if u dnt want to handle both in one request
    }

    if(req->Queries.idx > 0) {
        int key = req->Queries.InMap(&req->Queries, "para_name"); // key posistion in map
        if(key == -1) {
            printf("[ - ] Error, Unable to POST parameter....!\n");
            SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "Error, Fill out all parameters to continue");
            return
        }

        // there are POST parameters avaliable
        printf("%s => %s\n", ((jKey *)req->Get.arr[key])->key, ((jKey *)req->Get.arr[key])->value);
    }

    SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "welcome to /api_endpoint, request with GET or POST parameters to get started....!");
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