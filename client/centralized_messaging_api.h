#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

void setupConnection(char* IP, char* PORT);

void registerUser(int UID, char* pass);
void unregisterUser(int UID, char* pass);
void login(int UID, char* pass);
void logout();
void showUID();
void exitClient();
void groups();
void subscribe(int GID, char* GIDName);
void unsubscribe(int GID);
void my_groups();
void selectGroup(int GID);
void showGID();
void ulist();
void post(char* message, char* fname); // TODO file size and data
void retrieve(int MID);

#endif; //CENTRALIZED_MESSAGING_API_H
