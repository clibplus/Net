/*
*
*                   ╦ ╦╔═╗╔╗ ╔═╗╦╔═╗╔╗╔
*                   ║║║║╣ ╠╩╗╚═╗║║ ╦║║║
*                   ╚╩╝╚═╝╚═╝╚═╝╩╚═╝╝╚╝
*      [ Websign's Browser SDK (The Magic Browser Lib) ]
*
* - Serving a raw simplist web-server to work with in C.
*
* - Serving as a browser SDK to generate template supporting
*   everything a browser takes
*
* - We all know a web-browser is a web-client that automate certain takes for web 
*   developers to design and create great applications, but as we know in these modern 
*   years every server has a client managed by the creators of the servers, with that
*   being said. Knowing web-server was originally created 
*
* - Install Commands
*
* gcc -c web.c web_config.c web_route.c websign/*.c -lstr -larr -lmap -lpthread -g -g1
* ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "web.h"

Map DefaultHeaders = {0};
#define STATUS_CODE_COUNT 63

void *StatusCodeDef[][2] = {
    {(void *)CONTINUEE,                         "Continue" },
    {(void *)SWITCH_PROTOCOL,                   "Switching Protocols" },
    {(void *)PROCESSING,                        "Processing" },
    {(void *)EARLY_HINT,                        "Early Hints" },
    {(void *)OK,                                "OK" },
    {(void *)CREATED,                           "Created" },
    {(void *)ACCEPTED,                          "Accepted" },
    {(void *)NON_AUTHORIZED_INFO,               "Non-Authoritative Information" },
    {(void *)NO_CONTENT,                        "No Content" },
    {(void *)RESET_CONTENT,                     "Reset Content" },
    {(void *)PARTIAL_CONTENT,                   "Partial Content" },
    {(void *)MULTI_STATUS,                      "Multi-Status" },
    {(void *)ALREADY_REPRORTED,                 "Already Reported" },
    {(void *)IM_USED,                           "IM Used" },
    {(void *)MULTIPLE_CHOICES,                  "Multiple Choices" },
    {(void *)MOVED_PERMANENTLY,                 "Moved Permanently" },
    {(void *)FOUND,                             "Found" },
    {(void *)SEE_OTHER,                         "See Other" },
    {(void *)NOT_MODIFIED,                      "Not Modified" },
    {(void *)USE_PROXY,                         "Use Proxy" },
    {(void *)SWITCH_PROXY,                      "Switch Proxy" },
    {(void *)TEMP_REDIRECT,                     "Temporary Redirect" },
    {(void *)PERM_REDIRECT,                     "Permanent Redirect" },
    {(void *)BAD_REQUEST,                       "Bad Request" },
    {(void *)UNAUTHORIZED,                      "Unauthorized" },
    {(void *)PAYMENT_REQUIRED,                  "Payment Required" },
    {(void *)FORBIDDEN,                         "Forbidden" },
    {(void *)NOT_FOUND,                         "Not Found" },
    {(void *)METHOD_NOT_ALLOWED,                "Method Not Allowed" },
    {(void *)NOT_ACCEPTABLE,                    "Not Acceptable" },
    {(void *)PROXY_AUTH_REQUIRED,               "Proxy Authentication Required" },
    {(void *)REQUIRE_TIMEOUT,                   "Request Timeout" },
    {(void *)CONFLICT,                          "Conflict" },
    {(void *)GONE,                              "Gone" },
    {(void *)LENGTH_REQUIRED,                   "Length Required" },
    {(void *)PRECONDITION_FAILED,               "Precondition Failed" },
    {(void *)PAYLOAD_TOO_LARGE,                 "Content Too Large" },
    {(void *)URI_TOO_LONG,                      "URI Too Long" },
    {(void *)UNSUPPORTED_MEDIA_TYPE,            "Unsupported Media Type" },
    {(void *)RANGE_NOT_SATISFIABLE,             "Range Not Satisfiable" },
    {(void *)EXPECTATION_FAILED,                "Expectation Failed" },
    {(void *)IM_A_TEAPOT,                       "I'm a teapot" },
    {(void *)MISDIRECTED_REQUEST,               "Misdirected Request" },
    {(void *)UNPROCESSABLE_ENTITY,              "Unprocessable Content" },
    {(void *)LOCKED,                            "Locked" },
    {(void *)FAILED_DEPENDENCY,                 "Failed Dependency" },
    {(void *)TOO_EARLY,                         "Too Early" },
    {(void *)UPGRADE_REQUIRED,                  "Upgrade Required" },
    {(void *)PROCONDITION_REQUIRED,             "Precondition Required" },
    {(void *)TOO_MANY_REQUEST,                  "Too Many Requests" },
    {(void *)REQ_HEADER_FIELD_TOO_LARGE,        "Request Header Fields Too Large" },
    {(void *)UNAVAILABLE_FOR_LEGAL_REASON,      "Unavailable For Legal Reasons" },

    {(void *)INTERNAL_SERVER_ERR,               "Internal Server Error" },
    {(void *)NOT_IMPLEMENTED,                   "Not Implemented" },
    {(void *)BAD_GATEWAY,                       "Bad Gateway" },
    {(void *)SERVER_UNAVAILABLE,                "Service Unavailable" },
    {(void *)GATEWAY_TIMEOUT,                   "Gateway Timeout" },
    {(void *)HTTP_VERSION_NOT_SUPPORTED,        "HTTP Version Not Supported" },
    {(void *)VARIANT_ALSO_NEGOTIATES,           "Variant Also Negotiates" },
    {(void *)INSUFFICIENT_STORAGE,              "Insufficient Storage" },
    {(void *)LOOP_DETECTED,                     "Loop Detected" },
    {(void *)NOT_EXTENDED,                      "Not Extended" },
    {(void *)NETWORK_AUTHENTICATION_REQUIRED,   "Network Authentication Required" },
    NULL
};

cWS *StartWebServer(String ip, int port, int auto_search) {
    SetDefaultHeaders();
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
        .AddRoute   = AddRoute,
        .AddRoutes  = AddRoutes,
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

void SetDefaultHeaders() {
    DefaultHeaders = NewMap();
	DefaultHeaders.Append(&DefaultHeaders, "Content-Type", "text/html;charset=UTF-8");
	DefaultHeaders.Append(&DefaultHeaders, "Connection", "close");
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
    free(args);
    char *client_ip = GetSocketIP(request_socket);

    char *BUFFER = (char *)calloc(4096, sizeof(char));
    int bytes = read(request_socket, BUFFER, 4096);
    BUFFER[strlen(BUFFER) - 1] = '\0';

    cWR *r = ParseRequest(BUFFER);
    free(BUFFER);
    if(!r || !r->Route.data) {
        SendResponse(web, request_socket, OK, DefaultHeaders, ((Map){0}), ((Map){0}), "Error");
        close(request_socket);
        pthread_exit(NULL);
        return;
    }

    r->ClientIP = client_ip;
    printf("[ NEW REQUEST ATTEMPT ] %s\n", r->Route.data);
    int chk = SearchRoute(web, r->Route.data);
    if(chk == -1) {
        (void)(chk > -1 ? ((void (*)(cWS *, cWR *, WebRoute *, int))((void *)web->CFG.Err404_Handler))(web, r, web->CFG.Routes[chk], request_socket) : SendResponse(web, request_socket, OK, DefaultHeaders, ((Map){0}), ((Map){0}), "ERROR\n\n\n"));
        close(request_socket);
        r->Destruct(r);
        pthread_exit(NULL);
        return;
    }

    if(!strcmp(r->RequestType.data, "POST"))
        GetPostQueries(web, r);

    if(strstr(r->Route.data, "?"))
        RetrieveGetParameters(web, r);

    if(web->CFG.Middleware != NULL) {
        int check = (int)((int (*)(cWS *, cWR *, int))(void *)web->CFG.Middleware)(web, r, request_socket);
        if(!check) {
            close(request_socket);
            r->Destruct(r);
            pthread_exit(NULL);
            return;
        }
    }

    if(web->CFG.Routes[chk]->ReadOnly == 1) {
        SendResponse(web, request_socket, OK, DefaultHeaders, ((Map){0}), ((Map){0}), web->CFG.Routes[chk]->Template);
        sleep(1);
        close(request_socket);
        r->Destruct(r);
        pthread_exit(NULL);
        return;
    }

    (void)(chk > -1 ? ((void (*)(cWS *, cWR *, WebRoute *, int))((WebRoute *)web->CFG.Routes[chk])->Handler)(web, r, web->CFG.Routes[chk], request_socket) : SendResponse(web, request_socket, OK, DefaultHeaders, ((Map){0}), ((Map){0}), "ERROR\n\n\n"));

    close(request_socket);
    r->Destruct(r);
    pthread_exit(NULL);
}

cWR *ParseRequest(const char *data) {
    if(!data)
        return NULL;

    cWR *r = (cWR *)malloc(sizeof(cWR));
    *r = (cWR){
        .Headers = NewMap(),
        .Cookies = NewMap(),
        .Body = NewString(NULL),
        .Destruct = DestroyReq
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

    int READ_BODY = 0;
    for(int i = 1; i < lines.idx; i++) {
        if(!lines.arr[i])
            break;

        String line = NewString(lines.arr[i]);
        if(line.isEmpty(&line) || line.Is(&line, " ")) {
            if(READ_BODY) break;
            READ_BODY = 1;
        }

        if(line.Contains(&line, ":") && !READ_BODY) {
            Array args = NewArray(NULL);
            args.Merge(&args, (void **)line.Split(&line, ": "));

            int len = strlen(args.arr[0]) + 2;
            for(int i = 0; i < len; i++)
                line.TrimAt(&line, 0);

            if(!strcmp((char *)args.arr[0], "Cookie"))
                ParseCookies(r, line);

            r->Headers.Append(&r->Headers, args.arr[0], line.data);
            args.Destruct(&args);
        } else {
            r->Body.AppendString(&r->Body, line.data);
        }

        line.Destruct(&line);
    }

    r->Body.data[r->Body.idx] = '\0';

    traffic.Destruct(&traffic);
    lines.Destruct(&lines);

    return r;
}

int ParseCookies(cWR *req, String cookies) {
    if(!req || !cookies.data)
        return 0;

    Array cookz = NewArray(NULL);
    cookz.Merge(&cookz, (void **)cookies.Split(&cookies, ";"));

    for(int i = 0; i < cookz.idx; i++) {
        if(!cookz.arr[i])
            break;

        String cookie = NewString(cookz.arr[i]);
            
        Array args = NewArray(NULL);
        args.Merge(&args, (void **)cookie.Split(&cookie, "="));

        if(args.idx != 2)
            break;

        req->Cookies.Append(&req->Cookies, args.arr[0], args.arr[1]);

        cookie.Destruct(&cookie);
        args.Destruct(&args);
    }

    cookz.Destruct(&cookz);
    return req->Cookies.idx;
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

void SendResponse(cWS *web, int request_socket, StatusCode code, Map headers, Map cookies, Map vars, const char *body) {
    String resp = NewString("HTTP/1.1 ");
    resp.AppendNum(&resp, (int)code);
    resp.AppendArray(&resp, (const char *[]){" ", statuscode_to_str(code), "\r\n", NULL});

    if(headers.idx > 0)
        for(int i = 0; i < headers.idx; i++)
            resp.AppendArray(&resp, ((const char *[]){(char *)((Key *)headers.arr[i])->key, ": ", (char *)((Key *)headers.arr[i])->value, "\r\n", NULL}));

    if(cookies.idx > 0)
        for(int i = 0; i < cookies.idx; i++)
            resp.AppendArray(&resp, ((const char *[]){(char *)((Key *)cookies.arr[i])->key, ": ", (char *)((Key *)cookies.arr[i])->value, "\r\n", NULL}));

    if(body != NULL) {
        String body_output = NewString(body);
        if(vars.idx > 0)
            for(int i = 0; i < vars.idx; i++)
                body_output.Replace(&body_output, ((Key *)vars.arr[i])->key, ((Key *)vars.arr[i])->value);

        if(body != NULL & vars.idx > 0) {
            body_output.data[body_output.idx] = '\0';
        }
        
        resp.AppendArray(&resp, ((const char *[]){"\r\n", body_output.data, NULL}));
        body_output.Destruct(&body_output);
    }
    
    resp.AppendString(&resp, "\r\n\r\n");
    resp.data[resp.idx] = '\0';

    write(request_socket, resp.data, resp.idx - 1);
    
    resp.Destruct(&resp);
}

char *GetSocketIP(int sock) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (getpeername(sock, (struct sockaddr *)&addr, &addr_len) == -1)
        return NULL;

    char client_ip[INET_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET, &addr.sin_addr, client_ip, sizeof(client_ip)) == NULL)
        return NULL;

    return strdup(client_ip);
}

char *GetRfcTime(int seconds) {
	char current_time[100] = {0};
	time_t now = time(NULL);
	now += seconds;

	struct tm *gmt = gmtime(&now);
	strftime(current_time, sizeof(current_time), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	current_time[99] = '\0';

	return strdup(current_time);
}

Map CreateCookies(Cookie **arr) {
	Map cookies = NewMap();

	for(int i = 0; arr[i] != NULL; i++)
	{
		String value = NewString(arr[i]->name);
		value.AppendArray(&value, (const char *[]){"=", arr[i]->value, NULL});

		if(arr[i]->path)
			value.AppendArray(&value, (const char *[]){";Path=", arr[i]->path, NULL});

		if(arr[i]->expires)
			value.AppendArray(&value, (const char *[]){";Expires=", GetRfcTime(60 * arr[i]->expires), NULL});

		if(arr[i]->maxage) {
			value.AppendString(&value, ";MaxAge=");
			value.AppendNum(&value, arr[i]->maxage);
		}

		if(arr[i]->HTTPOnly)
			value.AppendString(&value, ";HTTPOnly; Secure");

		value.AppendString(&value, "\r\n");
		cookies.Append(&cookies, "Set-Cookie", value.data);
		value.Destruct(&value);
	}

	return cookies;
}

char *statuscode_to_str(StatusCode code) {
    for(int i = 0; i < STATUS_CODE_COUNT; i++)
        if(code == (StatusCode)StatusCodeDef[i][0])
            return StatusCodeDef[i][1];

    return NULL;
}

void DestroyReq(cWR *req) {
    if(req->ClientIP != NULL)
        free(req->ClientIP);

    if(req->Route.data != NULL)
        req->Route.Destruct(&req->Route);

    if(req->Fullroute.data != NULL)
        req->Fullroute.Destruct(&req->Fullroute);

    if(req->RequestType.data != NULL)
        req->RequestType.Destruct(&req->RequestType);

    if(req->Headers.arr != NULL)
        req->Headers.Destruct(&req->Headers);

    if(req->Queries.arr != NULL)
        req->Queries.Destruct(&req->Queries);

    if(req->Body.data != NULL)
        req->Body.Destruct(&req->Body);

    free(req);
}

void DestroyServer(cWS *web) {
    if(web->IP.data)
        web->IP.Destruct(&web->IP);

    if(web->CFG.Routes)
        web->CFG.Destruct(&web->CFG);
        
    free(web);
}