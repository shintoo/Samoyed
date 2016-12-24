#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "NetUtil.h"

/* Create a new socket and bind it, connect to specified server
 * if on TCP */
int Sock(const char *ip, const char *port, int protocol) {
	int yes=1;			// for setsockopt() SO_REUSEADDR
	int rv;		// ??, ??, returnvalue for getaddrinfo

	struct addrinfo hints,
					*ai,
					*p;

	int sock;

	// get a socket and bind it
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // IPv4 or IPv6, who cares
	hints.ai_socktype = protocol == TCP ? SOCK_STREAM : SOCK_DGRAM; // TCP
	if (!ip)
		hints.ai_flags = AI_PASSIVE; // use OUR IP address

	if ((rv = getaddrinfo(ip, port, &hints, &ai)) != 0) {
		fprintf(stderr, "%s\n", gai_strerror(rv));
		return -1;
	}

	// ai is now a linked list of possible addresses for
	// us to use

	for (p = ai; p != NULL; p = p->ai_next) {
		// Try to open socket on this address
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock < 0)
			continue;

		// Allow ports to be reused or something
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (!ip) {
			if (bind(sock, p->ai_addr, p->ai_addrlen) < 0) {
				close(sock);
				// this one didnt work, try the next one
				continue;
			}
		} else {
			if (protocol == TCP) {
				if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
	        	    close(sock);
	            	perror("Sock: connect");
            		continue;
            	} else {
            		break;
            	}
        	}
        }

		break;
	}

	// If we got to the end of the linked list, then we couldn't open a socket
	// or bind an opened socket
	if (p == NULL) {
		return -1;
	}

	// Free the linked list we used to get the address
	freeaddrinfo(ai);

	return sock;
}

/* Send data over TCP, makes sure all data is sent */
void TCPSend(int s, const char *buf, int len) {
	int total = 0;
	int bytesleft = len;
	int n;

	while (total < len) {
		n = send(s, buf+total, bytesleft, 0);
		if (n == -1)
			break;
		total += n;
		bytesleft -= n;
	}

	if (n == -1) {
		perror("sendall");
		printf("socket: %d\n", s);
	}
}
