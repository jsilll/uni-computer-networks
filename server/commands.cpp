#include <iostream>
#include <vector>
#include <map>

#include "commands.h"

using namespace std;

class User;
class Message;
class Group;

class User {
 public:
  string _id, _password;
  vector<Group> _groups;
  User(string id, string password);
};

User::User(string id, string password) {
  _id = id;
  _password = password;
  _groups = vector<Group>();
}

class Message {
  string _id, _text, _author_id;
 public:
  Message(string id, string text, string author_id);
};

Message::Message(string id, string text, string author_id) {
  _id = id;
  _text = text;
  _author_id = author_id;
}

class Group {
  string _id, _name;
  vector<Message> messages;
 public:
  Group(string id, string name);
};

Group::Group(string id, string name) {
  _id = id;
  _name = name;
  messages = vector<Message>();
}

map<string, User> users = map<string, User>();
vector<Group> groups = vector<Group>();

extern "C" int REG(char *uid, char *pass) {
  if (users.find(string(uid)) != users.end()) return -1;
  users.emplace(string(uid), User(string(uid), string(pass)));
  return 0;
}

extern "C" void UNR(char *uid, char *pass) {
  cout << "UNR" << endl;
}

extern "C" void LOG(char *uid, char *pass) {
  cout << "LOG" << endl;
}

extern "C" void OUT(char *uid, char *pass) {
  cout << "OUT" << endl;
}

extern "C" void GLS() {
  cout << "GLS" << endl;
}

extern "C" void GSR(char *uid, char *gid, char *gname) {
  cout << "GSR" << endl;
}

extern "C" void GUR(char *uid, char *gid) {
  cout << "GUR" << endl;
}

extern "C" void GLM(char *uid) {
  cout << "GLM" << endl;
}

extern "C" void ULS(char *gid) {
  cout << "ULS" << endl;
}

extern "C" void PST(char *uid, char *gid, int tsize, char *text, char *fname, int fsize, char *data) {
  cout << "PST" << endl;
}

extern "C" void RTV(char *uid, char *gid, char *mid) {
  cout << "RTV" << endl;
}