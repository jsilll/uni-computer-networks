#ifndef CLIENT_CLIENT_INTERFACE_H
#define CLIENT_CLIENT_INTERFACE_H

/* Input */
static const char CMD_REGISTER[] = "reg";
static const char CMD_UNREGISTER[] = "unregister";
static const char CMD_UNREGISTER_SHORT[] = "unr";
static const char CMD_LOGIN[] = "login";
static const char CMD_LOGOUT[] = "logout";
static const char CMD_EXIT[] = "exit";
static const char CMD_GROUPS[] = "groups";
static const char CMD_GROUPS_SHORT[] = "gl";
static const char CMD_SUBSCRIBE[] = "subscribe";
static const char CMD_SUBSCRIBE_SHORT[] = "s";
static const char CMD_UNSUBSCRIBE[] = "unsubscribe";
static const char CMD_UNSUBSCRIBE_SHORT[] = "u";
static const char CMD_MY_GROUPS[] = "my_groups";
static const char CMD_MY_GROUPS_SHORT[] = "mgl";
static const char CMD_SELECT[] = "select";
static const char CMD_SELECT_SHORT[] = "sag";
static const char CMD_ULIST[] = "ulist";
static const char CMD_ULIST_SHORT[] = "ul";
static const char CMD_POST[] = "post";
static const char CMD_RETRIEVE[] = "retrieve";
static const char CMD_RETRIEVE_SHORT[] = "r";

/* Output */
static const char MSG_UNKNOWN_CMD[] = "Unknown command\n";
static const char MSG_INVALID_GID[] = "Invalid GID argument\n";
static const char MSG_INVALID_TXT_MSG[] = "Invalid text message argument\n";
static const char MSG_INVALID_MID[] = "Invalid MID argument\n";
static const char MSG_INVALID_UID[] = "Invalid UID argument\n";
static const char MSG_INVALID_PASSWD[] = "Invalid password argument\n";
static const char MSG_INVALID_GNAME[] = "Invalid GName argument\n";
static const char MSG_INVALID_FNAME[] = "Invalid Fname argument\n";

#endif //CLIENT_CLIENT_INTERFACE_H
