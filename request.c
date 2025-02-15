#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "request.h"

// TODO: ***OPTIMIZATION***
Array ParseURL(const char *u) {
	String hostname = NewString(u);
	Array args = NewArray(NULL);
	hostname.data[hostname.idx] = '\0';

	/* Check for substring to remove */
	char *CheckArgs[] = {"https://", "http://", "www.", NULL};
	for(int i = 0 ; CheckArgs[i] != NULL; i++) {
		int chars = 0;
		if(!hostname.Contains(&hostname, CheckArgs[i]))
			continue;

		chars = strlen(CheckArgs[i]); 
		for(int i = 0; i < chars; i++)
			hostname.TrimAt(&hostname, 0);
	}

	if(hostname.Contains(&hostname, "/")) {
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

HTTPClientResponse RequestURL(const String URL, const Map h, const Req_T reqt) {
	Array hostname = ParseURL(strdup(URL.data));
	HTTPClient c = {
		.Hostname	= NewString(hostname.arr[0]),
		.Route 		= NewString(hostname.arr[1]),
		.URL_Route 	= URL,
		.Headers	= h,
		.Port 		= NewString((strstr(URL.data, "https://") ? "443" : "80"))
	};

	c.ServerFD = CreateHTTPSocket(&c);
	if(c.ServerFD < 1) {
		printf("[ x ] Error, Unable to start socket....!\n");
		return ((HTTPClientResponse){});
	}

	int check = 0;
	HTTPClientResponse Response;

	if(!strcmp(c.Port.data, "443")) {
		InitOpenSSL();
		c.CTX = CreateCTXContext();
		if(!c.CTX) {
			return ((HTTPClientResponse){});
		}

		c.SSL = CreateSSL(c.CTX);
		if(!c.SSL)
			return ((HTTPClientResponse){});


		if(SSL_set_fd(c.SSL, c.ServerFD) == 0) {
			return ((HTTPClientResponse){});
		}

		if(SSL_connect(c.SSL) != 1)
			return ((HTTPClientResponse){});

		if(reqt == __GET__) {
			int chk = SendHTTPGetReq(&c);
			Response = RetrieveHTTPResponse(&c);
			HTTPClientResponse empty;
			if(memcpy(&Response, &empty, sizeof(HTTPClientResponse)) || !ExtractRawTraffic(&c, &Response))
				return ((HTTPClientResponse){});
		} else {
			return ((HTTPClientResponse){});
		}

		SSL_free(c.SSL);
		SSL_CTX_free(c.CTX);
		CleanOpenSSL(c.SSL, c.CTX);
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
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
	}

	if(getaddrinfo(c->Hostname.data, c->Port.data, &hints, &res) != 0)
		return 0;

	c->ServerFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(c->ServerFD < 0)
		return 0;

	if(connect(c->ServerFD, res->ai_addr, res->ai_addrlen) < 0)
		return 0;

	int reuse = 1;
	if(setsockopt(c->ServerFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
		return 1;

	freeaddrinfo(res);
	return c->ServerFD;
}

int SendHTTPGetReq(HTTPClient *c) {
	if(!c)
		return 0;

	if(c->URL_Route.Contains(&c->URL_Route, "https://"))
		for(int i = 0; i < strlen("https://"); i++) 
			c->URL_Route.TrimAt(&c->URL_Route, 0);

	
	if(c->URL_Route.Contains(&c->URL_Route, "http://"))
		for(int i = 0; i < strlen("http://"); i++) 
			c->URL_Route.TrimAt(&c->URL_Route, 0);

	if(c->URL_Route.EndsWith(&c->URL_Route, "/"))
		c->URL_Route.TrimAt(&c->URL_Route, c->URL_Route.idx - 1);

	char *req_data[] = {"GET ", c->Route.data, " HTTP/1.1\r\nHost: ", c->URL_Route.data, "\r\n"};
	String req = NewString(NULL);
	for(int i = 0; i < 5; i++) req.AppendString(&req, req_data[i]);

	if(c->Headers.idx > 0) {
		for(int i = 0; i < c->Headers.idx; i++) {
			req.AppendArray(&req, (const char *[]){
				(char *)((Key *)c->Headers.arr[i])->key, 
				": ",
				(char *)((Key *)c->Headers.arr[i])->value, "\r\n", NULL
			});
		}
	}

	req.AppendString(&req, "\r\n");
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

	int max_length;
	if(!strcmp(c->Port.data, "443")) {
		while((bytes = SSL_read(c->SSL, BUFFER, sizeof(BUFFER) - 1)) != 0) {
			{
				if(strstr((char *)&BUFFER, "Content-Length:")) {
					String chk = NewString(BUFFER);
					char **arr = chk.Split(&chk, ":");

					String val = NewString(arr[1]);
					if(val.isNumber(&val)) {
						max_length = atoi(val.data);
					}

					val.Destruct(&val);
				}
			}
			
			r.Body.AppendString(&r.Body, (const char *)&BUFFER);
			memset(BUFFER, '\0', 4096);

			if(strlen(BUFFER) - 1 >= max_length && max_length > 0)
				break;
		}

		r.Body.data[r.Body.idx] = '\0';
		return r;
	}
	
	bytes = read(c->ServerFD, BUFFER, 4095);
	r.Body.AppendString(&r.Body, (const char *)&BUFFER);
	r.Body.data[r.Body.idx - 1] = '\0';

	return r;
}

int ExtractRawTraffic(HTTPClient *c, HTTPClientResponse *r) {
	if(!c || !r || !r->Body.data)
		return 0;

	String copy 		= NewString(r->Body.data);
	Array lines 		= NewArray(NULL);
	lines.Merge(&lines, (void **)copy.Split(&copy, "\n"));

	Map headers 		= NewMap();
	String Body 		= NewString(NULL);
	
	// if(lines.idx == 0)
	// 	return 0;

	/* Grab Status Code And HTTP Version (1.1) */
	String version 		= NewString(lines.arr[0]);
	Array status 		= NewArray(NULL);
	status.Merge(&status, (void **)version.Split(&version, " "));
	version.RmSubstr(&version, strlen(status.arr[0]), strlen(status.arr[1]) + strlen(status.arr[2]) - 1); // remove status code number and string '200 OK'

	r->StatusCode = atoi(status.arr[1]);
	version.Destruct(&version);
	status.Destruct(&status);

	/* Grab all headers */
	int stop = 0;
	for(int i = 1; i < lines.idx; i++) {
		if(!strcmp(lines.arr[i], "") || !strcmp(lines.arr[i], "2be")) {
			stop = 1;
			continue;
		} else if(!strcmp(lines.arr[i], "0")) {
			break;
		}

		String line		= NewString(lines.arr[i]);
		Array args 		= NewArray(NULL);
		args.Merge(&args, (void **)line.Split(&line, (const char *)":"));
		line.Strip(&line);
		if(!strcmp(line.data, "") || !strcmp(line.data, " ") || line.isEmpty(&line) || line.StartsWith(&line, "2e")) {
			stop = 1;
			continue;
		}

		if(args.idx >= 1 && !stop && !line.StartsWith(&line, "<")) {
			String value = NewString(NULL);
			for(int i = 1; i < args.idx; i++) value.AppendArray(&value, (const char *[]){args.arr[i], (i == args.idx - 1 ? NULL : ":"), NULL});
			value.Trim(&value, '\r');
			value.Trim(&value, '\n');
			headers.Append(&headers, args.arr[0], value.data);
			value.Destruct(&value);
		} else {
			Body.AppendString(&Body, lines.arr[i]);
		}

		line.Destruct(&line);
		args.Destruct(&args);
	}

	copy.Destruct(&copy);
	if(r->Body.idx > 1)
		r->Body.Destruct(&r->Body);

	r->Body = Body;
	r->Headers = headers;
	return 1;
}