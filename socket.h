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

	int					(*Bind)				(struct Socket *s);
	int					(*Connect)			(struct Socket *s);
	int					(*Listen)			(struct Socket *s, int concurrent);
	struct String		(*Read)				(struct Socket *s);
	int					(*Write)			(struct Socket *s, const char *data);
	struct Socket		(*Accept)			(struct Socket *s);

	int					(*SetReadTimeout)	(struct Socket *s, int timeout_len);
	int 				(*SetWriteTimeout)	(struct Socket *s, int timeout_len);
	void				(*Destruct)			(struct Socket *s);
} Socket;

extern int 		MAX_BUFFER_SIZE;

//
//				| - > Create a new intanse of a TCP Socket
//				| - > Returns an empty struct with SOcket info upon success or struct with 0s/NULL upon failure
//
Socket 			Create_TCP_Socket(Hostname_T ip_t, String *ip, int port);

//
//				| - > Set a socket read timeout
//				| - > Returns 1 upon success or 0 upon failure
//
static int 		SetSocketReadTimeOut(Socket *s, int timeout_len);

//
//				| - > Set a socket write timeout
//				| - > Returns 1 upon success or 0 upon failure
//
static int 		SetSocketWriteTimeOut(Socket *s, int timeout_len);
//
//				| - > Bind a socket to a port
//				| - > Returns 1 upon success or 0 upon failure
//
static int 		BindSocket(Socket *s);

//
//				| - > Connect to a server
//				| - > Returns 1 upon success or 0 upon failure
//
static int 		Connect(Socket *s);

//
//				| - > Concurrent count of sockets to listen to
//				| - > Returns 1 upon success or 0 upon failure
//
static int 		Listen(Socket *s, int concurrent);

//
//				| - > Read the socket for data
//				| - > Returns the data upon success or NULL upon failure
//
static String 	Read(Socket *s);

//
//				| - > Write to socket
//				| - > Returns 1 upon success or 0 upon failure
//
static int 		Write(Socket *s, const char *data);

//
//				| - > Accept a pending socket
//				| - > Returns a struct with socket info upon success or empty struct with 0s/NULL upon failure
//
static Socket 	Accept(Socket *s);

//
//				| - > Destroy the socket struct
//
static void 	DestroySocket(Socket *s);
