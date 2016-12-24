#include "UDPServer.h"
#include "NetUtil.h"

#define BUFLEN 1024

/* Start up a server accepting UDP packets and handling them with
 * onReceive, passing params to onReceive */
void RunUDPServer(const char *port,
	int (*onReceive)(int, const char *, int, void*),
	void *params) {

	int sockfd;
	int numbytes;

	UDPClient client;

	sockfd = Sock(NULL, port, UDP);

	client.len = sizeof(client.addr);

	for (;;) {
		memset(buf, 0, BUFLEN);
		// receive, get their_addr and their addr length
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&client.addr, &client.len)) == -1) {
			perror("RunUDPServer");
			exit(1);
		}

		onReceive(client, buf, numbytes, params);
	}

	// close
	close(sockfd);
}

void UDPSend(int sock, UDPClient c, const char *message, int len) {
	if (sendto(sock, message, len, 0, c.addr, c.addr_len) == -1)
		perror("UDPSend");
}