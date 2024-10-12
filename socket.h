#pragma once

#include <str.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

typedef enum Hostname_T {
	IPv4		= 0x038001,
	IPv6		= 0x038002
} Hostname_T;

typedef struct Socket {
	String 				*IP;
	int					Port;
	int					SockFD;
	struct sockaddr_in	SockAddr;
	int					BufferLen;

	int					(*Bind)		(struct Socket *s);
	int					(*Connect)	(struct Socket *s);
	int					(*Listen)	(struct Socket *s);
	Socket				(*Accept)	(struct Socket *s);
	void				(*Destruct)	(struct Socket *s);
} Socket;

//
//
//
//
Socket Create_TCP_Socket(Hostname_T *ip_t, String *ip, int port);

//
//
//
//
static int BindSocket(Socket *s);

//
//
//
//
static int Connect(Socket *s);

//
//
//
//
static int Listen(Socket *s, int concurrent);

//
//
//
//
static Socket Accept(Socket *s);

//
//
//
//
static void DestroySocket(String *s);
