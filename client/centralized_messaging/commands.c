#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "commands.h"

int fd_udp;
struct addrinfo* server_address;

/**
 * @brief Sets up the connection
 *
 * @param ip
 * @param port
 */
void setupSocketUDP(char* ip, char* port)
{
	if ((fd_udp = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		fprintf(stderr, "Error creating UDP socket.\n");
		exit(EXIT_FAILURE);
	}

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int errcode;
	if ((errcode = getaddrinfo(ip, port, &hints, &server_address)) != 0)
	{
		fprintf(stderr, "Error on getaddrinfo: %s\n", gai_strerror(errcode));
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Sends a command to the server using UDP protocol
 *
 * @param command
 */
void sendCommandUDP(char* command)
{
	ssize_t n = sendto(fd_udp, command, strlen(command), 0, server_address->ai_addr, server_address->ai_addrlen);
	if (n == -1)
	{
		fprintf(stderr, "Some error occurred sending command\n");
		exit(1);
	}
}

/**
 * @brief Following this command the User application sends a
 * message to the DS server, using the UDP protocol, asking to register a new user,
 * sending its identification UID and a selected password pass.
 * The result of the DS registration request should be displayed.
 *
 * @param uid
 * @param pass
 */
void registerUser(int uid, char* pass)
{
	sendCommandUDP("registerUser\n");
	printf("registerUser %d %s\n", uid, pass);
}

/**
 * @brief Following this command the User application sends
 * a message to the DS server, using the UDP protocol, asking to unregister the
 * user with identification UID and password pass. The DS server should
 * unsubscribe this user from all groups in which it was subscribed. The result of
 * the unregister request should be displayed.
 *
 * @param uid
 * @param pass
 */
void unregisterUser(int uid, char* pass)
{
	sendCommandUDP("unregisterUser\n");
	printf("unregisterUser %d %s\n", uid, pass);
}

/**
 * @brief Following this command the User application sends a
 * message in UDP to the DS to validate the user credentials: UID and pass. The
 * result of the DS validation should be displayed to the user.
 * The User application memorizes the UID in usage.
 *
 * @param uid
 * @param pass
 */
void login(int uid, char* pass)
{
	sendCommandUDP("login\n");
	printf("login %d %s\n", uid, pass);
}

/**
 * @brief Following this command the User application (locally) forgets the credentials of the
 * previously logged in user. A new login command, with different credentials, can
 * then be issued.
 *
 */
void logout()
{
	sendCommandUDP("logout\n");
	printf("logout\n");
}
/**
 * @brief Following this command the User application locally
 * displays the UID of the user that is logged in.
 */
void showUID()
{
	sendCommandUDP("showUID\n");
	printf("showUID\n");
}

/**
 * @brief Following this command the User application terminates, after making that all TCP
 * connections are closed.
 */
void exitClient()
{
	sendCommandUDP("exitClient\n");
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
	sendCommandUDP("groups\n");
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
 * @param gid
 * @param gid_name
 */
void subscribe(int gid, char* gid_name)
{
	sendCommandUDP("subscribe\n");
	printf("subscribe %d %s\n", gid, gid_name);
}

/**
 * @brief Following this command the User
 * application sends the DS server a message in UDP, including the user’s UID,
 * asking to unsubscribe group GID. The confirmation of success (or not) should
 * be displayed.
 *
 * @param gid
 */
void unsubscribe(int gid)
{
	sendCommandUDP("unsubscribe\n");
	printf("unsubscribe %d\n", gid);
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
	sendCommandUDP("my_groups\n");
	printf("my_groups\n");
}

/**
 * @brief Following this command the User application
 * locally memorizes GID as the ID of the active group. Subsequent ulist, post
 * and retrieve messaging commands refer to this GID.
 *
 * @param gid
 */
void selectGroup(int gid)
{
	sendCommandUDP("selectGroup\n");
	printf("select %d\n", gid);
}

/**
 * @brief Following this command the User application locally
 * displays the GID of the selected group.
 */
void showGID()
{
	sendCommandUDP("showGID\n");
	printf("showGID\n");
}

/**
 * @brief Following this command the User application sends the DS
 * server a message in TCP asking for the list of user UIDs that are subscribed to
 * the currently subscribed group GID.
 *
 */
void ulist()
{
	sendCommandUDP("ulist\n");
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
void post(char* message, char* fname) // TODO file size and data
{
	sendCommandUDP("post\n");
	printf("post %s %s\n", message, fname);
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
 * @param mid
 */
void retrieve(int mid)
{
	sendCommandUDP("retrieve\n");
	printf("retrieve %d\n", mid);
}