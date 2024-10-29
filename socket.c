#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "socket.h"

int MAX_BUFFER_SIZE = 1024;
inline void SetBufferSize(int c) { int MAX_BUFFER_SIZE = c; }

Socket Create_TCP_Socket(Hostname_T ip_t, String ip, int port) {
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

		.GetSocketIP		= Net__GetSocketIP,
		.SetReadTimeout		= SetSocketReadTimeOut,
		.Destruct			= DestroySocket
	};

	s.SockFD = socket(AF_INET, SOCK_STREAM, 0);
	if(s.SockFD == -1)
		return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	memset(&s.SockAddr, 0, sizeof(s.SockAddr));
	s.SockAddr.sin_family = AF_INET;
	s.SockAddr.sin_port = htons(s.Port);
	if(ip_t == IPv4 && strlen(ip.data) > 2)
		if(!inet_aton(ip.data, &s.SockAddr.sin_addr))
			return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	int reuse = 1;
	if(setsockopt(s.SockFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
		return (Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0};

	return s;
}

int SetSocketReadTimeOut(Socket *s, int timeout_len) {
	if(!s || s->SockFD <= 0)
		return 0;

	struct timeval timeout = {
		.tv_sec 	= timeout_len / 1000000,
		.tv_usec 	= timeout_len % 1000000
	};

	if(setsockopt(s->SockFD, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
		return 1;

	return 0;
}

int SetSocketWriteTimeOut(Socket *s, int timeout_len) {
	if(!s || s->SockFD <= 0)
		return 0;

	struct timeval timeout = {
		.tv_sec 	= timeout_len / 1000000,
		.tv_usec 	= timeout_len % 1000000
	};

	if(setsockopt(s->SockFD, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
		return 1;

	return 0;
}

int BindSocket(Socket *s) {
	if(!s || s->SockFD <= 0)
		return 0;

	if(bind(s->SockFD, (struct sockaddr *)&s->SockAddr, sizeof(s->SockAddr)) != 0)
		return 0;

	return 1;
}

int Connect(Socket *s) {
	if(!s || s->SockFD <= 0)
		return 0;

	return connect(s->SockFD, (struct sockaddr *)&s->SockAddr, (socklen_t)sizeof(s->SockAddr));
}

int Listen(Socket *s, int concurrent) {
	if(!s || s->SockFD <= 0)
		return 0;

	if(listen(s->SockFD, concurrent) != 0)
		return 0;

	return 1;
}

String Read(Socket *s) {
	if(!s || s->SockFD <= 0)
		return ((String){});

	char *BUFFER = (char *)malloc(MAX_BUFFER_SIZE);
	memset(BUFFER, '\0', MAX_BUFFER_SIZE);
	int bytes = read(s->SockFD, BUFFER, MAX_BUFFER_SIZE - 1);
	if(!bytes)
		return ((String){});

	return NewString(BUFFER);
}

int Write(Socket *s, const char *data) {
	if(!s || s->SockFD <= 0)
		return 0;

	int bytes_sent = send(s->SockFD, data, strlen(data), MSG_NOSIGNAL);
	if(bytes_sent == -1)
		return 0;

	return 1;
}

Socket Accept(Socket *s) {
	if(!s || s->SockFD <= 0)
		return ((Socket){.IP = NULL, .Port = 0, .SockFD = 0, .BufferLen = 0});

	Socket client;

	int len = sizeof(client.SockAddr);
	client.SockFD = accept(s->SockFD, (struct sockaddr *)&client.SockAddr, (socklen_t *)&len);

	return client;
}

void Net__GetSocketIP(Socket *s) {
	socklen_t addr_sz = sizeof(s->SockAddr);
	if(getpeername(s->SockFD, (struct sockaddr *)&s->SockAddr, &addr_sz))
		return;

	char ip[INET_ADDRSTRLEN] = {0};
	inet_ntop(AF_INET, &(s->SockAddr.sin_addr), ip, INET_ADDRSTRLEN);
	s->IP = NewString(ip);
	s->Port = ntohs(s->SockAddr.sin_port);
}

void DestroySocket(Socket *s) {
	if(s) {
		if(s->IP.data)
			s->IP.Destruct(&s->IP);

		if(s->SockFD > 0)
			close(s->SockFD);
	}
}

