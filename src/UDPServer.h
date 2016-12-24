#ifndef UDP_H
#define UDP_H

/* A function determining the behavior of a UDP server on reception
 * of data from a client */
typedef void (*UDPReceiveFun)(int socket, UDPClient, const char *, int, void*);

/* Stores client data as no connections are made on UDP, so the client
 * address is needed for response */
typedef struct _client {
	struct sockaddr_storage addr;
	socklen_t len;
} UDPClient;

/* Start up a server accepting UDP packets and handling them with
 * onReceive, passing params to onReceive */
void RunUDPServer(const char *port, UDPReceiveFun onReceive, void *params);

#endif