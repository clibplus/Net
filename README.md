<div align="center">
    <h1>cLib's Net Lib</h1>
    <p>A HTTP/S Library</p>
</div>

# Documentation

A HTTP/S library that include Websign Framework which is a HTML/CSS generative written in C for production use!

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

