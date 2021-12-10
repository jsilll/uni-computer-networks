#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

/* Server Addresses (UDP and TCP) */
void setupServerAddresses(char* ip, char* port);
void freeServerAdresses();

/* Client Commands */
void registerUser(char* uid_arg, char* pass_arg);
void unregisterUser(char* uid_arg, char* pass_arg);
void login(char* uid_arg, char* pass_arg);
void logout();
void showUID();
void exitClient(); // TODO should be here?
void groups();
void subscribe(char* gid_arg, char* gid_name_arg);
void unsubscribe(char* gid_arg);
void my_groups();
void selectGroup(char* gid_arg);
void showGID();
void ulist();
void post(char* message, char* fname);
void retrieve(char* mid);

#endif //CENTRALIZED_MESSAGING_API_H
