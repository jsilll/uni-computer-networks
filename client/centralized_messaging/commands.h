#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

/* Server Addresses (UDP and TCP) */
int setupServerAddresses(char *ip, char *port);
void freeServerAddress();

/* Local commands */
void showUID();
void exitClient();
void selectGroup(char *gid_arg);
void showGID();

/* Client-Server Commands */
void registerUser(char *uid_arg, char *pass_arg);
void unregisterUser(char *uid_arg, char *pass_arg);
void login(char *uid_arg, char *pass_arg);
void logout();
void groups();
void subscribe(char *gid_arg, char *gid_name_arg);
void unsubscribe(char *gid_arg);
void myGroups();
void ulist();
void post(char *message, char *fname);
void retrieve(char *mid);

#endif //CENTRALIZED_MESSAGING_API_H
