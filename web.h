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
*   Compile:
*       gcc -c web.c web_config.c web_route.c websign/*.c -lstr -larr -lmap -lpthread -g -g1
*       ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a
*
*   Use:
*       -lwebsign -lstr -larr -lmap -lpthread
*
*   Current Update: 1.0-2.15.24-
*   Last Update: 0.27-1.14.24.5f63294
*/
#pragma once

#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <str.h>
#include <arr.h>
#include <map.h>
#include <OS/file.h>
#include <Net/request.h>

/* Controls using HTML Tags */
typedef enum ControlTag {
    NO_TAG                              = 8490,

    HTML_TAG                            = 8491, // <html>
    HEAD_TAG                            = 8492, // <head>
    BODY_TAG                            = 8493, // <body>

    TITLE_TAG                           = 8494, // <title>
    H1_TAG                              = 8495, // <h1>
    H2_TAG                              = 8496, // <h2>
    H3_TAG                              = 8497, // <h3>
    INPUT_TAG                           = 8498, // <input>
    P_TAG                               = 8499, // <p>
    DIV_TAG                             = 8500, // <div>
    A_TAG                               = 8501, // <a>
    PRE_TAG                             = 8502, // <pre>
    BUTTON_TAG                          = 8503, // <button>
    FORM_TAG                            = 8504, // <form>
    IMG_TAG                             = 8505, // <img>
    META_TAG                            = 8506, // <meta>
    LINK_TAG                            = 8507, // <link>
    BASE_TAG                            = 8508, // <base>
    STYLE_TAG                           = 8509, // <style>
    ADDRESS_TAG                         = 8510, // <address>
    ARTICLE_TAG                         = 8511, // <article>
    ASIDE_TAG                           = 8512, // <aside>
    FOOTER_TAG                          = 8513, // <footer>
    HEADER_TAG                          = 8514, // <header>
    HGROUP_TAG                          = 8515, // <hgroup>
    MAIN_TAG                            = 8516, // <main>
    NAV_TAG                             = 8517, // <nav>
    SECTION_TAG                         = 8518, // <section>
    SEARCH_TAG                          = 8519, // <search>
    BLOCKQUOTE_TAG                      = 8520, // <blockquote>
    DD_TAG                              = 8521, // <dd>
    DL_TAG                              = 8522, // <dl>
    DT_TAG                              = 8523, // <dt>
    FIGCAPTION_TAG                      = 8524, // <figcaption>
    FIGURE_TAG                          = 8525, // <figure>
    HR_TAG                              = 8526, // <hr>
    LI_TAG                              = 8527, // <li>
    MENU_TAG                            = 8528, // <menu>
    OL_TAG                              = 8529, // <ol>
    UL_TAG                              = 8530, // <ul>
    ABBR_TAG                            = 8531, // <abbr>
    B_TAG                               = 8532, // <b>
    BDI_TAG                             = 8533, // <bdi>
    BDO_TAG                             = 8534, // <bdo>
    BR_TAG                              = 8535, // <br>
    CITE_TAG                            = 8536, // <cite>
    CODE_TAG                            = 8537, // <code>
    DATA_TAG                            = 8538, // <data>
    DFN_TAG                             = 8539, // <dfn>
    EM_TAG                              = 8540, // <em>
    I_TAG                               = 8541, // <i>
    KBD_TAG                             = 8542, // <kbd>
    MARK_TAG                            = 8543, // <mark>
    Q_TAG                               = 8544, // <q>
    RP_TAG                              = 8545, // <rp>
    RT_TAG                              = 8546, // <rt>
    RUBY_TAG                            = 8547, // <ruby>
    S_TAG                               = 8548, // <s>
    SAMP_TAG                            = 8549, // <samp>
    SMALL_TAG                           = 8550, // <small>
    SPAN_TAG                            = 8551, // <span>
    STRONG_TAG                          = 8552, // <strong>
    SUB_TAG                             = 8553, // <sub>
    SUP_TAG                             = 8554, // <sup>
    TIME_TAG                            = 8555, // <time>
    U_TAG                               = 8556, // <u>
    VAR_TAG                             = 8557, // <var>
    WBR_TAG                             = 8558, // <wbr>
    AREA_TAG                            = 8559, // <area>
    AUDIO_TAG                           = 8560, // <audio>
    MAP_TAG                             = 8561, // <map>
    TRACK_TAG                           = 8562, // <track>
    VIDEO_TAG                           = 8563, // <video>
    EMBED_TAG                           = 8564, // <embed>
    FENCEDFRAME_TAG                     = 8565, // <fencedframe>
    IFRAME_TAG                          = 8566, // <iframe>
    OBJECT_TAG                          = 8567, // <object>
    PICTURE_TAG                         = 8568, // <picture>
    SOURCE_TAG                          = 8569, // <source>
    SVG_TAG                             = 8570, // <svg>
    MATH_TAG                            = 8571, // <math>
    CANVAS_TAG                          = 8572, // <canvas>
    NOSCRIPT_TAG                        = 8573, // <noscript>
    SCRIPT_TAG                          = 8574, // <script>
    DEL_TAG                             = 8575, // <del>
    INS_TAG                             = 8576, // <ins>
    CAPTION_TAG                         = 8577, // <captio>
    COL_TAG                             = 8578, // <col>
    COLGROUP_TAG                        = 8579, // <colgroup>
    TABLE_TAG                           = 8580, // <table>
    TBODY_TAG                           = 8581, // <tbody>
    TD_TAG                              = 8582, // <td>
    TFOOT_TAG                           = 8583, // <tfoot>
    TH_TAG                              = 8584, // <th>
    THEAD_TAG                           = 8585, // <thead>
    TR_TAG                              = 8586, // <tr>
    DATALIST_TAG                        = 8587, // <datalist>
    FIELDSET_TAG                        = 8589, // <fieldset>
    LABEL_TAG                           = 8590, // <label>
    LEGEND_TAG                          = 8591, // <legend>
    METER_TAG                           = 8592, // <meter>
    OPTGROUP_TAG                        = 8593, // <optgroup>
    OPTION_TAG                          = 8594, // <option>
    OUTPUT_TAG                          = 8595, // <output>
    PROGRESS_TAG                        = 8596, // <progress>
    SELECT_TAG                          = 8597, // <select>
    TEXTAREA_TAG                        = 8598, // <textarea>
    DATAILS_TAG                         = 8599, // <datails>
    DIALOG_TAG                          = 8600, // <dialog>
    SUMMARY_TAG                         = 8601, // <summary>
    SLOT_TAG                            = 8602, // <slot>
    TEMPLATE_TAG                        = 8603, // <template>
} ControlTag;

typedef enum WJS_Value_T {
    FORM_ELEMENTS                       = 9040,     // Provide a form ID <form id=""></form>
    ELEMENT_IDS                         = 9041      // Provide an array of element IDs
} WJS_Value_T;

typedef enum WJS_Action_T {
    NO_ACTION                           = 10930,
    REDIRECT                            = 10931,
    MSG_BEFORE_REDIRECT                 = 10932,
    GET_RESULTS                         = 10933,
    SPIN_UNTIL_RESULTS                  = 10934,
    VERTICAL_BOOMERANG_UNTIL_RESULT     = 10935,
    HORIZONTAL_BOOMERANG_UNTIL_RESULT   = 10936,
} WJS_Action_T;

typedef struct CSS {
    char                *Class;
    char                **Data;
    char                DataCount;
    int                 Selector;

    char                **HoverData;
    char                **ClickData;
} CSS;

typedef struct WJS {
    WJS_Value_T         ValueType;      // OnClick Action Type
    void                **Elements;     // Grab value of other elements using IDs
    WJS_Action_T        Action;         // IF USING ANIMATION TAGS, AnimationID must be set.
    char                *AnimationID;   // Element ID to the element you want animated
    char                *ChangeID;      // Element ID to the element you want its value changed  
} WJS;

typedef struct Control {
    void                *Parent;
    ControlTag          Tag;            // ControlTag
    char                *ID;
    char                *Name;
    char                *Type;          // Type for <input> <button> <select> <script>
    char                *Text;          // text for tags: <p> <h1> <h2> <h3>
    char                *Class;         // class=""
    char                *href;          // href for <a>
    char                **CSS;          // CSS Function Generator for the tag <div style="CSS FUNCTION"></div>
    long                CSSCount;
    char                *Data;          // For Any Other Data In the Opening Tag <div Data... > </div>
    char                *Script;
    int                 InlineJS;
    void                **SubControls;
    long                SubControlCount;

    int                 (*sAppend)      (struct Control *c, struct Control new_control);
    int                 (*Append)       (struct Control *c, struct Control *new_control);
    int                 (*AppendAt)       (struct Control *c, int pos, struct Control *new_control);
    int                 (*AppendCSS)    (struct Control *c, char *new_control);
    String              (*Construct)    (struct Control *c, int sub, int oneline);
    int                 (*ToCHT)        (struct Control *c);
    int                 (*ToHTML)       (struct Control *c);
    void                (*Destruct)     (struct Control *c, int del_control, int del_styles);
} Control;

typedef struct WebRoute {
    char                *Name;          // Name of route
    char                *Path;          // Route
    char                *Template;

    void                *Handler;       // Web Route Handler and//or UI/UX Template Gen
    void                **Args;

    CSS                 **CSS;          // 2D Array CSS_SELECTOR_NAME => CSS_DATA (Cache)
    long                CSS_Count;

    long                ReadOnly;       // Use Template Only
    Control             **Controls;
    long                ControlCount;

    char                *(*ConstructCHT)(Control **controls, CSS **styles,  int click, int hover, int mouse_track, int keyboard, int oneline);
    void                (*Destruct)     (struct WebRoute *r);
} WebRoute;

typedef struct WebServerConfig {
    int                 DirRouteSearch;     // Search for new route pages in a directory

    WebRoute            **Routes;
    long                RouteCount;

    WebRoute            *Index;
    void                *Middleware;
    void                *Err404_Handler;
    int                 (*Set404Handle) (struct WebServerConfig *cfg, void *handle);
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
    void                *Last;

    int                 (*AddRoutes)    (struct cWS *web, WebRoute **routes);
    int                 (*AddRoute)     (struct cWS *web, WebRoute r);
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
    Map                 Cookies;
    String              Body;
    char                *ClientIP;
    
    clock_t             StartTime;
    clock_t             EndTime;
    double              Elapsed;
    void                (*Destruct)     (struct cWR *r);
} cWR;

typedef struct Cookie {
	char *name;
	char *value;
	char *path;
	int  expires;
	int  maxage;
	int  HTTPOnly;
} Cookie;

typedef Control Textbox;
typedef Control Button;
typedef Control Div;

extern Map DefaultHeaders;          // Default HTTP/1.1 Response Headers

extern void *HTML_TAGS[][2];        // List of all HTML Tag enums w/ a string definition
extern void *StatusCodeDef[][2];    // List of all status code enums w/ a string definition
extern void *EncodedSymbols[][2];

// == [ Web Server Operation ] ==

//
//          | - > Initalize a web server struct
//          | - > Returns struct with info upon success or empty struct upon failure
//
cWS         *StartWebServer(const String IP, int port, int auto_search);


//
//          | - > Set default browser headers for global use
//
void        SetDefaultHeaders();

//
//          | - > Start listening for connections
//
void        RunServer(cWS *web, int concurrents, const char *search_path);

//
//          | - > Check and route the request
//
void        ParseAndCheckRoute(void **args);

//
//          | - > Parse the incoming request
//          | - > Returns struct with info upon success or empty struct upon failure
//
cWR         *ParseRequest(const char *data);

//
//          | - > Parse cookies from request(s)
//          | - > Returns the amount of cookies found upon success or 0 upon failure
//
int         ParseCookies(cWR *req, String cookies);

//
//          | - > Split up data for POST parameters
//
void        GetPostQueries(cWS *web, cWR *r);

//
//          | - > split up the URL for GET parameters
//          | - > Returns 1 upon success or 0 for failure
//
int         RetrieveGetParameters(cWS *web, cWR *r);

//
//          | - > Send a response to client
//
void        SendResponse(cWS *web, int request_socket, StatusCode code, Map headers, Map cookies, const char *body);

//
//
//
//
String      web_body_var_replacement(Map vars, const char *body);

//
//
//
//
void        fetch_cf_post_data(cWS *server, cWR *req, int socket);

//
//
//
//
char        *GetSocketIP(int sock);

//
//          | - > Convert cookie structs to a Map of cookie ready for headers
//          | - > Returns a map of cookie data upon success or 0'd struct upon failure
//
Map         CreateCookies(Cookie **arr);


//
//          | - > Convert status code enum to string for output
//          | - > Returns the status code as string upon success or NULL upon failure
//
char        *statuscode_to_str(StatusCode code);

//
//          | - > Destruct Web Server
//
void        DestroyServer(cWS *web);

//
//          | - > Destruct cWR Struct
//
void        DestroyReq(cWR *req);

// == [ Web Config Operation ] ==


//
//
//
//
int         AddDynamicHandler(cWS *web);

//
//
//
//
int         SearchRoute(cWS *web, const char *data);

//
//
//
//
int         AddRoutes(cWS *web, WebRoute **routes);

//
//
//
//
int         AddCSS(WebRoute *r, void *arr);

//
//
//
//
int         AddRoute(cWS *web, WebRoute route);

//
//
//
//
int         AddRoutePtr(cWS *web, WebRoute *route);

//
//
//
//
String      control2str(Control *p);

//
//
//
//
void        DestroyCfg(WebServerConfig *cfg);

//
// == [ Websign Template Generation Operations ] ==
// == [ HTML / CSS Genetation (html_parser.c & html_utils.c) ] ==
//

//
//
//
//
char        *FindTag(Control *control);

//
//
//
//
ControlTag  FindTagType(const char *data);

//
//
//
//
char        *ConstructTemplate(Control **controls, CSS **styles, int click, int hover, int mouse_track, int keyboard, int oneline);

//
//
//
//
char        *ConstructCSS(CSS **styles, int oneline);

//
//
//
//
char        *decode_input_symbols(const char *data);

//
//
//
//
int         count_tabs(const char *data);

//
//
//
//
Control     **process_html_line(const char *data);

//
//
//
//
Control     **ParseHTMLContent(const char *data);

// == [ WebJS Genetation ] ==


//
//
//
//
String      ConstructHandler(int click, int hover, int mouse_track, int keyboard);

//
//
//
//
String      ChangeElement(int count, char **arr);

// == [ Web_Route.c ] ==

//
//
//
//
WebRoute    *CreateRoute(const char *n, const char *p, void *handler);

//
//
//
//
int         SetReadOnly(WebRoute *w, const char *data);

//
//
//
//
void        DestroyWebRoute(WebRoute *w);

// == [ Control.c ] ==

//
//
//
//
Control     *CreateControl(ControlTag tag, const char *sclass, const char *id, const char *text, Control **subcontrols);

//
//
//
//
int         Control__AppendControlAt(Control *c, int pos, Control *new_control);

//
//
//
//
int         Control__SetBuffer(Control *c, char *BUFF);

//
//
//
//
int         SetStyle(Control *c, char **style);

//
//
//
//
Control     *stack_to_heap(Control c);

//
//
//
//
int         Control__AppendStackControl(Control *c, Control new_control);

//
//
//
//
int         AppendControl(Control *c, Control *new_control);

//
//
//
//
int         AppendCSS(Control *c, char *new_css);

//
//
//
//
char        *create_index_line(int len);

//
//
//
//
String      ConstructControl(Control *c, int sub, int oneline);

//
//
//
//
void        DestructControl(Control *c, int del_control, int del_styles);

//
//
//
//
void        DestructControls(Control *c);

//
//
//
//
String      DumpControls(Control *controls, int nestingLevel);

// == [ ws_css.c ] ==

int AppendDesign(CSS *style, const char *q);