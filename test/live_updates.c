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
        &(Control){ .Tag = INPUT_TAG, .Type = "submit", .ID = "submit_btn_1", .Data = "value=\"submit button 1\"" },
        NULL
    }},
    &(Control){ .Tag = DIV_TAG, .ID = "custom_input_form2", .SubControlCount = 2, .SubControls = (void *[]){
        &(Control){ .Tag = INPUT_TAG, .Type = "text", .ID = "input_2", .Data = "placeholder=\"input 2\"" },
        &(Control){ .Tag = INPUT_TAG, .Type = "submit", .ID = "submit_btn_2", .Data = "value=\"submit button 2\"" },
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
    if(req->Queries.idx > 0) {
        int input_1 = req->Queries.InMap(&req->Queries, "input_1");
        if(input_1 == -1) {
            SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "Error, Unable to get input...!");
            return;
        }

        // use stack_to_heap() to convert a stack template to heap for HTML constructor
        Control *input_1_form = stack_to_heap(*(Control *)Body.SubControls[0]);
        char *output = CreateString((char *[]){"Results: ", ((jKey *)req->Queries.arr[input_1])->value, NULL});

        int add_ctrl = input_1_form.Append(&input_1_form, CreateControl(P_TAG, NULL, NULL, output, NULL));
        if(!add_ctrl) { // err
            DestructControls(input_1_form);
            free(output);
            SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "Error");
            return;
        }

        char *template = ConstructTemplate(input_1_form, NULL, 1, 0, 0, 0, 0, 0);
        if(!template) {
            printf("[ - ] Error, Unable to construct template...!\n");
            SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "Error");
            return;
        }

        SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), template);
        return;
    }

    char *template = ConstructTemplate(Template, NULL, 1, 0, 0, 0, 0, 0);
    if(!template) {
        SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "Error, Unable to construct template...!\n");
        return;
    }

    SendResponse(server, req->Socket, OK, DefaultHeader, ((Map){0}), "welcome to /api_endpoint, request with GET or POST parameters to get started....!");
}

// Demostrating a live-updates using AddEventHandler(), custom form inputs (non-form tag) and 
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

    AddEventHandler(server);
    server->Run(server, 999, 0);
    return 0;
}