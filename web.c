#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "web.h"

cWS *StartWebServer(const string ip, int port, int auto_search) {
    if(!ip.data || port <= 0)
        return NULL;

    cWS *web = (cWS *)malloc(sizeof(cWS));
    *web = (cWS){
        .IP         = NewString(ip.data),
        .Port       = port,
        .Socket     = socket(AF_INET, SOCK_STREAM, 0),
        .CFG        = (WebServerConfig){
            .DirRouteSearch = auto_search
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

    if(bind(web->Socket, (struct sockaddr *)web->Address, sizeof(web->Address)) < 0)
        return NULL;

    return web;
}

void RunServer(cWS *web, int concurrents, const char *search_path) {
    if(listen(web->Socket, concurrents) < 0)
        return;

    printf("[ cWS ] Web server is now listening on %s:%d\n", web->IP.data, web->Port);

    int request_socket = 0, addrlen = sizeof(web->Address);
    while(1) {
        if((request_socket = accept(web->Socket, (struct sockaddr *)web->Address, (socklen_t *)&addrlen)) < 0)
            continue;

        pthread_t tid;
        void **arr = (void **)malloc(sizeof(void *) * 2);
        arr[0] = web;
        arr[1] = request_socket;

        pthread_create(&tid, NULL, (void *)thread_req, (void *)arr);
    }
}

void ParseAndCheckRoute(cWS *web, int request_socket) {
    char BUFFER[4096] = {0};
    read(request_socket, buffer, 4095);

    cWR *r = ParseRequest(&buffer);
    if(!r || !r->Route) {
        SendResponse(s, request_socket, OK, headers, NULL, s->err_404_filepath);
        return;
    }

    Map new_headers = NewMap();
    new_headers.Append(&new_headers, "Content-Type", "text/html; charset=UTF-8");
    new_headers.Append(&new_headers, "Connection", "close");

    int chk = isRouteValid(web, r->Route.data);
    if(!strcmp(r->RequestType.data, "POST"))
        GetPostQueries(web, r);

    if(strstr(r->Route, "?"))
        RetrieveGetParameters(web, r);

    (chk ? web->Routes->Keys[chk]->Value(web, r, request_socket) : SendResponse(s, request_socket, OK, headers, s->err_404_filepath, NULL) );
}

cWR *ParseRequest(const char *data) {
    if(!data)
        return NULL;

    String raw_data = NewString(data);
    Array lines = NewArray(NULL);
    lines.Merge(&lines, (void **)raw_data.Split(&raw_data, "\n"));

    cWR *r = (cWR *)malloc(sizeof(cWR *));
    *r = (cWR){
        .Headers = NewMap(),
        .Body = NewString(NULL)
    };

    String HTTP_VERSION = NewString(lines.arr[0]);
    Array ver_args = NewArray(NULL);
    ver_args.Merge(&ver_args, HTTP_VERSION.Split(&HTTP_VERSION, " "));

    if(ver_args.idx < 1)
        return NULL;

    r->Route = NewString(ver_args.arr[1]);
    HTTP_VERSION.Destruct(&HTTP_VERSION);

    for(int i = 1; i < lines.idx; i++) {
        if(!strcmp(lines.arr[i], "") || !strcmp(lines.arr[i], "2be") || !strcmp(lines.arr[i], "0"))
            break;

        String line = NewString(lines.arr[i]);
        Array args = NewArray(NULL);
        args.Merge(&args, line.Split(&line, ":"));

        line.Strip(&line);
        if(!strcmp(line.data, ""))
            break;

        if(args.idx >= 2) {
            String value = NewString(lines.arr[i]);
            value.Replace(&value, args[0], "");

            r->Headers.Append(r->Headers, args[0], value.data);
            value.Destruct(&value);
        } else {
            r->Body.AppendString(r->Body, lines.arr[i]);
        }

        line.Destruct(&line);
        args.Destruct(&args);
    }

    raw_data.Destruct(&raw_data);
    lines.Destruct(&lines);
    ver_args.Destruct(&ver_args);

    return r;
}

void GetPostQueries(cWS *web, cWR *r) {
    Map Queries = NewMap();
    Array args = NewArray(NULL);

    args.Merge(&args, r->Body.Split(&r->Body, "&"));

    if(args.idx < 1)
        return ((Map){});

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
}

int RetrieveGetParameters(cWS *web, cWR *r) {
    if(!strstr(r->Route.data, "?"))
        return 0;

    Map *queries = NewMap();
    Array link_args = NewArray(NULL);
    link_args.Merge(&link_args, (void **)r->Route.Split(&r->Route, "?"));

    String parameters(args.arr[1]);
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

    r->Queries = Queries;

    link_args.Destruct(&link_args);
    args.Destruct(&args);

    return 1;
}

void SendResponse(cWS *web, int request_socket, StatusCode_T code, Map headers, Map vars, const char *body) {
    String _resp = NewString("HTTP/1.1 ");

    switch(code) {
        case OK: { 
            _resp.AppendNum(&_resp, (int)OK); 
            _resp.AppendString(&_resp, " OK");
            break; 
        };
    }

    for(int i = 0; i < headers.idx; i++)
        _resp.AppendArray(&_resp, ((const char *[]){headers.arr[i]->key, ": ", headers.arr[i]->value, "\r\n"}));

    if(body)
        _resp.AppendString(&_resp, body);
    
    _resp.AppendString(&_resp, "\r\n\r\n");
    write(request_socket, _resp.data, _resp.idx);
    _resp.Destruct(&_resp);
}

void *DestroyServer(cWS *web) {
    if(web->IP.data)
        web->IP.Destruct(&web-IP);

    if(web->Routes.arr)
        web->Routes.Destruct(&web->Routes);
    
    free(web);
}