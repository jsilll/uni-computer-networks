#ifndef OPERATIONS_H
#define OPERATIONS_H

/* Operation Related Functions */

int registerUser(char *uid, char *pass);
int unregisterUser(char *uid, char *pass);
int login(char *uid, char *pass);
int logout(char *uid, char *pass);
int groups(char *buffer);
int subscribe(char *uid, char *gid, char *gname);
int unsubscribe(char *uid, char *gid);
int myGroups(char *uid, char *buffer);
DIR *ulist(char *gid);
void ulsGetGName(char *gid, char *buffer);
void ulsAppendUser(struct dirent *de, char *buffer);
FILE *post(char *uid, char *gid, char *text, char *fname, char *mid);
int retrieve(char *uid, char *gid, char *mid);
FILE *retrieveAux(char *gid, int mid, char *buffer);

/* Auxiliar Functions */

int readFile(FILE *FPtr, char *data, int size_read);
void writeToFile(FILE *FPtr, char *data, int size_read);

#endif // OPERATIONS_H
