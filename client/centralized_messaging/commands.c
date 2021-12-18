#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

#include "commands.h"

#define MAX_INPUT_SIZE 128

 bool logged_in = false;
 char *uid, *pass, *group_id;

char command_buffer[MAX_INPUT_SIZE];
struct addrinfo* server_address_udp, *server_address_tcp;

/**
 * @brief Sets up the UDP socket
 *
 * @param ip
 * @param port
 */
void setupServerAddresses(char* ip, char* port)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int errcode;
	if ((errcode = getaddrinfo(ip, port, &hints, &server_address_udp)) != 0)
	{
		fprintf(stderr, "Error on getaddrinfo (udp): %s\n", gai_strerror(errcode));
		return;
	}

	hints.ai_socktype = SOCK_STREAM;
	if ((errcode = getaddrinfo(ip, port, &hints, &server_address_tcp)) != 0)
	{
		fprintf(stderr, "Error on getaddrinfo (tcp): %s\n", gai_strerror(errcode));
		return;
	}
}

/**
 * Frees the server adresses
 */
void freeServerAdresses()
{
	free(server_address_udp);
	free(server_address_tcp);
}


/**
 * @brief Sends a command to the server using UDP protocol
 *
 * @param command
 */
void sendCommandUDP(char* command)
{
	int fd_udp;
	if ((fd_udp = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		fprintf(stderr, "Couldn't send command. Error creating UDP socket.\n");
        return;
	}

	ssize_t n = sendto(fd_udp, command, strlen(command), 0, server_address_udp->ai_addr, server_address_udp->ai_addrlen);
	if (n == -1)
	{
		fprintf(stderr, "Couldn't send command. Error sending command\n");
	}

	close(fd_udp);
}


/**
 * @brief Sends a command to the server using TCP protocol
 *
 * @param command
 */
void sendCommandTCP(char* command) // TODO
{
	int fd_tcp;
	if ((fd_tcp = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "Couldn't send command. Error creating TCP socket.\n");
		return;
	}

	ssize_t n = sendto(fd_tcp, command, strlen(command), 0, server_address_tcp->ai_addr, server_address_tcp->ai_addrlen);
	if (n == -1)
	{
		fprintf(stderr, "Couldn't send command. Error sending command\n");
	}

	close(fd_tcp);
}

/**
 * @brief Following this command the User application sends a
 * message to the DS server, using the UDP protocol, asking to register a new user,
 * sending its identification UID and a selected password pass.
 * The result of the DS registration request should be displayed.
 *
 * @param uid_arg
 * @param pass_arg
 */
void registerUser(char* uid_arg, char* pass_arg)
{
	// Server
	sprintf(command_buffer, "REG %s %s\n", uid_arg, pass_arg);
	sendCommandUDP(command_buffer);
	// (consoante o server) Consultar Local State
	printf("registerUser %s %s\n", uid_arg, pass_arg);
}

/**
 * @brief Following this command the User application sends
 * a message to the DS server, using the UDP protocol, asking to unregister the
 * user with identification UID and password pass. The DS server should
 * unsubscribe this user from all groups in which it was subscribed. The result of
 * the unregister request should be displayed.
 *
 * @param uid_arg
 * @param pass_arg
 */
void unregisterUser(char* uid_arg, char* pass_arg)
{
	sprintf(command_buffer, "UNR %s %s\n", uid_arg, pass_arg);
	sendCommandUDP(command_buffer);
	printf("unregisterUser %s %s\n", uid_arg, pass_arg);
}

/**
 * @brief Following this command the User application sends a
 * message in UDP to the DS to validate the user credentials: UID and pass. The
 * result of the DS validation should be displayed to the user.
 * The User application memorizes the UID in usage.
 *
 * @param uid_arg
 * @param pass_arg
 */
void login(char* uid_arg, char* pass_arg)
{
	sprintf(command_buffer, "LOG %s %s\n", uid_arg, pass_arg);
	sendCommandUDP(command_buffer);
	printf("login %s %s\n", uid_arg, pass_arg);
}

/**
 * @brief Following this command the User application (locally) forgets the credentials of the
 * previously logged in user. A new login command, with different credentials, can
 * then be issued.
 *
 */
void logout()
{
	sprintf(command_buffer, "OUT %s %s\n", uid, pass);
	sendCommandUDP(command_buffer);
	printf("logout\n");
}
/**
 * @brief Following this command the User application locally
 * displays the UID of the user that is logged in.
 */
void showUID()
{
	printf("showUID\n");
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
	sprintf(command_buffer, "GLS\n");
	sendCommandUDP(command_buffer);
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
 * @param gid_arg
 * @param gid_name_arg
 */
void subscribe(char* gid_arg, char* gid_name_arg)
{
	sprintf(command_buffer, "GSR %s %s\n", gid_arg, gid_name_arg);
	sendCommandUDP(command_buffer);
	printf("subscribe %s %s\n", gid_arg, gid_name_arg);
}

/**
 * @brief Following this command the User
 * application sends the DS server a message in UDP, including the user’s UID,
 * asking to unsubscribe group GID. The confirmation of success (or not) should
 * be displayed.
 *
 * @param gid_arg
 */
void unsubscribe(char* gid_arg)
{
	sprintf(command_buffer, "GUR %s %s\n", uid, gid_arg);
	sendCommandUDP(command_buffer);
	printf("unsubscribe %s\n", gid_arg);
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
	sprintf(command_buffer, "GLM %s\n", uid);
	sendCommandUDP(command_buffer);
	printf("my_groups\n");
}

/**
 * @brief Following this command the User application
 * locally memorizes GID as the ID of the active group. Subsequent ulist, post
 * and retrieve messaging commands refer to this GID.
 *
 * @param gid_arg
 */
void selectGroup(char* gid_arg)
{
	sprintf(command_buffer, "\n");
	sendCommandUDP(command_buffer);
	printf("select %s\n", gid_arg);
}

/**
 * @brief Following this command the User application locally
 * displays the GID of the selected group.
 */
void showGID()
{
	sprintf(command_buffer, "\n");
	sendCommandUDP(command_buffer);
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
	sprintf(command_buffer, "\n");
	sendCommandUDP(command_buffer);
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
void post(char* message, char* fname) // TODO file size and data, and trim message
{
	sprintf(command_buffer, "\n");
	sendCommandUDP(command_buffer);
	if (fname == NULL)
	{
		printf("post %s\n", message);
		return;
	}
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
void retrieve(char* mid)
{
	sprintf(command_buffer, "\n");
	sendCommandUDP(command_buffer);
	printf("retrieve %s\n", mid);
}