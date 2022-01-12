#include <errno.h>
#include <libgen.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "commands.h"
// TODO interface.h?

bool LOGGED_IN = false, GROUP_SELECTED = false;
int UID = 0, GID = 0;
char PASSWORD[9] = {'\0'}, COMMAND_BUFFER[512], RESPONSE_BUFFER[3274];
struct addrinfo *ADDR_UDP, *ADDR_TCP;

/**
 * @brief Sets up the udp and tcp server addresses
 *
 * @param ip
 * @param port
 * @return int
 */
int setupServerAddresses(char *ip, char *port)
{
    struct addrinfo hints;
    int errcode;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((errcode = getaddrinfo(ip, port, &hints, &ADDR_UDP)) != 0)
    {
        fprintf(stderr, "Error on getaddrinfo (udp): %s.\n", gai_strerror(errcode));
        return -1;
    }

    hints.ai_socktype = SOCK_STREAM;
    if ((errcode = getaddrinfo(ip, port, &hints, &ADDR_TCP)) != 0)
    {
        fprintf(stderr, "Error on getaddrinfo (tcp): %s.\n", gai_strerror(errcode));
        return -1;
    }

    return 0;
}

/**
 * @brief Sends a command to the server using UDP protocol
 *
 */
void sendCommandUDP()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating UDP socket.\n");
        return;
    }

    struct timeval tmout;
    memset((char *)&tmout, 0, sizeof(tmout));
    tmout.tv_sec = 15;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) < 0)
    {
        fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
        exit(EXIT_FAILURE);
    }

    if (sendto(sockfd, COMMAND_BUFFER, strlen(COMMAND_BUFFER), 0, ADDR_UDP->ai_addr, ADDR_UDP->ai_addrlen) == -1)
    {
        close(sockfd);
        fprintf(stderr, "sendto(COMMAND_BUFFER) failed.\n");
        return;
    }

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    if (recvfrom(sockfd, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER), 0, (struct sockaddr *)&addr, &addrlen) == -1)
    {
        close(sockfd);
        fprintf(stderr, "recvfrom(RESPONSE_BUFFER) failed.\n");
        return;
    }

    close(sockfd);
}

/**
 * @brief Opens a TCP socket
 *
 * @return int
 */
int openTCPSocket()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating TCP socket.\n");
        return -1;
    }

    struct timeval tmout;
    memset((char *)&tmout, 0, sizeof(tmout));
    tmout.tv_sec = 15;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) == -1)
    {
        fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
        return -1;
    }

    if (connect(sockfd, ADDR_TCP->ai_addr, ADDR_TCP->ai_addrlen) == -1)
    {
        close(sockfd);
        fprintf(stderr, "connect(ADDR_TCP) failed.\n");
        return -1;
    }

    return sockfd;
}

/**
 * @brief Exits the client
 *
 */
void freeServerAddresses()
{
    free(ADDR_UDP);
    free(ADDR_TCP);
}

/**
 * @brief Registers a user in the server
 *
 * @param uid
 * @param pass
 */
void registerUser(int uid, char *password)
{
    sprintf(COMMAND_BUFFER, "REG %05d %s\n", uid, password);
    sendCommandUDP();

    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%3s %3s", op, status);
    if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Invalid UID and/or password.\n");
    }
    else if (!strcmp(status, "DUP"))
    {
        fprintf(stderr, "User already registered.\n");
    }
    else
    {
        printf("User registered successfully.\n");
    }
}

/**
 * @brief Unregsiters a user from the server
 *
 * @param uid
 * @param password
 */
void unregisterUser(int uid, char *password)
{
    sprintf(COMMAND_BUFFER, "UNR %05d %s\n", uid, password);
    sendCommandUDP();

    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%3s %3s", op, status);
    if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Invalid UID and/or password, or user doesn't exist.\n");
    }
    else
    {
        printf("User unregistered successfully.\n");
    }

    if (LOGGED_IN && uid == UID && !strcmp(password, PASSWORD))
    {
        LOGGED_IN = false;
        UID = 0;
        bzero(PASSWORD, sizeof(PASSWORD));
        printf("Logged out successfully.\n");
    }
}

/**
 * @brief Logs a user in the server
 *
 * @param uid
 * @param password
 */
void login(int uid, char *password)
{
    if (LOGGED_IN)
    {
        fprintf(stderr, "[LOCAL] User needs to logout first.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "LOG %05d %s\n", uid, password);
    sendCommandUDP();

    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%3s %3s", op, status);
    if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Invalid UID and/or password, or user doesn't exist.\n");
    }
    else
    {
        LOGGED_IN = true;
        UID = uid;
        strncpy(PASSWORD, password, sizeof(PASSWORD) - 1);

        printf("User logged in successfully.\n");
    }
}

/**
 * @brief Logs a user out of the server
 *
 */
void logout()
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "[LOCAL] User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "OUT %05d %s\n", UID, PASSWORD);
    sendCommandUDP();

    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%3s %3s", op, status);
    if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Invalid UID and/or password, or user doesn't exist.\n");
    }
    else
    {
        LOGGED_IN = false;
        UID = 0;
        bzero(PASSWORD, sizeof(PASSWORD));
        printf("Logged out successfully.\n");
    }
}

/**
 * @brief Displays the user's UID
 *
 */
void showUID()
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "[LOCAL] User needs to be logged in.\n");
        return;
    }

    printf("[LOCAL] Currently selected UID: %d\n", UID);
}

/**
 * @brief Lists all the groups on the server
 *
 */
void groups()
{
    sprintf(COMMAND_BUFFER, "GLS\n");
    sendCommandUDP(COMMAND_BUFFER, RESPONSE_BUFFER);

    char op[4], n[3];
    sscanf(RESPONSE_BUFFER, "%3s %2s", op, n);
    printf("%d group(s):\n", atoi(n));

    int base = strlen(op) + strlen(n) + 2, last_read_size = 0;
    char gid[3], gname[25], mid[5];
    for (int i = 0; i < atoi(n); i++)
    {
        base += last_read_size;
        sscanf(&RESPONSE_BUFFER[base], "%2s %24s %4s", gid, gname, mid);
        last_read_size = 2 + strlen(gname) + 4 + 3;
        printf("%s %s %s\n", gid, gname, mid); // TODO atoi()??
    }
}

/**
 * @brief Subscribes a user to a group
 *
 * @param gid
 * @param gname
 */
void subscribe(int gid, char *gname)
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "[LOCAL] User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "GSR %05d %02d %s\n", UID, gid, gname);
    sendCommandUDP();

    char op[4], status[9];
    sscanf(RESPONSE_BUFFER, "%3s %8s", op, status);
    if (!strcmp(status, "E_USR"))
    {
        fprintf(stderr, "Invalid UID.\n");
    }
    else if (!strcmp(status, "E_GRP"))
    {
        fprintf(stderr, "Invalid GID.\n");
    }
    else if (!strcmp(status, "E_GNAME"))
    {
        fprintf(stderr, "Invalid GName.\n");
    }
    else if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Unexpected error occurred.\n");
    }
    else if (!strcmp(status, "FULL"))
    {
        fprintf(stderr, "Maximum number of groups reached.\n");
    }
    else if (!strcmp(status, "NEW"))
    {
        int n;
        sscanf(&RESPONSE_BUFFER[7], "%d", &n);
        printf("New group created: %d.\n", n);
    }
    else
    {
        printf("Subscribed to group successfully.\n");
    }
}

/**
 * @brief Unsubscribes a user from a group
 *
 * @param gid
 */
void unsubscribe(int gid)
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "[LOCAL] User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "GUR %05d %02d\n", UID, gid);
    sendCommandUDP();

    char op[4], status[9];
    sscanf(RESPONSE_BUFFER, "%3s %8s", op, status);
    if (!strcmp(status, "E_USR"))
    {
        fprintf(stderr, "Invalid UID.\n");
    }
    else if (!strcmp(status, "E_GRP"))
    {
        fprintf(stderr, "Invalid GID.\n");
    }
    else if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Group doesn't exist.\n");
    }
    else
    {
        printf("Unsubscribed from group %02d successfully.\n", gid);
    }
}

/**
 * @brief Lists all the groups a user is subscribed to
 *
 */
void myGroups()
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "[LOCAL] User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "GLM %05d\n", UID);
    sendCommandUDP();

    char op[4], n[3];
    sscanf(RESPONSE_BUFFER, "%3s %2s", op, n);
    printf("%d group(s):\n", atoi(n));

    int base = strlen(op) + strlen(n) + 2, last_read_size = 0;
    char gid[3], gname[25], mid[5];
    for (int i = 0; i < atoi(n); i++)
    {
        base += last_read_size;
        sscanf(&RESPONSE_BUFFER[base], "%2s %24s %4s", gid, gname, mid);
        last_read_size = 2 + strlen(gname) + 4 + 3;
        printf("%s %s %s\n", gid, gname, mid); // TODO atoi()??
    }
}

/**
 * @brief Selects a group
 *
 * @param gid
 */
void selectGroup(int gid)
{
    GROUP_SELECTED = true;
    GID = gid;
    printf("[LOCAL] %02d is now selected.\n", GID);
}

/**
 * @brief Displays the selected GID
 *
 */
void showGID()
{
    if (!GROUP_SELECTED)
    {
        fprintf(stderr, "[LOCAL] GID not selected.\n");
    }
    else
    {
        printf("[LOCAL] GID: %02d\n", GID);
    }
}

/**
 * @brief Displays all the users subscribed to the select group
 *
 */
void ulist()
{
    sprintf(COMMAND_BUFFER, "ULS %02d\n", GID);

    int sockfd;
    if ((sockfd = openTCPSocket()) == -1)
    {
        return;
    }

    if (!GROUP_SELECTED)
    {
        fprintf(stderr, "Group needs to be selected.\n");
        return;
    }

    if (write(sockfd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Couldn't send command_buffer.\n");
        return;
    }

    int n;
    bzero(RESPONSE_BUFFER, 8);
    n = read(sockfd, RESPONSE_BUFFER, 7);
    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%s %s", op, status);
    if (!strcmp(status, "NOK"))
    {
        close(sockfd);
        fprintf(stderr, "Group %02d doesn't exist.\n", GID);
        return;
    }

    if (n == 6)
    {
        read(sockfd, RESPONSE_BUFFER, 1);
    }

    bzero(RESPONSE_BUFFER, 32);
    while ((n = read(sockfd, RESPONSE_BUFFER, 31)) > 0)
    {
        printf("%s", RESPONSE_BUFFER);
        bzero(RESPONSE_BUFFER, n);
    }

    if (n == -1)
    {
        fprintf(stderr, "Error receiving server's response.\n");
    }

    close(sockfd);
}

/**
 * @brief Posts a message to a group
 *
 * @param message
 * @param fname
 */
void post(char *message, char *fname)
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    if (!GROUP_SELECTED)
    {
        fprintf(stderr, "Group needs to be selected.\n");
        return;
    }

    int sockfd;
    if ((sockfd = openTCPSocket()) == -1)
    {
        return;
    }

    if (fname == NULL)
    {
        sprintf(COMMAND_BUFFER, "PST %05d %02d %lu %s\n", UID, GID, strlen(message), message);
        if (write(sockfd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
        {
            close(sockfd);
            fprintf(stderr, "Couldn't send command_buffer.\n");
            return;
        }

        bzero(RESPONSE_BUFFER, 10);
        if (read(sockfd, RESPONSE_BUFFER, 9) == -1)
        {
            close(sockfd);
            fprintf(stderr, "Error receiving server's response.\n");
            return;
        }

        close(sockfd);

        char op[4], status[5];
        sscanf(RESPONSE_BUFFER, "%s %s", op, status);
        if (!strcmp(status, "NOK"))
        {
            fprintf(stderr, "Unknown error occurred.\n");
        }
        else
        {
            printf("Posted in group %d successfully, message id is %d.\n", GID, atoi(status));
        }
        return;
    }

    FILE *fptr = fopen(fname, "rb");
    if (fptr == NULL)
    {
        fprintf(stderr, "File does not exist.\n");
        return;
    }
    fseek(fptr, 0L, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    sprintf(COMMAND_BUFFER, "PST %05d %02d %lu %s %s %lu ", UID, GID, strlen(message), message, basename(fname), fsize);
    printf("%s\n", COMMAND_BUFFER);
    if (write(sockfd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Couldn't send command_buffer.\n");
        return;
    }

    int bytes_read;
    char file_buffer[1024];
    bzero(file_buffer, sizeof(file_buffer));
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), fptr)) > 0)
    {
        if (write(sockfd, file_buffer, bytes_read) == -1)
        {
            switch (errno)
            {
            case ECONNRESET:
                bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
                if (read(sockfd, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER)) == -1)
                {
                    close(sockfd);
                    fprintf(stderr, "Error receiving server's response.\n");
                    return;
                }
                printf("%s", RESPONSE_BUFFER);
                break;

            default:
                fprintf(stderr, "Couldn't send file: %s.\n", strerror(errno));
                break;
            }

            close(sockfd);
            return;
        }

        bzero(file_buffer, bytes_read);
    }

    fclose(fptr);

    if (write(sockfd, "\n", 1) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Couldn't send '\\n' after file: %s.\n", strerror(errno));
        return;
    }

    bzero(RESPONSE_BUFFER, 10);
    if (read(sockfd, RESPONSE_BUFFER, 9) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }

    close(sockfd);

    char op[4], status[5];
    sscanf(RESPONSE_BUFFER, "%s %s", op, status);
    if (!strcmp(status, "NOK"))
    {
        fprintf(stderr, "Unknown error occurred.\n");
    }
    else
    {
        printf("Posted in group %d successfully, message id is %d.\n", GID, atoi(status));
    }
}

/**
 * @brief Retrieves messages from a group
 *
 * @param mid
 */
void retrieve(int mid)
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    if (!GROUP_SELECTED)
    {
        fprintf(stderr, "Group needs to be selected.\n");
        return;
    }

    int sockfd;
    if ((sockfd = openTCPSocket()) == -1)
    {
        return;
    }

    sprintf(COMMAND_BUFFER, "RTV %05d %02d %04d\n", UID, GID, mid);
    if (write(sockfd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Couldn't send command_buffer.\n");
        return;
    }

    bzero(RESPONSE_BUFFER, 8);
    if (read(sockfd, RESPONSE_BUFFER, 7) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }

    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%s %s", op, status);
    if (strcmp(status, "OK"))
    {
        close(sockfd);
        printf("%s %s\n", op, status);
        return;
    }

    int bytes_read;
    char file_buffer[1024];
    bzero(file_buffer, sizeof(file_buffer));
    FILE *tmpfptr = fopen("temp.bin", "wb+");
    while ((bytes_read = read(sockfd, file_buffer, sizeof(file_buffer))) > 0)
    {
        fwrite(file_buffer, sizeof(char), bytes_read, tmpfptr);
        bzero(file_buffer, bytes_read);
    }

    if (bytes_read == -1)
    {
        close(sockfd);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }

    rewind(tmpfptr);

    int n_msg;
    fscanf(tmpfptr, "%d ", &n_msg);
    printf("Retrieving %d message(s) from group %d.\n", n_msg, GID);

    for (int i = 0; i < n_msg; i++)
    {
        char curr_mid[5], uid[6], tsize[4];
        fscanf(tmpfptr, "%4s %5s %3s", curr_mid, uid, tsize);

        char path_buffer[256];
        sprintf(path_buffer, "GROUP_%02d_MSG_%s.txt", GID, curr_mid);
        FILE *msgfptr = fopen(path_buffer, "w");

        fgetc(tmpfptr);
        bzero(file_buffer, atoi(tsize) + 1);
        fread(file_buffer, sizeof(char), atoi(tsize), tmpfptr);

        printf("%s", file_buffer);

        fwrite(file_buffer, sizeof(char), atoi(tsize), msgfptr);
        fclose(msgfptr);

        if (fgetc(tmpfptr) == '\n')
        {
            printf("\n");
            break;
        }

        char c;
        if ((c = fgetc(tmpfptr)) != '/')
        {
            fseek(tmpfptr, -1L, SEEK_CUR);
        }
        else
        {
            fgetc(tmpfptr);

            char fname[25], fsize[11];
            fscanf(tmpfptr, "%24s %10s", fname, fsize);

            printf(" %s %s Bytes", fname, fsize);

            fgetc(tmpfptr);
            int iter = atoi(fsize);
            FILE *datafptr = fopen(fname, "wb");
            for (int j = 0; j < iter; j += 1024)
            {
                size_t size = fread(file_buffer, sizeof(char), ((1024) < (iter - j)) ? (1024) : (iter - j), tmpfptr);
                fwrite(file_buffer, sizeof(char), size, datafptr);
            }

            fclose(datafptr);
        }

        printf("\n");
        bzero(file_buffer, sizeof(file_buffer));
    }

    fclose(tmpfptr);
    unlink("temp.bin");
    close(sockfd);
}