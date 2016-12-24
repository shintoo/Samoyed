#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TCP_SUCCESS 0
#define TCP_FAILURE 1

/* Set of file descriptors, one for each client */
typedef struct _user_set {
	fd_set master;
	int listener;
	int max;
} UserSet;

typedef int (*function1)(UserSet, int, void *);
typedef int (*function2)(UserSet, int, const char *, int, void *);

/* Determines the behavior of a server */
typedef struct _tcp_server_controller {
	function1 onConnect;
	function1 onDisconnect;
	function2 onReceive;
} TCPServerController;

/* Initializaion */
TCPServerController * MakeTCPServerController(function1 onConnect,
		function1 onDisconnect, function2 onReceive);

/* Start */
void RunTCPServer(TCPServerController *tsc, const char *port, void *paramlist);

/* Iterate over users (clients) in a UserSet */
#define for_each_user(I, U) \
 	for (I = 0; I <= (U).max; I++) \
 		if (FD_ISSET((I), &(U).master)) \
 			if (I != (U).listener)

#define disconnect_user(I, U) \
 	close(I); \
	FD_CLR(I, &(U).master);

#endif