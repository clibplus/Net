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
        .Routes     = NewMap()
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

void RunServer(HTTPServer *web, int concurrents, const char *search_path) {
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

void ParseAndCheckRoute(HTTPServer *web, int request_socket) {
    char BUFFER[4096] = {0};
    read(request_socket, buffer, 4095);

    cWR *r = ParseRequest(&buffer);
    if(!r || !r->Route) {
        SendRawResponse(s, request_socket, OK, headers, s->err_404_filepath, NULL);
        return;
    }

    Map new_headers = NewMap();
    new_headers.Append(&new_headers, "Content-Type", "text/html; charset=UTF-8");
    new_headers.Append(&new_headers, "Connection", "close");

    int chk = isRouteValid(web, r->Route.data);
    if(!strcmp(r->RequestType.data, "POST"))
        get_post_queries(web, r);

    if(chk)
        web->Routes->Keys[chk]->Value(web, r, request_socket);

    
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