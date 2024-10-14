#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "request.h"

// TODO: ***OPTIMIZATION***
Array ParseURL(const char *u) {
	String hostname = NewString(u);
	Array args = NewArray(NULL);

	if(strstr(hostname.data, "https://"))
		hostname.RemoveSubstr(&hostname, 0, 7);

	if(strstr(hostname.data, "http://"))
		hostname.RemoveSubstr(&hostname, 0, 6);

	if(strstr(hostname.data, "www."))
		hostname.RemoveSubstr(&hostname, 0, 4);

	if(strstr(hostname.data, "/")) {
		if(hostname.data[hostname.idx - 1] == '/') {
			hostname.Trim(&hostname, '/');
			args.Append(&args, hostname.data);
			args.Append(&args, "/");
			return args;
		}

		char ch = hostname.FindChar(&hostname, '/');
		char *url = hostname.GetSubstr(&hostname, 0, ch);
		char *p = hostname.GetSubstr(&hostname, ch, hostname.idx);

		args.Append(&args, url);
		args.Append(&args, (!p ? ((char *)"/") : p));

		return args;
	}

	args.Append(&args, hostname.data);
	args.Append(&args, "/");
	return args;
}

HTTPClientResponse ReqGET_URL(const String URL, const Map h, const Req_T reqt) {
	Array hostname = ParseURL(URL.data);
	HTTPClient c = {
		.Hostname	= NewString(hostname.arr[0]),
		.URL_Route 	= NewString(hostname.arr[1]),
		.Headers	= h,
		.Port 		= NewString((strstr(URL.data, "https://") ? "443" : "80"))
	};

	c.ServerFD = CreateHTTPSocket(&c);
	int check = 0;
	HTTPClientResponse Response;

	if(!strcmp(c.Port.data, "443")) {
		InitOpenSSL();
		c.CTX = CreateSSLContext();
		c.SSL = SSL_new(c.CTX);
		SSL_set_fd(c.SSL, c.ServerFD);

		if(SSL_connect(c.SSL) <= 0)
			return ((HTTPClientResponse){});

		if(reqt == __GET__) {
			SendHTTPGetReq(&c);
			HTTPClientResponse empty;
			if(memcpy(&Response, &empty, sizeof(HTTPClientResponse)) || !ExtractRawTraffic(&c, &Response))
				return ((HTTPClientResponse){});
		} else {
			return ((HTTPClientResponse){});
		}

		SSL_free(c.SSL);
		SSL_CTX_free(c.CTX);
		CleanOpenSSL();
		close(c.ServerFD);

		return Response;
	}

	SendHTTPGetReq(&c);
	Response = RetrieveHTTPResponse(&c);
	ExtractRawTraffic(&c, &Response);

	close(c.ServerFD);
	return Response;
}

int CreateHTTPSocket(HTTPClient *c) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	{
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
	}

	if(getaddrinfo(c->Hostname.data, c->Port.data, &hints, &res) != 0)
		return 0;

	c->ServerFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(c->ServerFD < 0)
		return 0;

	if(connect(c->ServerFD, res->ai_addr, res->ai_addrlen) < 0)
		return 0;

	freeaddrinfo(res);
	return c->ServerFD;
}

int SendHTTPGetReq(HTTPClient *c) {
	if(!c)
		return 0;

	char *req_data[] = {"GET", c->URL_Route.data, "HTTP/1.1\r\nHost: ", c->Hostname.data, "\r\n"};
	String req = NewString(NULL);
	for(int i = 0; i < 5; i++) req.AppendString(&req, req_data[i]);

	if(c->Headers.idx > 0) {
		for(int i = 0; i < c->Headers.idx; i++) {
			char *current_header[] = {
				(char *)((Key *)c->Headers.arr[i])->key, 
				": ", 
				(char *)((Key *)c->Headers.arr[i])->value, "\r\n"
			};
			for(int arg = 0; arg < 4; arg++) req.AppendString(&req, current_header[arg]);
		}
	}

	(void)(!strcmp(c->Port.data, "443") ? SSL_write(c->SSL, req.data, strlen(req.data)) : write(c->ServerFD, req.data, strlen(req.data)));
	req.Destruct(&req);

	return 1;
}

HTTPClientResponse RetrieveHTTPResponse(HTTPClient *c) {
	if(!c || c->ServerFD <= 0)
		return ((HTTPClientResponse){});

	char BUFFER[4096] = {0};
	int bytes = 0;

	HTTPClientResponse r = {
		.FullRoute = c->URL_Route,
		.Body = NewString(NULL)
	};

	if(!strcmp(c->Port.data, "443")) {
		while((bytes = SSL_read(c->SSL, BUFFER, sizeof(BUFFER) - 1)) > 0) {
			BUFFER[bytes] = '\0';
			r.Body.AppendString(&r.Body, (const char *)&BUFFER);
			memset(BUFFER, '\0', 4096);
		}

		return r;
	}
	
	bytes = read(c->ServerFD, BUFFER, 4095);
	r.Body.AppendString(&r.Body, (const char *)&BUFFER);

	return r;
}

int ExtractRawTraffic(HTTPClient *c, HTTPClientResponse *r) {
	if(!c || !r || !r->Body.data)
		return 0;

	String copy 		= NewString(r->Body.data);
	Array lines 		= NewArray((const void **)copy.Split(&copy, "/"));
	int line_count 		= copy.CountChar(&copy, '\n');

	Map headers 		= NewMap();
	String Body 		= NewString(NULL);
	
	if(line_count == 0)
		return 0;

	/* Grab Status Code And HTTP Version (1.1) */
	String version 		= NewString(lines.arr[0]);
	Array status 		= NewArray((const void **)version.Split(&version, " "));
	version.Replace(&version, status.arr[0], "");

	for(int i = 0; i < status.idx; i++) {
		version.AppendString(&version, status.arr[i]);
		version.AppendString(&version, " ");
	}

	r->StatusCode = atoi(status.arr[1]);
	headers.Append(&headers, status.arr[0], version.data);
	version.Destruct(&version);
	status.Destruct(&status);

	/* Grab all headers */
	int stop = 0;
	for(int i = 1; i < line_count; i++) {
		if(!strcmp(lines.arr[i], "") || !strcmp(lines.arr[i], "2be")) {
			stop = 1;
			continue;
		} else if(!strcmp(lines.arr[i], "0")) {
			break;
		}

		String line		= NewString(lines.arr[i]);
		Array args 		= NewArray((const void **)line.Split(&line, (const char *)":"));
		int arg_count 	= line.CountChar(&line, ':');
		line.Strip(&line);
		if(!strcmp(line.data, "")) {
			stop = 1;
			continue;
		}

		if(arg_count >= 2 && !stop) {
			String value = NewString(lines.arr[i]);
			value.Replace(&value, args.arr[0], "");
			headers.Append(&headers, args.arr[0], value.data);
			value.Destruct(&value);
		} else {
			Body.AppendString(&Body, lines.arr[i]);
		}

		line.Destruct(&line);
		args.Destruct(&args);
	}

	copy.Destruct(&copy);
	lines.Destruct(&lines);
	r->Body.Destruct(&r->Body);
	r->Body = Body;
	r->Headers = headers;
	return 1;
}
