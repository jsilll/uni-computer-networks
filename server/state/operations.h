#ifndef RC_PROJECT_SERVER_STATE_H_
#define RC_PROJECT_SERVER_STATE_H_

int REG(char *uid, char *pass);
int UNR(char *uid, char *pass);
int LOG(char *uid, char *pass);
int OUT(char *uid, char *pass);
int GLS(char *buffer);
int GSR(char *uid, char *gid, char *gname);
int GUR(char *uid, char *gid);
int GLM(char *uid);
int ULS(char *gid);
int PST(char *uid, char *gid, int tsize, char *text, char *fname, int fsize, char *data);
int RTV(char *uid, char *gid, char *mid);

#endif //RC_PROJECT_SERVER_STATE_H_
