#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

void setupSocketUDP(char* ip, char* port);

/* Client Commands */
void registerUser(int uid, char* pass);
void unregisterUser(int uid, char* pass);
void login(int uid, char* pass);
void logout();
void showUID();
void exitClient();
void groups();
void subscribe(int gid, char* gid_name);
void unsubscribe(int gid);
void my_groups();
void selectGroup(int gid);
void showGID();
void ulist();
void post(char* message, char* fname); // TODO file size and data
void retrieve(int mid);

#endif //CENTRALIZED_MESSAGING_API_H
