#include <stdio.h>
#include <string.h>
#include "../../src/TCPServer.h"
#include "../../src/NetUtil.h"
#include "UserNameList.h"

#define BUFLEN 1024

/* Send a welcome message to a user after they've connected with a name,
 * and send all other users on the server that that users has joined
 */
void SendWelcomeMessage(UserSet users, int userID, UserNameList *names) {
	UserNameList *iter;
	char buffer[BUFLEN];
	char namebuffer[BUFLEN];
	int i;

	/* Get the username of the new user */
	GetUserName(names, userID, namebuffer, BUFLEN);
	/* Create the welcome message for the new user */
	snprintf(buffer, BUFLEN, "Welcome to the chat, %s!\nUsers here:\n", namebuffer);
	for (iter = names; iter; iter = iter->next) {
		snprintf(namebuffer, BUFLEN, "\t%s\n", iter->name);
		strncat(buffer, namebuffer, BUFLEN);
	}
	/* Send the welcome message */
	TCPSend(userID, buffer, strlen(buffer));

	/* Alert all other users that the new user has joined */
	GetUserName(names, userID, namebuffer, BUFLEN);
	snprintf(buffer, BUFLEN, "%s joined the chat.\n", namebuffer);
	for_each_user(i, users) {
		if (i != userID) {
			TCPSend(i, buffer, strlen(buffer));
		}
	}
}

/* Send a message that the user has sent to all other usesrs */
void SendUserMessage(UserSet users,
					int userID,
					UserNameList *names,
					const char *message) {
	int i;
	char namebuffer[BUFLEN];
	char buffer[BUFLEN];

	/* Get the username of the sending user */
	GetUserName(names, userID, namebuffer, BUFLEN);

	/* Format the message */
	snprintf(buffer, BUFLEN, "%s: ", namebuffer);
	strncat(buffer, message, BUFLEN);

	/* Send to all other users */
	for_each_user(i, users) {
		if (i != userID) {
			TCPSend(i, buffer, strlen(buffer));
		}
	}
}

/* Send a goodbye message to the leaving user and alert other users that
 * this user has left */
void SendGoodByeMessage(UserSet users, int userID, UserNameList *names) {
	char buffer[BUFLEN];
	char namebuffer[BUFLEN];
	int i;

	GetUserName(names, userID, namebuffer, BUFLEN);
	snprintf(buffer, BUFLEN, "%s left the chat.\n", namebuffer);

	for_each_user(i, users) {
		if (i != userID) {
			TCPSend(i, buffer, strlen(buffer));
		}
	}

	TCPSend(userID, "Goodbye!", 9);
}

/* Do nothing special when a user connects */
int onConnect(UserSet users, int userID, void *parameters) {
	return TCP_SUCCESS;
}

/* on disconnect, remove the user's name from the usernames list */
int onDisconnect(UserSet users, int userID, void *parameters) {
	RemoveUserName(parameters, userID);

	return TCP_SUCCESS;
}

int onReceive(UserSet users,
			  int userID,
			  const char *message,
			  int len,
			  void *parameters) {
	int i;
	char buffer[BUFLEN];
	
	switch(message[0]) {
	case 'l': // login 
		AddUserName(parameters, userID, message + 2);
		SendWelcomeMessage(users, userID, parameters);
		break;
	case 'm': // message
		SendUserMessage(users, userID, parameters, message + 2);
		break;
	case 'e': // user exit
		SendGoodByeMessage(users, userID, parameters);
		RemoveUserName(parameters, userID);
		disconnect_user(userID, users);
		break;
	}

	return TCP_SUCCESS;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s <port>\n", argv[0]);
		return 127;
	}

	UserNameList *usernamelist = new_UserNameList();

	TCPServerController *chatController = MakeTCPServerController(
		onConnect, onDisconnect, onReceive
	);

	RunTCPServer(chatController, argv[1], usernamelist);
}