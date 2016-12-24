#ifndef NETUTIL_H
#define NETUTIL_H

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

enum {
	TCP, UDP
};

int Sock(const char *ip, const char *port, int protocol);
void TCPSend(int s, const char *buf, int len);

#endif