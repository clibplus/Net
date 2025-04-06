#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <Net/web.h>

Control Head = (Control){ .Tag = HEAD_TAG, .SubControlCount = 1, .SubControls = (void *[]){
    &(Control){ .Tag = TITLE_TAG, .Text = "Hello World Webapp" },
    NULL
}};

Control Body = (Control){ .Tag = BODY_TAG, .SubControlCount = 1, .SubControls = (void *[]){
    &(Control){ .Tag = DIV_TAG, .ID = "custom_input_form1", .SubControlCount = 2, .SubControls = (void *[]){
        &(Control){ .Tag = INPUT_TAG, .Type = "text", .ID = "input_1", .Data = "placeholder=\"input 1\"" },
        &(Control){ .Tag = INPUT_TAG, .Type = "submit", .ID = "ws_submit_btn_1", .Data = "value=\"submit button 1\"" },
        NULL
    }},
    &(Control){ .Tag = DIV_TAG, .ID = "custom_input_form2", .SubControlCount = 2, .SubControls = (void *[]){
        &(Control){ .Tag = INPUT_TAG, .Type = "text", .ID = "input_2", .Data = "placeholder=\"input 2\"" },
        &(Control){ .Tag = INPUT_TAG, .Type = "submit", .ID = "ws_submit_btn_2", .Data = "value=\"submit button 2\"" },
        NULL
    }},
    NULL
}};

Control *Template[] = {
    &Head,
    &Body,
    NULL
};

void live_updates(cWS *server, cWR *req, WebRoute *route) {
    printf("Event Checking: %ld Found\n", req->Event.idx);

    if(req->Event.idx == 13) {
        char *input_1 = ((jKey *)req->Event.arr[11])->value;
        char *parent = ((jKey *)req->Event.arr[12])->value;
        printf("%s: %s\n", parent, input_1);

        if(!input_1) {
            SendResponse(server, req->Socket, OK, DefaultHeaders, ((Map){0}), "fetched");
            return;
        }

        // use stack_to_heap() to convert a stack template to heap for HTML constructor
        Control *input_1_form = stack_to_heap((!strcmp(((jKey *)req->Event.arr[11])->key, "input_1") ? *(Control *)Body.SubControls[0] : *(Control *)Body.SubControls[1]));
        if(!input_1_form) {
            printf("[ - ] Error, Unable to create control....!\n");
        }

        char *output = CreateString((char *[]){"Results: ", input_1, NULL});
        if(!output) {
            printf("[ - ] Error, Unable to create string...!\n");
        }

        int add_ctrl = input_1_form->Append(input_1_form, CreateControl(P_TAG, NULL, NULL, output, NULL));
        if(!add_ctrl) { // err
            DestructControls(input_1_form);
            free(output);
            SendResponse(server, req->Socket, OK, DefaultHeaders, ((Map){0}), "Error");
            return;
        }

        printf("Sending UI update....!\n");
        UpdateUI(server, req, NULL, (Control *[]){input_1_form, NULL}, NULL);
        return;
    }

    char *template = ConstructTemplate(Template, NULL, 1, 0, 0, 0, 0, 0);
    if(!template) {
        SendResponse(server, req->Socket, OK, DefaultHeaders, ((Map){0}), "Error, Unable to construct template...!\n");
        return;
    }

    SendResponse(server, req->Socket, OK, DefaultHeaders, ((Map){0}), template);
    free(template);
}

// Demostrating a live-updates using EnableLiveHandler(), custom form inputs (non-form tag) and 
// live client events using ConstructTemplate()
int main() {
    cWS *server = StartWebServer(NewString(""), 80, 0);
    if(!server)
    {
        printf("[ - ] Error, Unable to start web-server....!\n");
        return 1;
    }

    WebRoute *route_test = CreateRoute("index", "/", live_updates);
    int route = AddRoutePtr(server, route_test);
    if(!route)
    {
        printf("[ - ] Error, Unable to add web route...!\n");
        return 1;
    }

    EnableLiveHandler(server);
    server->Run(server, 999, 0);
    return 0;
}