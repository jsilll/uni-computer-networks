#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <libgen.h>

#include "commands.h"

#define MAX_INPUT_SIZE 128

bool logged_in = false;
char uid[6], pass[9], gid[3];

char command_buffer[MAX_INPUT_SIZE], response_buffer[MAX_INPUT_SIZE];
struct addrinfo *server_address_udp, *address_tcp; // TODO

/**
 * @brief Sets up the UDP socket
 *
 * @param ip
 * @param port
 */
int setupServerAddresses(char *ip, char *port)
{
    struct addrinfo hints;
    int errcode;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((errcode = getaddrinfo(ip, port, &hints, &server_address_udp)) != 0)
    {
        fprintf(stderr, "Error on getaddrinfo (udp): %s\n", gai_strerror(errcode));
        return -1;
    }

    hints.ai_socktype = SOCK_STREAM;
    if ((errcode = getaddrinfo(ip, port, &hints, &address_tcp)) != 0)
    {
        fprintf(stderr, "Error on getaddrinfo (udp): %s\n", gai_strerror(errcode));
        return -1;
    }

    return 0;
}

/**
 * Frees the server adresses
 */
void freeServerAddress()
{
    free(server_address_udp);
    free(address_tcp);
}

/**
 * @brief Sends a command_buffer to the server using UDP protocol
 *
 * @param command_buffer
 */
int sendCommandUDP()
{
    int fd;

    bzero(response_buffer, MAX_INPUT_SIZE);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Couldn't send command_buffer. Error creating UDP socket.\n");
        return -1;
    }

    if (sendto(fd, command_buffer, strlen(command_buffer), 0, server_address_udp->ai_addr, server_address_udp->ai_addrlen) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
        return -1;
    }

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (recvfrom(fd, response_buffer, MAX_INPUT_SIZE, 0, (struct sockaddr *)&addr, &addrlen) == -1)
    {
        close(fd);
        fprintf(stderr, "Error receiving server's response.\n");
        return -1;
    }

    close(fd);
    return 0;
}

/**
 * @brief Sends a command_buffer to the server using TCP protocol
 *
 * @param command_buffer
 * @param response_buffer
 */
int sendCommandTCP()
{
    int fd;

    bzero(response_buffer, MAX_INPUT_SIZE);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Couldn't send command_buffer. Error creating TCP socket.\n");
        return -1;
    }

    if (connect(fd, address_tcp->ai_addr, address_tcp->ai_addrlen) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error establishing a connection with server.\n");
        return -1;
    }

    if (write(fd, command_buffer, strlen(command_buffer)) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
        return -1;
    }

    if (read(fd, response_buffer, MAX_INPUT_SIZE) == -1)
    {
        close(fd);
        fprintf(stderr, "Error receiving server's response.\n");
        return -1;
    }

    close(fd);
    return 0;
}

int sendFileTCP()
{
    int fd;

    bzero(response_buffer, MAX_INPUT_SIZE);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Couldn't send command_buffer. Error creating TCP socket.\n");
        return -1;
    }

    if (connect(fd, address_tcp->ai_addr, address_tcp->ai_addrlen) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error establishing a connection with server.\n");
        return -1;
    }

    if (write(fd, command_buffer, strlen(command_buffer)) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Following this command the User application sends a
 * message to the DS server, using the UDP protocol, asking to
 * register a new user, sending its identification UID and a
 * selected password pass. The result of the DS registration request
 * should be displayed.
 *
 * @param uid
 * @param pass
 */
int REG(char *uid_arg, char *pass_arg)
{
    sprintf(command_buffer, "REG %s %s\n", uid_arg, pass_arg);
    sendCommandUDP();
    printf("%s", response_buffer);
}

/**
 * @brief Following this command the User application sends
 * a message to the DS server, using the UDP protocol, asking to unregister the
 * user with identification UID and password pass. The DS server should
 * GUR this user from all GLS in which it was subscribed. The result of
 * the unregister request should be displayed.
 *
 * @param uid_arg
 * @param pass_arg
 */
void UNR(char *uid_arg, char *pass_arg)
{
    sprintf(command_buffer, "UNR %s %s\n", uid_arg, pass_arg);
    sendCommandUDP();
    printf("%s", response_buffer);
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
void LOG(char *uid_arg, char *pass_arg)
{
    sprintf(command_buffer, "LOG %s %s\n", uid_arg, pass_arg);
    sendCommandUDP();
    printf("%s", response_buffer);
    if (!strcmp(response_buffer, "RLO OK\n"))
    {
        logged_in = true;
        strcpy(uid, uid_arg);
        strcpy(pass, pass_arg);
    }
}

/**
 * @brief Following this command the User application (locally) forgets the credentials of the
 * previously logged in user. A new LOG command, with different credentials, can
 * then be issued.
 *
 */
void OUT()
{
    if (logged_in)
    {
        sprintf(command_buffer, "OUT %s %s\n", uid, pass);
        sendCommandUDP();
        printf("%s", response_buffer);
        if (!strcmp(response_buffer, "ROU OK\n"))
        {
            logged_in = false;
            strcpy(uid, "");
            strcpy(pass, "");
        }
    }
    else
    {
        fprintf(stderr, "User needs to be logged in\n");
    }
}
/**
 * @brief Following this command the User application locally
 * displays the UID of the user that is logged in.
 */
void showUID()
{
    if (logged_in)
    {
        printf("UID: %s\n", uid);
    }
    else
    {
        fprintf(stderr, "You are not logged in\n");
    }
}

/**
 * @brief Following this command the User application terminates, after making sure that all TCP
 * connections are closed.
 */
void exitClient()
{
    printf("(local) exit\n");
}

/**
 * @brief Following this command the User application sends the DS
 * server a message in UDP asking for the list of available GLS. The reply
 * should be displayed as a list of group IDs (GID) and names (GName).
 *
 */
void GLS()
{
    sprintf(command_buffer, "GLS\n");
    sendCommandUDP();
    printf("%s", response_buffer);
}

/**
 * @brief Following this command the
 * User application sends the DS server a message in UDP, including the user’s
 * UID, asking to GSR the desired group, identified by its GID and GName. If
 * GID = 0 this corresponds to a request to create and GSR to a new group
 * named GName. The confirmation of successful subscription (or not) should be
 * displayed.
 *
 * @param gid_arg
 * @param gid_name_arg
 */
void GSR(char *gid_arg, char *gid_name_arg)
{
    if (logged_in)
    {
        sprintf(command_buffer, "GSR %s %s %s\n", uid, gid_arg, gid_name_arg);
        sendCommandUDP();
        printf("%s", response_buffer);
    }
    else
    {
        fprintf(stderr, "User needs to be logged in\n");
    }
}

/**
 * @brief Following this command the User
 * application sends the DS server a message in UDP, including the user’s UID,
 * asking to GUR group GID. The confirmation of success (or not) should
 * be displayed.
 *
 * @param gid_arg
 */
void GUR(char *gid_arg)
{
    if (logged_in)
    {
        sprintf(command_buffer, "GUR %s %s\n", uid, gid_arg);
        sendCommandUDP();
        printf("%s", response_buffer);
    }
    else
    {
        fprintf(stderr, "User need to be logged in\n");
    }
}

/**
 * @brief Following this command the User application sends
 * the DS server a message in UDP, including the user’s UID, asking the list of
 * GLS to which this user has already subscribed. The reply should be displayed
 * as a list of group IDs and names.
 *
 */
void GLM()
{
    sprintf(command_buffer, "GLM %s\n", uid);
    sendCommandUDP();
    printf("%s", response_buffer);
}

/**
 * @brief Following this command the User application
 * locally memorizes GID as the ID of the active group. Subsequent ULS, PST
 * and RTV messaging commands refer to this GID.
 *
 * @param gid_arg
 */
void selectGroup(char *gid_arg)
{
    strcpy(gid, gid_arg);
    printf("(local) select %s\n", gid_arg);
}

/**
 * @brief Following this command the User application locally
 * displays the GID of the selected group.
 */
void showGID()
{
    printf("(local) %s\n", gid);
}

/**
 * @brief Following this command the User application sends the DS
 * server a message in TCP asking for the list of user UIDs that are subscribed to
 * the currently subscribed group GID.
 *
 */
void ULS()
{
    sprintf(command_buffer, "ULS %s\n", gid);
    sendCommandTCP();
    printf("%s", response_buffer);
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
void PST(char *message, char *fname) /* TODO size não pode exceder tamanho */
{
    if (logged_in)
    {
        if (fname != NULL)
        {
            FILE *FPtr = fopen(fname, "rb");
            fseek(FPtr, 0L, SEEK_END);
            long size = ftell(FPtr);
            rewind(FPtr);
            sprintf(command_buffer, "PST %s %s %lu %s %s %lu ", uid, gid, strlen(message), message, basename(fname), size);

            int fd;
            bzero(response_buffer, MAX_INPUT_SIZE);

            if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                fprintf(stderr, "Couldn't send command_buffer. Error creating TCP socket.\n");
                return;
            }

            if (connect(fd, address_tcp->ai_addr, address_tcp->ai_addrlen) == -1)
            {
                close(fd);
                fprintf(stderr, "Couldn't send command_buffer. Error establishing a connection with server.\n");
                return;
            }

            if (write(fd, command_buffer, strlen(command_buffer)) == -1)
            {
                close(fd);
                fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
                return;
            }

            int n;
            char data[1024];
            bzero(data, 1024);
            int bytes_read;
            while ((bytes_read = fread(data, 1, 1024, FPtr)) > 0)
            {
                if (write(fd, data, bytes_read) == -1)
                {
                    close(fd);
                    fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
                    return;
                }
                bzero(data, 1024);
            }
            fclose(FPtr);
            if (read(fd, response_buffer, MAX_INPUT_SIZE) == -1)
            {
                close(fd);
                fprintf(stderr, "Error receiving server's response.\n");
                return;
            }
            close(fd);
            printf("%s", response_buffer);
        }
        else
        {
            sprintf(command_buffer, "PST %s %s %lu %s\n", uid, gid, strlen(message), message);
            sendCommandTCP();
            printf("%s", response_buffer);
        }
    }
    else
    {
        fprintf(stderr, "User need to be logged in\n");
    }
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
void RTV(char *mid)
{
    sprintf(command_buffer, "RTV %s %s %s\n", uid, gid, mid);

    int fd;

    bzero(response_buffer, MAX_INPUT_SIZE);
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Couldn't send command_buffer. Error creating TCP socket.\n");
        return;
    }

    if (connect(fd, address_tcp->ai_addr, address_tcp->ai_addrlen) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error establishing a connection with server.\n");
        return;
    }

    if (write(fd, command_buffer, strlen(command_buffer)) == -1)
    {
        close(fd);
        fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
        return;
    }

    int n;
    if ((n = read(fd, response_buffer, 7)) == -1)
    {
        close(fd);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }
    else
    {
        char op[4], status[4];
        sscanf(response_buffer, "%s %s", op, status);

        if (!strcmp(status, 'OK'))
        {
            char data[1024];
            bzero(data, 1024);

            FILE *TempFile = fopen("temp.bin", "wb+");
            while ((n = read(fd, data, 1024)) > 0)
            {
                fwrite(data, sizeof(char), n, TempFile);
            }
            rewind(TempFile);

            char n_msg[3];
            fscanf(TempFile, "%s ", n_msg);
            int num_msg = atoi(n_msg);

            for (int i = 0; i < num_msg; i++)
            {
                char mid[5], uid[6], tsize[4];
                fscanf(TempFile, "%s %s %s", mid, uid, tsize);
                char path_buffer[256];
                sprintf(path_buffer, "GROUP%s_MSG%s.txt", gid, mid);
                FILE *MessageFile = fopen(path_buffer, "w");
                bzero(data, 1024);
                fgetc(TempFile);
                fread(data, sizeof(char), atoi(tsize), TempFile);
                fwrite(data, sizeof(char), atoi(tsize), MessageFile);
                fclose(MessageFile);

                fgetc(TempFile);
                char c = fgetc(TempFile);
                if (c == '/')
                {
                    char fname[25], fsize[11];
                    fgetc(TempFile);
                    fscanf(TempFile, "%s %s", fname, fsize);
                }
                else
                {
                    fseek(TempFile, -1L, SEEK_CUR);
                }
                bzero(data, 1024);
            }
        }
        else
        {
            printf(response_buffer);
        }
    }

    close(fd);
}