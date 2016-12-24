#include <stdlib.h>
#include <stdio.h>
#include "TCPServer.h"
#include "NetUtil.h"

#define BUFLEN 1024

/* Create a new TCPServerController that handles client connections,
 * disconnections, and message sends */
TCPServerController * MakeTCPServerController(function1 oC, function1 oD, function2 oR) {
	TCPServerController *ret = malloc(sizeof(TCPServerController));
	if (!ret) {
		perror("TCPServer");
		return NULL;
	}

	ret->onConnect = oC;
	ret->onReceive = oR;
	ret->onDisconnect = oD;

	return ret;
}

/* Create a server on a port that handles client interactions using
 * the TCPServerController on the port */
void RunTCPServer(TCPServerController *tsc, const char *port, void *paramlist) {
	UserSet us;
	fd_set read_fds;

	int newfd;			// newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr;	// client address
	socklen_t addrlen;	// length of remoteaddr

	char buf[BUFLEN];	// buffer for client data
	int nbytes;			// number of bytes received/to send

	char remoteIP[INET6_ADDRSTRLEN];

	int yes=1;			// for setsockopt() SO_REUSEADDR
	int i, j, rv;		// ??, ??, returnvalue for getaddrinfo

	us.listener = Sock(NULL, port, TCP);

	struct timeval tv;	// Timeout after 10s in select()
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	// Set us up to listen for connections
	if (listen(us.listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	FD_ZERO(&us.master);
	FD_ZERO(&read_fds);

	// Add the listener to the master set
	FD_SET(us.listener, &us.master);

	// Keep track of the biggest file descriptor
	// Right now, it's our only one, listener
	us.max = us.listener;

	// main loop
	for (;;) {
		// Copy the fd set so our master doesn't get overwritten
		// from select()
		read_fds = us.master;

		if (select(us.max+1, &read_fds, NULL, NULL, &tv) == -1) {
			perror("select");
			exit(4);
		}

		// Run through existing connections looking for data to read
		for (i = 0; i <= us.max; i++) {
			// One of the socket fd's was placed in the read_fds by select()
			// to show that it has data to be read
			if (FD_ISSET(i, &read_fds)) {
				if (i == us.listener) { // is the listener socket
					// handle new connections
					addrlen = sizeof(remoteaddr);
					newfd = accept(us.listener, (struct sockaddr*) &remoteaddr,
							&addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						// Add new connection to master set
						FD_SET(newfd, &us.master);
						if (newfd > us.max)
							us.max = newfd;
						if (tsc->onConnect(us, newfd, paramlist) == TCP_FAILURE)
							return;
					}
				} else { // not the listener socket (a connection socket)
					memset(buf, 0, BUFLEN);
					// error or connection closed
					if ((nbytes = recv(i, buf, BUFLEN, 0)) <= 0) {
						// connection closed
						if (nbytes == 0) {
							if (tsc->onDisconnect(us, i, paramlist) == TCP_FAILURE)
								return;
						} else
							perror("recv");
						close(i); // bye!
						FD_CLR(i, &us.master); // remove from master set
					} else { // bytes were read
						if (tsc->onReceive(us, i, buf, nbytes, paramlist) == TCP_FAILURE)
							return;
					}
				} // handle data from client
			} // get new incoming connection
		} // looping through fds
	} // main loop
}