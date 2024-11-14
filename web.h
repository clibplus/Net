#pragma once

#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <str.h>
#include <arr.h>
#include <map.h>

#include "request.h"

typedef struct WebRoute {
    char    *Name;
    char    *Path;
    char    *Output;

    int    *GenOutput;
} WebRoute;

typedef struct WebServerConfig {
    int                 DirRouteSearch;     // Search for new route pages in a directory

    WebRoute            *Index;
    char                *Err404;
} WebServerConfig;

/* C Web Server */
typedef struct cWS {
    String              IP;
    int                 Port;
    int                 Socket;
    Map                 *Routes;
    struct sockaddr_in  Address;
    SSL                 *SSL;
    SSL_CTX             *CTX;
    WebServerConfig     CFG;

    void                (*Run)          (struct cWS *web, int concurrents, const char *search_path);
    void                (*Destruct)     (struct cWS *web);
} cWS;

/* C Web Reqest */
typedef struct cWR {
    String              Route;
    String              Fullroute;
    String              RequestType;
    Map                 Headers;
    Map                 Queries;
    String              Body;
} cWR;

cWS *StartWebServer(const string IP, int port, int auto_search);
void RunServer(HTTPServer *web, int concurrents, const char *search_path);
void ParseAndCheckRoute(cWS *web, int request_socket);
cWR *ParseRequest(const char *data);
void GetPostQueries(cWS *web, cWR *r);
int RetrieveGetParameters(cWS *web, cWR *r);
void SendResponse(cWS *web, int request_socket, StatusCode_T code, Map headers, Map vars, const char *body);
void *DestroyServer(cWS *web);