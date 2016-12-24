#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../../src/NetUtil.h"

#define BUFLEN 1024

/* Remove a new line at the end of a buffer */
void removeNewline(char *buffer) {
	char *nl;

	nl = strchr(buffer, '\r');
	if (nl)
		*nl = '\0';
	
	nl = strchr(buffer, '\n');
	if (nl)
		*nl = '\0';

}

/* Get the username from the user and send it to the server */
void username(int socket) {
	char buffer[BUFLEN];
	char namebuffer[BUFLEN];
	char *nl;

	/* Get name */
	printf("Enter your name: ");
	fgets(namebuffer, BUFLEN, stdin);

	/* Ensure it's under 13 chars long */
	while (strlen(namebuffer) > 14) {
		printf("Name may not exceed 12 characters.\nEnter your name: ");
		fgets(namebuffer, BUFLEN, stdin);
	}

	/* Send login to server */
	removeNewline(namebuffer);
	snprintf(buffer, BUFLEN, "l:%s", namebuffer);
	TCPSend(socket, buffer, strlen(buffer));
}

/* Handle '/'-prefixed commands client-side */
void handleCommand(int fd, char *message, int *running) {
	if (!strcmp(message, "quit")) {
		TCPSend(fd, "e", 2);
		recv(fd, message, BUFLEN, 0);
		puts(message);
		*running = 0;
	}
}

/* Send a message to the server */
void sendMessage(int fd, const char *message) {
	char buffer[BUFLEN];

	snprintf(buffer, BUFLEN, "m:%s", message);
	TCPSend(fd, buffer, strlen(buffer));
}

/* Listen for messages from the server */
void * messageListener(void * fdp) {
	int fd = *((int*)fdp);
	char buffer[BUFLEN];
	int ret;

	for (;;) {
		memset(buffer, 0, BUFLEN);
		ret = recv(fd, buffer, BUFLEN, 0);

		if (ret < 0) {
			perror("receive");
			exit(1);
		}
		if (ret == 0) {
			puts("error: server offline");
			exit(1);
		}

		puts(buffer);
	}
}

int main(int argc, const char **argv) {
	if (argc < 3) {
		printf("usage: %s <server-ip> <server-port>\n", argv[0]);
		return 127;
	}

	int fd = Sock(argv[1], argv[2], TCP);
	char buffer[BUFLEN];
	int running = 1;
	pthread_t listener;

	/* Start the message listener */
	if (pthread_create(&listener, NULL, messageListener, &fd)) {
		perror("could not create message listener");
		return 127;
	}

	/* Get the username and send to server */
	username(fd);

	while (running) {
		fgets(buffer, BUFLEN, stdin);
		if (buffer[0] == '/') {
			handleCommand(fd, buffer+1, &running);
			continue;
		}
		removeNewline(buffer);
		sendMessage(fd, buffer);
	}

	close(fd);
	pthread_exit(NULL);
}