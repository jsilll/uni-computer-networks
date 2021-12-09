#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

void setupSocketUDP(char* ip, char* port);

/* Client Commands */
void registerUser(int uid, char* pass);
void unregisterUser(int uid, char* pass);
void login(int uid, char* pass); // TODO should be here?
void logout(); // TODO should be here?
void showUID(); // TODO should be here?
void exitClient(); // TODO should be here?
void groups();
void subscribe(int gid, char* gid_name);
void unsubscribe(int gid);
void my_groups();
void selectGroup(int gid); // TODO should be here?
void showGID(); // TODO should be here?
void ulist();
void post(char* message, char* fname);
void retrieve(int mid);

#endif //CENTRALIZED_MESSAGING_API_H
