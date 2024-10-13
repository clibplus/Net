#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "socket.h"

inline void SetBufferSize(int c) { int MAX_BUFFER_SIZE = c; }

Socket Create_TCP_Socket(Hostname_T ip_t, String *ip, int port) {
	Socket s = {
		.IP 				= ip,
		.Port 				= (!port ? 1337 : port),
		.SockFD	 			= -1,
		.BufferLen 			= 1024,

		.Bind				= BindSocket,
		.Connect			= Connect,
		.Listen 			= Listen,
		.Read				= Read,
		.Write				= Write,
		.Accept				= Accept,

		.SetReadTimeout		= SetReadTimeOut,
		.Destruct			= DestroySocket		
	};

	s.SockFD = socket(AF_INET, SOCK_STREAM, 0);
	if(s.SockFD == -1)
		return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	memset(&s.SockAddr, 0, sizeof(s.SockAddr));
	s.SockAddr.sin_family = AF_INET;
	if(ip)
		inet_aton(ip->data, &s.SockAddr.sin_addr);

	int reuse = 1;
	if(setsockopt(s.SockFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
		return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	s.SockAddr.sin_port = htons(s.Port);
	return s;
}

static int SetSocketReadTimeOut(Socket *s, int timeout_len) {
	struct timeval timeout = {
		.tv_sec 	= timeout_len,
		.tv_usec 	= timeout_len,
	};

	if(setsockopt(s->SockFD, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 1)
		return 1;

	return 0;
}

static int SetSocketWriteTimeOut(Socket *s, int timeout_len) {
	struct timeval timeout = {
		.tv_sec 	= timeout_len,
		.tv_usec 	= timeout_len,
	};

	if(setsockopt(s->SockFD, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 1)
		return 1;

	return 0;
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

static String *Read(Socket *s) {
	if(!s || !s->SockFD)
		return NULL;

	char BUFFER[MAX_BUFFER_SIZE] = {0};
	int bytes = read(s->SockFD, BUFFER, MAX_BUFFER_SIZE);
	if(!bytes)
		return NULL;

	return String(BUFFER);
}

static int Write(Socket *s, const char *data) {
	if(!s || !s->SockFD)
		return 0;

	int bytes_sent = write(s->SockFD, data, strlen(data), MSG_NOSIGNAL);
	if(bytes_sent == -1)
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

static void DestroySocket(Socket *s) {
	if(s->IP)
		free(s->IP);

	close(s->SockFD);
}
