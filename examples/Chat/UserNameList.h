#ifndef USERNAMELIST_H
#define USERNAMELIST_H

typedef struct _nameIDPair {
	int ID;
	char name[13];
	struct _nameIDPair *next;
} UserNameList;

UserNameList * new_UserNameList(void);
void AddUserName(UserNameList *list, int userID, const char *name);
void RemoveUserName(UserNameList *list, int userID);
void GetUserName(UserNameList *list, int userID, char *buffer, int size);
void Destroy_UserNameList(UserNameList *list);

#endif