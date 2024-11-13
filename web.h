#pragma once

#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <str.h>
#include <arr.h>

typedef struct WebServerConfig {
    int                 DirRouteSearch;     // Search for new route pages in a directory

    char                *Index;
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

    void                (*Run) (struct cWS *web, int concurrents, const char *search_path);
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
cWR *ParseRequest(const char *data);