<div align="center">
    <h1>cLib's Net Lib</h1>
    <p>A HTTP/S Library</p>
</div>

<h2 align="center"> Table of Content</h2>
<table align="center">
<tr><td valign=top>

<p>About<p>

* [Information](#what-is-websign-)
    * [Why](#why-websign-)
    * [Standards](#why-websign-)
</td>
<td valign=top>

<p>Developers</p>

* [Dev Set-Up](#Installation)
    * [Minimal Web-app Start up](#initialize-websigns-c-web-server-cws)
    * [Full Example](#complete-example)
</td></tr>
</table>

<table align="center">
<tr><td>

* [Doc](https://github.com/clibplus/Net/blob/main/websign.md)
</td></tr>
</table>

# Introduction

### What is Websign .? 

Websign is a web server and browser SDK toolchain that generates HTML, CSS, and JavaScript using structs, nesting them in a structure similar to traditional HTML.

It is a hobbyist project but is built for production use with complete memory management!

The library aims to mimic or replace other web development libraries with a GUI-styled syntax library while pushing the boundaries of dynamic web generation.
With Websign, We're looking to create a public community where everyone share components with each other to work on advancing web designing in programming features.

<details><summary>An example of the syntactic design we're aiming for the use of Websign Usage!</summary>

Instead of just having a pre-made template in-file, We're looking to make Web-designing as close as GUI-designing.
```c
Control *parent = CreateControl(BODY_TAG, NULL, NULL, NULL, (Control *[]){
    CreateControl(P_TAG, NULL, NULL, "Click the button", NULL),
    CreateButton(INPUT_TAG, "submit_btn", "green_huge_btn", "Click Me", NULL),
    NULL
});

void handler( args ... ) {
    if(parent->isClicked(req->Event)) {
        // handle button click
        // UpdateUI(Control **Controls, CSS **Styles);
        return; //exit 
    }

    // send regular GET response
}
```
</details>

### Why Websign .? 

Most web development frameworks rely on pre-made templating systems with limited modification capabilities, often restricting flexibility and adding unnecessary bloat.

As programmers, we prefer dynamic solutions over hardcoded ones. Websign enables runtime page modifications, eliminating the need to manually create new files or templates. This approach allows for better dynamic event handling and greater automation in web applications **with back compatibility** for external files!

Another main reason is, If you have to load file per request (like Apache) or store a template for routes in memory, why not just load the components using programming features... Eliminating the need to navigate through a front-end file tree structure and the code!

### Websign's Web-server Standards

Websign was designed to follow HTTP 1.0 for security reasons with one of them being 
> **"A quick connection close is better than waiting for the idle delay to detect disconnection with ton of threads during an attack"**

However, this does not prevent Websign from supporting HTTP 1.1. While the library is built with a low-level approach for maximum flexibility, HTTP 1.1 can be enabled by setting up headers and adjusting handler(s)

PS. I might look into making some Websign functions to move connections to 1.1 and//or websocket!

# Installation 

We currently have an un-official installation script for maintainers, testers and experimental devs.
You can install Websign as quick in 2 commands listed below
```
git clone https://github.com/clibplus/Net.git
cd Net; sh install.sh
```

# A Minimal Hello-World-WebApp Startup

### Initialize Websign's C Web-Server (cWS)
Initialize your web-app using 

```c
cWS *StartWebServer(String IP, int port, int auto_search);
```

PS: You can provide an empty String to host for 127.0.0.1 (``NewString("")``)

<details><summary>Example Snippet</summary>

```c
cWS *my_app = StartWebServer(NewString(""), 80, 0);
```
</details>

### Add Routes

Create your function handlers and add them using either one of the following function pointers from cWS struct with the route creation function and SendResponse function for responding to request(s)

```c
int (*AddRoute)  (cWS *server, WebRoute routes);
int (*AddRoutes) (cWS *server, WebRoute **routes);

WebRoute *CreateRoute(const char *n, const char *p, void *handler);

void SendResponse(cWS *web, int request_socket, StatusCode code, Map headers, Map cookies, const char *body);
```

<details><summary>Example Snippet</summary>

```c

// Use 1.0 Default and basic headers
void *index_handler(cWS *server, cWR *req, WebRoute *route, int socket) {
    SendResponse(server, socket, OK, DefaultHeaders, ((Map){0}), "Hello World");
}

// AddRoute
my_app->AddRoute(my_app, (WebRoute){ .Name = "index", .Path = "/", .Handler = index_handler });

// AddRoutes
WebRoute *Routes[1][] = {
    CreateRoute("index", "/", index_handler),
    NULL
};
my_app->AddRoutes(my_app, Routes);
```
</details>

### Run the Web-server

You can start the web-server using the function pointer from cWS struct below

```c
void (*Run) (struct cWS *web, int concurrents, const, char *search_path);
```

<details><summary>Example Snippet</summary>

```c
my_app->Run(server, 99, 0); // Adjust concurrents to your needs!
```
</details>

### Clean up the web-server and exit

You can clean up the server and exit by using the destructor!

```c
void (*Destruct) (struct cWS *web);
```

```c
my_app->Destruct(my_app);
```

### Complete Example

<details><summary>Full Example Snippet</summary>

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <map.h>
#include <Net/web.h>

// Use 1.0 Default and basic headers
void *index_handler(cWS *server, cWR *req, WebRoute *route, int socket) {
    SendResponse(server, socket, OK, DefaultHeaders, ((Map){0}), "Hello World");
}

int main() {
    // Initialize cWS
    cWS *my_app = StartWebServer(NewString(""), 80, 0);
    if(!my_app)
    {
        printf("[ - ] Error, Unable to start web server....");
        return 1;
    }

    
    // AddRoute
    int route_check = my_app->AddRoute(my_app, (WebRoute){ .Name = "index", .Path = "/", .Handler = index_handler });
    if(!route_check) {
        printf("[ x ] Error, Unable to add route...!\n");
        return 1;
    }

    // Run the web-server (Can be threaded if needed)
    my_app->Run(my_app, 999, 0);

    // Clean up and exit
    my_app->Destruct(my_app);
    return 0;
}
```
</details>

# Memory Management, Undefined Behavior, and Vulnerabilities

This library is written for modern C and has been tested on AMD 64-bit Ubuntu. Most bugs arise due to testing new features. While I try to cover as many issues as possible, the repository is updated frequently, and some updates may or may not be stable.

Official versions are released in the repository on the release tab and are not intended for development purposes.

If you encounter a bug in a released version, please report it here!

# Discord 

Join our discord community! https://discord.gg/8Xzc4tKUe9