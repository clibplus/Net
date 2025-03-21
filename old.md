<div align="center">
    <h1>cLib's Net Lib</h1>
    <p>A HTTP/S Library</p>
</div>

# Documentation

A HTTP/S library that include Websign, which is a HTML/CSS/JS toolchain framework written in C for production use!

This library is the C version of React.JS, or at-least the starting point.

## Install From Source

You can install websign directly on your system using 'git' command and 'make' to install dependencies

```
sudo apt install make -y
git clone https://github.com/clibplus/Net.git
cd Net; make
```

## Getting Started with a Hello World web page

A simple Hello World web page application can be just as easy as using a few functions, similar to high-level language NET modules/libs

```
#include <Net/web.h>

int main() {
    cWS *server = StartWebServer(NewString(""), 80, 0);     // Start web server
    if(!server)
        return 1;

    server->AddRoute(server, (WebRoute){                    // Add a route using WebRoute struct
        .Name = "index",
        .Path = "/",
        .Template = "Hello World"
    });

    server->Run(server, 999, 0);                            // Threaded

    getch();                                                // Blocking state
    return 0;
}
```

## Creating a Route Handler

Create your handler function with 4 parameters for webserver cWS, incoming request cWR, WebRoute Route, and Socket's FD int then link it to the WebRoute's path struct using the route.Handler field

```
// You can handle incoming request to the specified path with this function
void index_handler(cWS *server, cWR *req, WebRoute *route, int socket) {     
    SendResponse(web, socket, OK, DefaultHeaders, ((Map){}), "Hello World!");
}

// Edit's your WebRoute struct to use a handler
(WebRoute){
    .Name = "index",
    .Path = "/",
    .Handler = index_handler
};
```

## Create a template generator for specific paths

You can easily use Control and CSS structs to declare what you want for the template, from HTML tags to in-tag attributes and CSS!

```
/* Controls using HTML Tags */
typedef enum ControlTag {
    NO_TAG                              = 8490,

    HTML_TAG                            = 8491,
    HEAD_TAG                            = 8492,
    BODY_TAG                            = 8493,

    TITLE_TAG                           = 8494,
    H1_TAG                              = 8495,
    H2_TAG                              = 8496,
    H3_TAG                              = 8497,
    INPUT_TAG                           = 8498,
    P_TAG                               = 8499,
    DIV_TAG                             = 8500,
    A_TAG                               = 8501,
    PRE_TAG                             = 8502,
    BUTTON_TAG                          = 8503,
    FORM_TAG                            = 8504
} ControlTag;

typedef struct CSS {
    char                *Class;     // <name> {
    char                **Data;     // (arr) }
    int                 Selector;   // add a period (.) before the <name>
} CSS;

typedef struct Control {
    void                *Parent;        // Do not use
    ControlTag          Tag;            // ControlTag
    char                *ID;
    char                *Type;          // Type for <input> <button> <select> <script>
    char                *Text;          // text for tags: <p> <h1> <h2> <h3>
    char                *Class;         // class=""
    char                *href;          // href for <a>
    char                **CSS;          // CSS Function Generator for the tag <div style="CSS FUNCTION"></div>
    long                CSSCount;
    char                *Data;          // For Any Other Data In the Opening Tag <div Data... > </div>
    long                OnClick;        // Enable this to 1 and Use FormID and DisplayID
    char                *OnClickJS;     // use direct js (onclick="")
    char                *FormID;        // TESTING FIELD
    char                *DisplayID;     // TESTING FIELD
    void                **SubControls;  // Add more controls
    long                SubControlCount;

    int                 (*ToCHT)        (struct Control *c);
    int                 (*ToHTML)       (struct Control *c);
    void                (*Destruct)     (struct Control *c);
} Control;
```


(This enables special customization for whatever you want per site, user, rank/role, and//or any special site operation, unlike loading a specific template, Even randomize templates!)

```
void index_handler(cWS *server, cWR *req, WebRoute *route, int socket) { 
        Control *Controls[] = {
        &(Control){ .Tag = HEAD_TAG, .SubControls = (void *[]){
            &(Control){ .Tag = TITLE_TAG, .Text = "Hello World Page" },
            NULL
        }},
        NULL
    };

    CSS *Styles[] = {
        &(CSS){ .Class = "body", .Data = (char *[]){ // use '.Selector = 1' to add '.' before the class name in CSS
            "background-color: #000",
            "color: #fff",
            NULL
        }},
        NULL
    };

    char *template = ConstructTemplate(server, Controls, Styles);
    if(!template)
        printf("[ - ] Error, Unable to construct template....!\n");
    
    SendResponse(web, socket, OK, DefaultHeaders, ((Map){}), template);
}
```

## Using a 3rd Party Template

Yes, You read that right, Being able to use 3rd PARTY template is something that hasnt been done YET and we're glad to announce being able to use templates that arent locally installed wheather the template is empty and ready to use template or using a template being used!

Yep, you are reading correctly! Websign can parse an entire site for its elements and styles document(s) and you can ship the template without saving/installing it, strip the element values and modify for shipment!

[PSA//Note]: THIS FEATURE IS NOT COMPLETELY FINISH BUT HAS BEEN TESTED and NEEDS SOME WORK DONE! The code below is a TEST version of it last working. 

```
#include <Net/web.h>

int main() {
	File html_file = Openfile("t.txt", FILE_READ);
	html_file.Read(&html_file);

	Control **Controls = ParseHTMLContent(html_file.data);
	html_file.Destruct(&html_file);

	int i = 0;
	while(Controls[i] != NULL) {
		char *Tag = FindTag(Controls[i]);
		if(!Tag)
			printf("ERROR\n");

		printf("Tag: %s\n", Tag);


		free(Controls[i]->ID);
		free(Controls[i]->Type);
		free(Controls[i]->Text);
		free(Controls[i]->Class);
		free(Controls[i]->href);
		free(Controls[i]->Data);
		free(Controls[i]->OnClickJS);
		free(Controls[i]);
		i++;
	}

	return 0;
}
```

## Plans

- Currently looking for experimental and recreational developer to test and advance whats already implemented

- Looking for an efficient Javascript developer for server-sided dynamic event handling.

- Looking to advance event handling on both back-end code and Javascript's front-end. A general unversial render and post handler. Here's a snippet of the syntax im aimming for

```
// handle element with an ID
if(isButtonClick(req, YOUR_CONTROL_STRUCT.ID)) {
    printf("Button has been clicked...!\n");
}
```

- Developers to bind and wrap the library for other languages


### Notes

Even though Websign is in development stage. Current symbols aren't to change in the future unless there a major update!