#ifndef RC_PROJECT_SERVER_STATE_H_
#define RC_PROJECT_SERVER_STATE_H_

int registerUser(char *uid, char *pass);
int unregisterUser(char *uid, char *pass);
int login(char *uid, char *pass);
int logout(char *uid, char *pass);
int groups(char *buffer);
int subscribe(char *uid, char *gid, char *gname);
int unsubscribe(char *uid, char *gid);
int myGroups(char *uid, char *buffer);
DIR *ulist(char *gid);
void ulsAux(struct dirent *de, char *buffer);
FILE *post(char *uid, char *gid, char *text, char *fname, char *mid);
int retrieve(char *uid, char *gid, char *mid);
FILE *retrieveAux(char *gid, int mid, char *buffer);

void getGName(char *gid, char *buffer);
int ReadFile(FILE *FPtr, char *data, int size_read);
void WriteToFile(FILE *FPtr, char *data, int size_read);

#endif // RC_PROJECT_SERVER_STATE_H_
