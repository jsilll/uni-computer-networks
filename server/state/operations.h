#ifndef RC_PROJECT_SERVER_STATE_H_
#define RC_PROJECT_SERVER_STATE_H_

#ifdef __cplusplus
extern "C"
#endif
int REG(char *uid, char *pass);

#ifdef __cplusplus
extern "C"
#endif
void UNR(char *uid, char *pass);

#ifdef __cplusplus
extern "C"
#endif
void LOG(char *uid, char *pass);

#ifdef __cplusplus
extern "C"
#endif
void OUT(char *uid, char *pass);

#ifdef __cplusplus
extern "C"
#endif
void GLS();

#ifdef __cplusplus
extern "C"
#endif
void GSR(char *uid, char *gid, char *gname);

#ifdef __cplusplus
extern "C"
#endif
void GUR(char *uid, char *gid);

#ifdef __cplusplus
extern "C"
#endif
void GLM(char *uid);

#ifdef __cplusplus
extern "C"
#endif
void ULS(char *gid);

#ifdef __cplusplus
extern "C"
#endif
void PST(char *uid, char *gid, int tsize, char *text, char *fname, int fsize, char *data);

#ifdef __cplusplus
extern "C"
#endif
void RTV(char *uid, char *gid, char *mid);

#endif //RC_PROJECT_SERVER_STATE_H_
