#include "TCPServer.h"
#include "NetUtil.h"

/* A simple anonymous chat server
 * build with: $ gcc template.c TCPServer.c NetUtil.c
 * Connect with: $ telnet 127.0.0.1 2674
 */

/* Welcome the user when they connect */
int onConnect(UserSet users, int userID, void *parameters) {
	const char *hw = "server: Hello, world!\n";

	TCPSend(userID, hw, strlen(hw));
	return TCP_SUCCESS;
}

/* Do nothing on disconnect */
int onDisconnect(UserSet users, int userID, void *parameters) {
	return TCP_SUCCESS;
}

/* Send message out to all users except sending user */
int onReceive(UserSet users, int userID, const char *message, int len, void *parameters) {
	int i;
	for_each_user(i, users) {
		if (i != userID) {
			TCPSend(i, message, len);
		}
	}
	return TCP_SUCCESS;
}

int main(int argc, char **argv) {
	TCPServerController *tsc = MakeTCPServerController(
		onConnect,
		onDisconnect,
		onReceive
	);

	RunTCPServer(tsc, "2674", NULL);
	return 0;
}
