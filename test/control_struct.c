#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <Net/web.h>

// Keep in mind, all controls can be UNVERSIAL and optionally done dynamically 
// allowing for more flexibility and customization during run-time!
Control Head = (Control){ .Tag = HEAD_TAG, .SubControlCount = 1, SubControl = (void *[]){
    &(Control){ .Tag = TITLE_TAG, .Text = "Hello World Webapp" },
    NULL
}};

Control Body = (Control){ .Tag = BODY_TAG, .SubControlCount = 1, .SubControl = (void *[]){
    &(Control){ .Tag = P_TAG, .Text = "Hello World" },
    NULL
}};

Control *IndexTemplate[] = {
    &Head,
    &Body,
    NULL
};

// Demostrating template creation using Control struct with the readonly and ConstructTemplate()
// char *ConstructTemplate(Control **controls, 
//     CSS **styles, 
//     int click, 
//     int hover, 
//     int mouse_track, 
//     int keydown, 
//     int keyup, 
//     int oneline
// );

int main() {
    cWS *server = StartWebServer(NewString(""), 80, 0);
    if(!server)
    {
        printf("[ - ] Error, Unable to start web-server....!\n");
        return 1;
    }

    WebRoute *route_test = CreateRoute("index", "/", NULL);

    /* Provide the html constructor your template */
    char *temaplte = ConstructTemplate(IndexTemplate, NULL, 0, 0, 0, 0, 0, 0)
    if(!template) {
        printf("[ - ] Error, Unable to construct template....!\n");
        
        // use backup template or exit
        return 1;
    }

    int __t = SetReadOnly(route_test, template);
    int route = AddRoutePtr(server, route_test);
    if(!route)
    {
        printf("[ - ] Error, Unable to add web route...!\n");
        return 1;
    }

    server->Run(server, 999, 0);
    return 0;
}