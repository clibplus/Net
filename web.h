/*
    Compile:
        gcc -c web.c web_config.c designer.c -lstr -larr -lmap -lpthread -g -g1
        ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a

    Use:
        -lwebsign -lstr -larr -lmap -lpthread
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
#include <OS/file.h>

#include "request.h"

typedef enum ControlTag {
    NO_TAG              = 8490,

    HTML_TAG            = 8491,
    HEAD_TAG            = 8492,
    BODY_TAG            = 8493,

    TITLE_TAG           = 8494,
    H1_TAG              = 8495,
    H2_TAG              = 8496,
    H3_TAG              = 8497,
    INPUT_TAG           = 8498,
    P_TAG               = 8499,
    DIV_TAG             = 8500,
    A_TAG               = 8501,
    PRE_TAG             = 8502
} ControlTag;

typedef struct CSS {
    char                *Class;
    char                **Data;
    int                 Selector;
} CSS;

typedef struct Control {
    void                *Parent;
    ControlTag          Tag;        // ControlTag
    char                *Type;      // Type for <input> <button> <select> <script>
    char                *Text;      // text for tags: <p> <h1> <h2> <h3>
    char                *Class;     // class=""
    char                *Data;      // For Any Other Data In the Opening Tag <div Data... > </div>
    char                *href;      // href for <a>
    char                **CSS;      // CSS Function Generator for the tag <div style="CSS FUNCTION"></div>
    long                CSS_Count;
    void                **SubControls;
} Control;

typedef struct WebRoute {
    char                *Name;          // Name of route
    char                *Path;          // Route
    char                *Template;

    void                *Handler;       // Web Route Handler
    void                *Generator;     // Generator UI/UX Template

    CSS                 **CSS;          // 2D Array CSS_SELECTOR_NAME => CSS_DATA (Cache)
    long                CSS_Count;

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
    char                *Logs;

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

// == [ Web Server Operation ] ==

//
//      | - > Initalize a web server struct
//      | - > Returns struct with info upon success or empty struct upon failure
//
cWS     *StartWebServer(const String IP, int port, int auto_search);

//
//      | - > Start listening for connections
//
void    RunServer(cWS *web, int concurrents, const char *search_path);

//
//      | - > Check and route the request
//
void    ParseAndCheckRoute(void **args);

//
//      | - > Parse the incoming request
//      | - > Returns struct with info upon success or empty struct upon failure
//
cWR     *ParseRequest(const char *data);

//
//      | - > Split up data for POST parameters
//
void    GetPostQueries(cWS *web, cWR *r);

//
//      | - > split up the URL for GET parameters
//      | - > Returns 1 upon success or 0 for failure
//
int     RetrieveGetParameters(cWS *web, cWR *r);

//
//      | - > Send a response to client
//
void    SendResponse(cWS *web, int request_socket, StatusCode code, Map headers, Map vars, const char *body);

//
//      | - > Destruct Web Server
//
void    DestroyServer(cWS *web);

// == [ Route Operation ] ==

//
//      | - > Search for route
//      | - > Returns position of route in the route list upon success or -1 upon failure
//
int     SearchRoute(cWS *web, const char *data);

//
//
//
//
int     AddCSS(WebRoute *r, void *arr);

//
//      | - > Add a routes to the web server 
//      | - > Returns 1 upon success or 0 upon failure
//
int     AddRoutes(cWS *web, WebRoute **routes);

//
//      | - > Add a route to the web server 
//      | - > Returns 1 upon success or 0 upon failure
//
int     AddRoute(cWS *web, WebRoute route);

//
//      | - > Config Destructor
//
void    DestroyCfg(WebServerConfig *cfg);

//
//      | - > Route Destructor
//
void    DestroyRoute(WebRoute *r);

// == [ Websign Template Generation Operations ] ==

char *FindTag(Control *control);
int ConstructTemplate(WebRoute *route);
char *ConstructCSS(WebRoute *route);
String  ConstructParent(Control *p, int sub);