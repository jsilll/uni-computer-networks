#ifndef INTERFACE_H_
#define INTERFACE_H_

/* Commands */

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

/* Info Messages */

const char MSG_NOT_IP[] = "Address isn't a valid IP, checking if it's a valid address.\n";

/* Error Messages */

const char ERR_INVALID_PORT[] = "Invalid value for port argument.\n";
const char ERR_UNKNOWN_CMD[] = "Unknown command.\n";
const char ERR_INVALID_POST_CMD[] = "Invalid post command.\n";
const char ERR_INVALID_TXT_MSG[] = "Message exceeds 240 characters.\n";
const char ERR_INVALID_FNAME[] = "Invalid filename argument.\n";
const char ERR_INVALID_GID[] = "Invalid group id.\n";
const char ERR_INVALID_MID[] = "Invalid message id.\n";
const char ERR_INVALID_UID[] = "Invalid user id.\n";
const char ERR_INVALID_PASSWD[] = "Invalid password.\n";

#endif //INTERFACE_H_
