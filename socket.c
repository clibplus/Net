#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "socket.h"

Socket Create_TCP_Socket(Hostname_T *ip_t, String *ip, int port) {
	Socket s = {
		.IP 		= ip,
		.Port 		= (!port ? 1337 : port),
		.SockFD	 	= -1,
		.BufferLen 	= 1024,

		.Bind		= BindSocket,
		.Connect	= Connect,
		.Listen 	= Listen,
		.Accept		= Accept,
		.Destruct	= DestroySocket		
	};

	s.SockFD = socket(AF_INET, SOCK_STREAM, 0);
	if(s.SockFD == -1)
		return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	bzero(&s.SockAddr, sizeof(s.SockAddr));
	s.SockAddr.sin_family = AF_INET;
	if(!ip)
		inet_aton(ip->data, &s.SockAddr.sin_addr);

	int reuse = 1;
	if(setsockopt(s.SockFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
		return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	s.SockAddr.sin_port = htons(s.Port);
	return s;
}

static int BindSocket(Socket *s) {
	if(bind(s->SockFD, (struct sockaddr *)&s->SockAddr, sizeof(s->SockAddr)) != 0)
		return 0;

	return 1;
}

static int Connect(Socket *s) {
	return connect(s->SockFD, (struct sockaddr *)&s->SockAddr, (socklen_t)sizeof(s->SockAddr));
}

static int Listen(Socket *s, int concurrent) {
	if(!s || s->SockFD == -1)
		return 0;

	if(listen(s->SockFD, concurrent) != 0)
		return 0;

	return 1;
}

static Socket Accept(Socket *s) {
	if(!s || s->SockFD == -1)
		return ((Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0});

	Socket client;

	int len = sizeof(client.SockAddr);
	client.SockFD = accept(s->SockFD, (struct sockaddr *)&client.SockAddr, &len);
	
	return client;
}

static void DestroySocket(String *s) {
	if(s->IP)
		free(s->IP);

	close(s->SockFD);
}
