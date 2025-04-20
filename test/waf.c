#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <arr.h>
#include <map.h>
#include <OS/utils.h>
#include <Net/web.h>

#include <___utils___.h>

cWS *server = NULL;
typedef Array IPStack;
IPStack Blocked = {0};

int is_ip_blocked(char *ip) {
    if(!ip || Blocked.idx < 1)
        return 0;

    for(int i = 0; i < Blocked.idx; i++) {
        if(!Blocked.arr[i])
            break;

        if(!strcmp(Blocked.arr[i], "ip"))
            return 1;
    }

    return 0;
}

int validate_ipv4_format(char *ip) {
    String r = NewString(ip);

    // Validate 3 periods '.'
    if(r.CountChar(&r, '.') != 3)
        return 0;

    int iplen = strlen(ip);
    if(iplen < strlen("0.0.0.0") || iplen > strlen("255.255.255.255")) {
        r.Destruct(&r);
        return 0;
    }

    Array args = NewArray(NULL);
    args.Merge(&args, (void **)r.Split(&r, "."));

    if(args.idx != 4) {
        r.Destruct(&r);
        args.Destruct(&args, 1, 1);
        return 0;
    }

    int arg1 = atoi(args.arr[0]);
    int arg2 = atoi(args.arr[1]);
    int arg3 = atoi(args.arr[2]);
    int arg4 = atoi(args.arr[3]);
    if((arg1 < 1 || arg1 > 255) ||
       (arg2 < 0 || arg2 > 255) || 
       (arg3 < 0 || arg3 > 255) ||
       (arg4 < 0 || arg4 > 255)) {
        r.Destruct(&r);
        args.Destruct(&args, 1, 1);
        return 0;
    }

    r.Destruct(&r);
    args.Destruct(&args, 1, 1);
    return 1;
}

int validate_ipv6_format(char *ipv6) {
    if(!strstr(ipv6, ":")) {
        return 0;
    }

    String r = NewString(ipv6);
    if(r.CountChar(&r, ':') != 8) {
        r.Destruct(&r);
        return 0;
    }

    Array n = NewArray(NULL);
    n.Merge(&n, (void **)r.Split(&r, ":"));

    if(n.idx != 8) {
        r.Destruct(&r);
        n.Destruct(&n, 1, 1);
        return 0;
    }

    for(int i = 0, len = strlen(n.arr[i]); i < n.idx; i++, len = strlen(n.arr[i != n.idx ? i : n.idx - 1]) ) {
        if((len != 4)) {
            r.Destruct(&r);
            n.Destruct(&n, 1, 1);
            return 0;
        }
    }

    r.Destruct(&r);
    n.Destruct(&n, 1, 1);
    return 1;
}

String construct_ip_block(char *ip, int ip4) {
    if(!ip)
        return ((String){0});

    if(ip4) {
        String cmd = NewString(strdup("sudo iptables -A INPUT -s "));
        cmd.AppendArray(&cmd, (const char *[]){
            ip, " -j DROP; sudo iptables -A OUTPUT -s ", ip, " -j DROP", "; sudo iptables-save; sudo ip6tables-save",
            NULL
        });

        cmd.data[cmd.idx] = '\0';
        return cmd;
    }

    //sudo iptables -A INPUT -s 116.98.163.112 -j DROP; sudo iptables -A OUTPUT -s 116.98.163.112 -j DROP; sudo iptables-save; sudo ip6tables-save
    String cmd = NewString(strdup("sudo ip6tables -A INPUT -s "));
    cmd.AppendArray(&cmd, (const char *[]){
        ip, " -j DROP; sudo ip6tables -A OUTPUT -s ", ip, " -j DROP", "; sudo iptables-save; sudo ip6tables-save",
        NULL
    });

    cmd.data[cmd.idx] = '\0';
    return cmd;
}

void *test_p(cWS *web, cWR *req, WebRoute *route) {
    char *resp = "Websign WAF | IPs Blocked: N/A";
    if(((Array *)route->Args[0])->idx > 0) {
        char *num = iString(((Array *)route->Args[0])->idx);
        resp = CreateString((char *[]){"Websign WAF | IPs Blocked: ", num, NULL});
        SendResponse(web, req->Socket, OK, DefaultHeaders, ((Map){0}), resp);
        free_many_strs((void *[]){resp, num, NULL}, 0);

    }
    SendResponse(web, req->Socket, OK, DefaultHeaders, ((Map){0}), resp);
}

void Detection() {
    int attack_mode = 0;
    while(1) {
        // printf("Attack Mode: %s | Blocked IPs: %ld\n", 
        //     (server->ThreadPool->ThreadCount >= server->ThreadPool->MAX_THREADS > 0 ? "True" : "False"),
        //     Blocked.idx
        // );

        if(server->ThreadPool->ThreadCount >= server->ThreadPool->MAX_THREADS)
            attack_mode = 1;

        if(attack_mode && server->ThreadPool->ThreadCount < server->ThreadPool->MAX_THREADS) {
            attack_mode = 0;
            Blocked.Destruct(&Blocked, 1, 1);
            Blocked = NewArray(NULL);
            ExecuteCmd("sudo iptables -F; sudo ip6tables -F");
        }
        sleep(1);
    }
}

int WS_Middleware(cWS *web, cWR *req, WebRoute *route) {
    int pos = req->Headers.InMap(&req->Headers, "x-forwarded-for");
    char *cf = (pos > -1 ? ((Key *)req->Headers.arr[pos])->value : "N/A ");

    String rcf = NewString(strdup(cf));
    rcf.data[rcf.idx - 1] = '\0';

    printf("[ + ] Client: %s | CF: '%s'\n", req->ClientIP, rcf.data);

    if(is_ip_blocked(rcf.data) || is_ip_blocked(req->ClientIP)) {
        printf("[ + ] Suspecious Request from %s on %s", rcf.data, req->Route.data);
        SendResponse(web, req->Socket, UNAUTHORIZED, DefaultHeaders, ((Map){0}), "You are blocked from using this web-server....!\n");
        return 0;
    }

    if(web->ThreadPool->ThreadCount >= web->ThreadPool->MAX_THREADS) {
        SendResponse(web, req->Socket, OK, DefaultHeaders, ((Map){0}), "Error, We're under heavy load....!");
        
        String cmd =  construct_ip_block(rcf.data, validate_ipv4_format(rcf.data));
        if(cmd.idx < 0)
        {
            printf("ERROR\n");
            return 0;
        }

        printf("IP Blocked: %s\n", rcf.data);
        char *_n = ExecuteCmd(cmd.data);

        Blocked.Append(&Blocked, strdup(rcf.data));
        cmd.Destruct(&cmd);
        free(_n);
        return 0;
    }

    return 1;
}

int main() {
    String IP = NewString(strdup("IP_ADDRESS"));
    Blocked = NewArray(NULL);
    server = StartWebServer(IP, 80, 0, 40);
    if(!server)
    {
        printf("[ - ] Error, Unable to start web-server....!\n");
        return 1;
    }

    server->CFG.Middleware = WS_Middleware;

    Array r = NewArray(NULL);
    r.Append(&r, CreateRoute("index", "/", test_p));
    r.arr[r.idx] = NULL;

    server->AddRoutes(server, (WebRoute *[]){
        &(WebRoute){ .Name = "index", .Path = "/", .Handler = test_p, .Args = (void *[]){&Blocked, NULL} },
        NULL
    }, 1);

    pthread_t id;
    pthread_create(&id, NULL, (void *)Detection, NULL);

    server->Run(server, 0);
    return 0;
}