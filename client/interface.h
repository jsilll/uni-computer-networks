#ifndef CLIENT_CLIENT_INTERFACE_H
#define CLIENT_CLIENT_INTERFACE_H

/* Input */
const char CMD_REGISTER[] = "reg";
const char CMD_UNREGISTER[] = "unregister";
const char CMD_UNREGISTER_SHORT[] = "unr";
const char CMD_LOGIN[] = "login";
const char CMD_LOGOUT[] = "logout";
const char CMD_SHOW_UID[] = "showuid";
const char CMD_SHOW_UID_SHORT[] = "su";
const char CMD_EXIT[] = "exit";
const char CMD_GROUPS[] = "groups";
const char CMD_GROUPS_SHORT[] = "gl";
const char CMD_SUBSCRIBE[] = "subscribe";
const char CMD_SUBSCRIBE_SHORT[] = "s";
const char CMD_UNSUBSCRIBE[] = "unsubscribe";
const char CMD_UNSUBSCRIBE_SHORT[] = "u";
const char CMD_MY_GROUPS[] = "my_groups";
const char CMD_MY_GROUPS_SHORT[] = "mgl";
const char CMD_SELECT[] = "select";
const char CMD_SELECT_SHORT[] = "sag";
const char CMD_SHOW_GID[] = "showgid";
const char CMD_SHOW_GID_SHORT[] = "sg";
const char CMD_ULIST[] = "ulist";
const char CMD_ULIST_SHORT[] = "ul";
const char CMD_POST[] = "post";
const char CMD_RETRIEVE[] = "retrieve";
const char CMD_RETRIEVE_SHORT[] = "r";

/* Output */
const char MSG_INVALID_IP[] = "Invalid value for ip argument, checking if it's an address\n";
const char MSG_INVALID_PORT[] = "Invalid value for port argument\n";

const char MSG_UNKNOWN_CMD[] = "Unknown command\n";
const char MSG_INVALID_GID[] = "Invalid group ID argument\n";
const char MSG_INVALID_TXT_MSG[] = "Invalid text message argument\n";
const char MSG_INVALID_MID[] = "Invalid message ID argument\n";
const char MSG_INVALID_UID[] = "Invalid user ID argument\n";
const char MSG_INVALID_PASSWD[] = "Invalid password argument\n";
const char MSG_INVALID_GNAME[] = "Invalid group name argument\n";
const char MSG_INVALID_FNAME[] = "Invalid file name argument\n";
const char MSG_INVALID_POST[] = "Invalid post command\n";


#endif //CLIENT_CLIENT_INTERFACE_H
