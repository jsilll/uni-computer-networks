/* centralized_messaging_api.h */
#ifndef CENTRALIZED_MESSAGING_API_H
#define CENTRALIZED_MESSAGING_API_H

#include <stdio.h>

/**
 * @brief Following this command the User application sends a
 * message to the DS server, using the UDP protocol, asking to register a new user,
 * sending its identification UID and a selected password pass.
 * The result of the DS registration request should be displayed.
 * 
 * @param UID 
 * @param pass 
 */
void registerUser(int UID, char *pass)
{
    printf("registerUser\n");
}

/**
 * @brief Following this command the User application sends
 * a message to the DS server, using the UDP protocol, asking to unregister the
 * user with identification UID and password pass. The DS server should
 * unsubscribe this user from all groups in which it was subscribed. The result of
 * the unregister request should be displayed.
 * 
 * @param UID 
 * @param pass 
 */
void unregisterUser(int UID, char *pass)
{
    printf("unrgesterUser\n");
}

/**
 * @brief Following this command the User application sends a
 * message in UDP to the DS to validate the user credentials: UID and pass. The
 * result of the DS validation should be displayed to the user.
 * The User application memorizes the UID in usage.
 * 
 * @param UID 
 * @param pass 
 */
void login(int UID, char *pass)
{
    printf("login\n");
}

/**
 * @brief Following this command the User application (locally) forgets the credentials of the
 * previously logged in user. A new login command, with different credentials, can
 * then be issued.
 * 
 */
void logout()
{
    printf("logout\n");
}

/**
 * @brief Following this command the User application terminates, after making that all TCP
 * connections are closed.
 */
void exitClient()
{
    printf("exit\n");
}

/**
 * @brief Following this command the User application sends the DS
 * server a message in UDP asking for the list of available groups. The reply
 * should be displayed as a list of group IDs (GID) and names (GName).
 * 
 */
void groups()
{
    printf("groups\n");
}

/**
 * @brief Following this command the
 * User application sends the DS server a message in UDP, including the user’s
 * UID, asking to subscribe the desired group, identified by its GID and GName. If
 * GID = 0 this corresponds to a request to create and subscribe to a new group
 * named GName. The confirmation of successful subscription (or not) should be
 * displayed.
 * 
 * @param GID 
 * @param GIDName 
 */
void subscribe(int GID, char *GIDName)
{
    printf("subscribe\n");
}

/**
 * @brief Following this command the User
 * application sends the DS server a message in UDP, including the user’s UID,
 * asking to unsubscribe group GID. The confirmation of success (or not) should
 * be displayed.
 * 
 * @param GID 
 */
void unsubscribe(int GID)
{
    printf("unsubscribe\n");
}

/**
 * @brief Following this command the User application sends
 * the DS server a message in UDP, including the user’s UID, asking the list of
 * groups to which this user has already subscribed. The reply should be displayed
 * as a list of group IDs and names.
 * 
 */
void my_groups()
{
    printf("my_groups\n");
}

/**
 * @brief Following this command the User application
 * locally memorizes GID as the ID of the active group. Subsequent ulist, post
 * and retrieve messaging commands refer to this GID.
 * 
 * @param GID 
 */
void selectGroup(int GID)
{
    printf("select\n");
}

/**
 * @brief Following this command the User application sends the DS
 * server a message in TCP asking for the list of user UIDs that are subscribed to
 * the currently subscribed group GID.
 * 
 */
void ulist()
{
    printf("ulist\n");
}

/**
 * @brief Following this command the User establishes a
 * TCP session with the DS server and sends a message containing text (between
 * “ “), and possibly also a file with name Fname.
 * The confirmation of success (or not) should be displayed, including the posted
 * message’s ID MID. The TCP connection is then closed.
 * 
 * @param message 
 * @param fname 
 */
void post(char *message, char *fname) // TODO file size and data
{
    printf("post\n");
}

/**
 * @brief Following this command the User establishes a
 * TCP session with the DS server and sends a message asking to receive up to 20
 * unread messages, starting with the one with identifier MID, for the active group
 * GID. The DS server only sends messages that include at least an author UID and
 * text – any incomplete messages are omitted.
 * After receiving the messages, the User application sends the DS a confirmation
 * and then closes the TCP session. The reply should be displayed as a numbered
 * list of text messages and, if available, the associated filenames and respective
 * sizes.
 * 
 * @param MID 
 */
void retrieve(int MID)
{
    printf("retrieve\n");
}

#endif /* centralized_messaging_api.h */