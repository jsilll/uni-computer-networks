#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

/* Server Addresses (UDP and TCP) */
int setupServerAddresses(char *ip, char *port);
void freeServerAddress();

/* Client local commands */
void showUID();
void exitClient();
void selectGroup(char *gid_arg);
void showGID();

/* Client Commands */
void REG(char *uid_arg, char *pass_arg);
void UNR(char *uid_arg, char *pass_arg);
void LOG(char *uid_arg, char *pass_arg);
void OUT();
void GLS();
void GSR(char *gid_arg, char *gid_name_arg);
void GUR(char *gid_arg);
void GLM();
void ULS();
void PST(char *message, char *fname);
void RTV(char *mid);

#endif //CENTRALIZED_MESSAGING_API_H
