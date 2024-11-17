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

typedef enum ControlTag {
    NO_TAG              = 8490,
    TITLE_TAG           = 8491,
    H1_TAG              = 8492,
    INPUT_TAG           = 8493
} ControlTag;

typedef struct Control {
    ControlTag          Tag;        // ControlTag
    char                *Text;      // For tags like <p> <h1> <h2> <h3>
    char                *Type;      // For <input type="submit"> Tag
    char                **CSS;
} Control;

typedef struct WebRoute {
    char                *Name;
    char                *Path;
    char                *Output;
    void                *Handler;

    int                 GenOutput;
    Control             **Controls;
    long                ControlCount;
} WebRoute;

typedef struct WebServerConfig {
    int                 DirRouteSearch;     // Search for new route pages in a directory

    WebRoute            **Routes;
    long                RouteCount;

    WebRoute            *Index;
    char                *Err404;
} WebServerConfig;

/* C Web Server */
typedef struct cWS {
    String              IP;
    int                 Port;
    int                 Socket;
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
    
    void                (*Destruct)     (struct cWS *r);
} cWR;

cWS     *StartWebServer(const String IP, int port, int auto_search);
void    RunServer(cWS *web, int concurrents, const char *search_path);
void    ParseAndCheckRoute(void **args);
cWR     *ParseRequest(const char *data);
void    GetPostQueries(cWS *web, cWR *r);
int     RetrieveGetParameters(cWS *web, cWR *r);
void    SendResponse(cWS *web, int request_socket, StatusCode code, Map headers, Map vars, const char *body);
void    DestroyServer(cWS *web);

int     SearchRoute(cWS *web, const char *data);
int     AddRoute(cWS *web, WebRoute route);

void DestroyCfg(WebServerConfig *cfg);
void DestroyRoute(WebRoute *r);