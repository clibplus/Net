/*
    Compile:
        gcc -c web.c web_config.c designer.c -lstr -larr -lmap -lpthread -g -g1
        ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a
*/
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

    HTML_TAG            = 8491,
    HEAD_TAG            = 8492,
    BODY_TAG            = 8493,

    TITLE_TAG           = 8494,
    H1_TAG              = 8495,
    INPUT_TAG           = 8496,
    P_TAG               = 8497,
    DIV_TAG             = 8498,
    A_TAG               = 8499
} ControlTag;

typedef struct Control {
    void                *Parent;
    ControlTag          Tag;        // ControlTag
    char                *Type;      // Type for <input> <button> <select> <script>
    char                *Text;      // text for tags: <p> <h1> <h2> <h3>
    char                *Data;      // For Any Other Data In the Opening Tag <div Data... > </div>
    char                *href;      // href for <a>
    char                **CSS;      // CSS Function Generator for the tag <div style="CSS FUNCTION"></div>
    void                **SubControls;
} Control;

typedef struct WebRoute {
    char                *Name;
    char                *Path;
    char                *Output;
    int                 InlineCSS;

    void                *Handler;
    void                *Generator;

    Control             **Controls;
    long                ControlCount;
    void                (*Destruct)     (struct WebRoute *r);
} WebRoute;

typedef struct WebServerConfig {
    int                 DirRouteSearch;     // Search for new route pages in a directory

    WebRoute            **Routes;
    long                RouteCount;

    WebRoute            *Index;
    char                *Err404;
    void                (*Destruct)     (struct WebServerConfig *cfg);
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

typedef Control Textbox;
typedef Control Button;
typedef Control Div;

extern void *HTML_TAGS[][2];

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
void    DestroyCfg(WebServerConfig *cfg);
void    DestroyRoute(WebRoute *r);

char *ExecuteConstructor(Control *control);
String ConstructDesign(Control **controls);
String ConstructControl(Control *control);
char *ConstructTag(Control *header);
String ConstructParent(Control *p);