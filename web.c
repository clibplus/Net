#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "web.h"

cWS *StartWebServer(String ip, int port, int auto_search) {
    if(!ip.data || port <= 0)
        return NULL;

    cWS *web = (cWS *)malloc(sizeof(cWS));
    *web = (cWS){
        .IP         = NewString(ip.data),
        .Port       = port,
        .Socket     = socket(AF_INET, SOCK_STREAM, 0),
        .CFG        = (WebServerConfig){
            .DirRouteSearch     = auto_search,
            .Routes             = (WebRoute **)malloc(sizeof(WebRoute *) * 1),
            .Destruct           = DestroyCfg
        },

        .Run        = RunServer,
        .Destruct   = DestroyServer
    };

    if(web->Socket <= 0)
        return NULL;

    web->Address.sin_port = htons(port);
    if(ip.idx == 0) {
        web->Address.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_aton(ip.data, &web->Address.sin_addr);
    }

    int reuse = 1;
    if(setsockopt(web->Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
        return NULL;

    if(bind(web->Socket, (struct sockaddr *)&web->Address, sizeof(web->Address)) < 0)
        return NULL;

    return web;
}

void RunServer(cWS *web, int concurrents, const char *search_path) {
    if(listen(web->Socket, concurrents) < 0)
        return;

    printf("[ cWS ] Web server is now listening on %s:%d\n", web->IP.data, web->Port);

    int request_socket = 0, addrlen = sizeof(web->Address);
    while(1) {
        if((request_socket = accept(web->Socket, (struct sockaddr *)&web->Address, (socklen_t *)&addrlen)) < 0)
            continue;

        pthread_t tid;
        void **arr = (void **)malloc(sizeof(void *) * 2);
        arr[0] = (void *)web;
        arr[1] = (void *)&request_socket;

        pthread_create(&tid, NULL, (void *)ParseAndCheckRoute, (void *)arr);
    }
}

void ParseAndCheckRoute(void **args) {
    cWS *web = (cWS *)args[0];
    int request_socket = *(int *)args[1];

    char *BUFFER = (char *)calloc(4096, sizeof(char));
    int bytes = read(request_socket, BUFFER, 4096);
    BUFFER[bytes + 1] = '\0';
    
    Map new_headers = NewMap();
    new_headers.Append(&new_headers, "Content-Type", "text/html; charset=UTF-8");
    new_headers.Append(&new_headers, "Connection", "close");

    cWR *r = ParseRequest(BUFFER);
    if(!r || !r->Route.data) {
        SendResponse(web, request_socket, OK, new_headers, ((Map){}), web->CFG.Err404);
        close(request_socket);
        return;
    }

    printf("[ NEW REQUEST ATTEMPT ] %s\n", r->Route.data);
    int chk = SearchRoute(web, r->Route.data);
    if(chk == -1) {
        free(BUFFER);
        close(request_socket);
        return;
    }

    if(!strcmp(r->RequestType.data, "POST"))
        GetPostQueries(web, r);

    if(strstr(r->Route.data, "?"))
        RetrieveGetParameters(web, r);

    printf("[ NEW REQUEST ] %s = > %s\n", web->CFG.Routes[chk]->Name, web->CFG.Routes[chk]->Path);
    (void)(chk > -1 ? ((void (*)(cWS *, cWR *, WebRoute *, int))((WebRoute *)web->CFG.Routes[chk])->Handler)(web, r, web->CFG.Routes[chk], request_socket) : SendResponse(web, request_socket, OK, new_headers, ((Map){}), NULL));

    free(BUFFER);
    close(request_socket);
    pthread_exit(NULL);
}

cWR *ParseRequest(const char *data) {
    if(!data)
        return NULL;

    cWR *r = (cWR *)malloc(sizeof(cWR));
    *r = (cWR){
        .Headers = NewMap(),
        .Body = NewString(NULL),
    };

    String traffic = NewString(data);
    Array lines = NewArray(NULL);
    lines.Merge(&lines, (void **)traffic.Split(&traffic, "\n"));

    if(lines.idx < 1) {
        traffic.Destruct(&traffic);
        lines.Destruct(&lines);
        return NULL;
    }

    String request_type = NewString(lines.arr[0]);
    Array argz = NewArray(NULL);
    argz.Merge(&argz, (void **)request_type.Split(&request_type, " "));

    r->RequestType = NewString(argz.arr[0]);
    r->Route = NewString(argz.arr[1]);

    argz.Destruct(&argz);
    request_type.Destruct(&request_type);

    for(int i = 0; i < lines.idx; i++) {
        if(!lines.arr[i])
            break;

        String line = NewString(lines.arr[i]);
        if(line.isEmpty(&line) || line.Is(&line, " "))
            break;

        if(line.Contains(&line, ":")) {
            Array args = NewArray(NULL);
            args.Merge(&args, (void **)line.Split(&line, ": "));

            r->Headers.Append(&r->Headers, args.arr[0], args.arr[1]);
            args.Destruct(&args);
        } else {
            r->Body.AppendString(&r->Body, line.data);
        }

        line.Destruct(&line);
    }

    traffic.Destruct(&traffic);
    lines.Destruct(&lines);

    return r;
}

void GetPostQueries(cWS *web, cWR *r) {
    Map Queries = NewMap();
    Array args = NewArray(NULL);

    args.Merge(&args, (void **)r->Body.Split(&r->Body, "&"));
    if(args.idx < 1)
        return;

    for(int i = 0; i < args.idx; i++) {
        String query = NewString(args.arr[i]);
        Array query_args = NewArray(NULL);
        query_args.Merge(&query_args, (void **)query.Split(&query, "="));

        if(query_args.idx > 1)
            Queries.Append(&Queries, query_args.arr[0], query_args.arr[1]);

        query.Destruct(&query);
        query_args.Destruct(&query_args);
    }
    
    r->Queries = Queries;
    args.Destruct(&args);
}

int RetrieveGetParameters(cWS *web, cWR *r) {
    if(!strstr(r->Route.data, "?"))
        return 0;

    Map queries = NewMap();
    Array link_args = NewArray(NULL);
    link_args.Merge(&link_args, (void **)r->Route.Split(&r->Route, "?"));

    String parameters = NewString(link_args.arr[1]);
    Array args = NewArray(NULL);
    args.Merge(&args, (void **)parameters.Split(&parameters, "&"));

    if(args.idx < 1)
        return 0;

    for(int i = 0; i < args.idx; i++) {
        String para = NewString(args.arr[i]);
        Array para_args = NewArray(NULL);
        para_args.Merge(&para_args, (void **)para.Split(&para, "="));

        queries.Append(&queries, para_args.arr[0], para_args.arr[1]);

        para.Destruct(&para);
        para_args.Destruct(&para_args);
    }

    r->Queries = queries;

    link_args.Destruct(&link_args);
    args.Destruct(&args);

    return 1;
}

void SendResponse(cWS *web, int request_socket, StatusCode code, Map headers, Map vars, const char *body) {
    String resp = NewString("HTTP/1.1 200 OK\r\n");

    if(headers.idx > 0)
        for(int i = 0; i < headers.idx; i++)
            resp.AppendArray(&resp, ((const char *[]){(char *)((Key *)headers.arr[i])->key, ": ", (char *)((Key *)headers.arr[i])->value, "\r\n", NULL}));

    resp.AppendArray(&resp, ((const char *[]){"\r\n", body, "0\r\n", NULL}));
    write(request_socket, resp.data, resp.idx - 1);

    resp.Destruct(&resp);
}

void DestroyServer(cWS *web) {
    if(web->IP.data)
        web->IP.Destruct(&web->IP);

    if(web->CFG.Routes)
        web->CFG.Destruct(&web->CFG);
        
    free(web);
}