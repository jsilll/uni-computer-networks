#ifndef RC_PROJECT_SERVER_STATE_H_
#define RC_PROJECT_SERVER_STATE_H_

int REG(char *uid, char *pass);
int UNR(char *uid, char *pass);
int LOG(char *uid, char *pass);
int OUT(char *uid, char *pass);
int GLS(char *buffer);
int GSR(char *uid, char *gid, char *gname);
int GUR(char *uid, char *gid);
int GLM(char *uid, char *buffer);
int ULS(char *gid, char *buffer);
FILE *PST(char *uid, char *gid, int tsize, char *text, char *fname, int fsize, char *data, int size_read, char *mid);
int RTV(char *uid, char *gid, char *mid);
FILE *RTVAux(char *gid, int mid, char *buffer);

void WriteFile(FILE *FPtr, char *data, int size_read);
int ReadFile(FILE *FPtr, char *data, int size_read);

#endif //RC_PROJECT_SERVER_STATE_H_
