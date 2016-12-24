#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UserNameList.h"

/* A linked list for storing pairs of user IDs and usenames.
 * A hash table would be a lot better here, of course. */

/* Create a new username list */
UserNameList * new_UserNameList(void) {
	UserNameList *ret = malloc(sizeof(UserNameList));
	ret->next = NULL;
	ret->ID = -1;
	return ret;
}

/* Get rid of one */
void Destroy_UserNameList(UserNameList *list) {
	UserNameList *iter = list;
	UserNameList *temp;

	while (iter) {
		temp = iter->next;
		free(iter);
		iter = temp->next;
	}
}

/* Add a new username to the list, paired with an ID */
void AddUserName(UserNameList *list, int userID, const char *name) {
	UserNameList *iter;

	printf("Adding username \"%s\"\n", name);

	if (list->ID == -1) {
		list->ID = userID;
		strncpy(list->name, name, 13);
		list->next = NULL;
		return;
	}

	for (iter = list; iter->next; iter = iter->next);

	iter->next = malloc(sizeof(UserNameList));
	iter->next->ID = userID;
	strncpy(iter->next->name, name, 13);
	iter->next->next = NULL;
}

/* Remove the user name from the list that is paired with the ID */
void RemoveUserName(UserNameList *list, int userID) {
	UserNameList *iter;
	UserNameList *rem;

	for (iter = list; iter->next != NULL && iter->next->ID != userID;
		iter = iter->next);

	if (!iter->next)
		return;

	rem = iter->next;
	iter->next = iter->next->next;
	free(rem);
}

/* Get the user name of a user that is paired with the ID */
void GetUserName(UserNameList *list, int userID, char *buffer, int size) {
	UserNameList *iter;

	for (iter = list; iter != NULL && iter->ID != userID;
		iter = iter->next);

	if (!iter) {
		buffer[0] = '\0';
		return;
	}

	strncpy(buffer, iter->name, size);
}