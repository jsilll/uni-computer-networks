#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <libgen.h>
#include <errno.h>
#include "commands.h" // TODO remove unused imports, all files
// TODO interface.h?

bool LOGGED_IN = false, GROUP_SELECTED = false, CONNECTED = false;
char UID[6], PASSWORD[9], GID[3];                // TODO 8 16 4
char COMMAND_BUFFER[512], RESPONSE_BUFFER[3274]; // TODO 4096
int SOCKFD;
struct addrinfo *ADDR_UDP, *ADDR_TCP;
// TODO unecessarily large bzeros

/**
 * @brief Sets up the UDP socket
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
        fprintf(stderr, "Error on getaddrinfo (udp): %s.\n", gai_strerror(errcode));
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
    if ((SOCKFD = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating UDP socket.\n");
        return;
    }

    CONNECTED = true;

    if (sendto(SOCKFD, COMMAND_BUFFER, strlen(COMMAND_BUFFER), 0, ADDR_UDP->ai_addr, ADDR_UDP->ai_addrlen) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "sendto(COMMAND_BUFFER) failed.\n");
        return;
    }

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    if (recvfrom(SOCKFD, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER), 0, (struct sockaddr *)&addr, &addrlen) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "recvfrom(RESPONSE_BUFFER) failed.\n");
        return;
    }

    close(SOCKFD);
    CONNECTED = false;
}

/**
 * @brief Opens a TCP socket
 * 
 * @return int 
 */
int openTCPSocket()
{
    if ((SOCKFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating TCP socket.\n");
        return -1;
    }

    CONNECTED = true;

    struct timeval tmout;
    memset((char *)&tmout, 0, sizeof(tmout));
    tmout.tv_sec = 15;
    if (setsockopt(SOCKFD, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) == -1)
    {
        fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
        return -1;
    }

    if (connect(SOCKFD, ADDR_TCP->ai_addr, ADDR_TCP->ai_addrlen) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "connect(ADDR_TCP) failed.\n");
        return -1;
    }

    return SOCKFD;
}

/**
 * @brief Exits the client
 * 
 */
void closeAllConnections()
{
    if (CONNECTED)
    {
        close(SOCKFD);
    }
    free(ADDR_UDP);
    free(ADDR_TCP);
    printf("[LOCAL] exiting client.\n");
}

/**
 * @brief Registers a user
 *
 * @param uid
 * @param pass
 */
void registerUser(char *uid, char *password)
{
    sprintf(COMMAND_BUFFER, "REG %s %s\n", uid, password);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Unregsiters a user
 *
 * @param uid
 * @param password
 */
void unregisterUser(char *uid, char *password)
{
    sprintf(COMMAND_BUFFER, "UNR %s %s\n", uid, password);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);

    if (LOGGED_IN && !strcmp(uid, UID) && !strcmp(password, PASSWORD))
    {
        LOGGED_IN = false;
    }
}

/**
 * @brief Logs a user in the server
 *
 * @param uid
 * @param password
 */
void login(char *uid, char *password)
{
    if (LOGGED_IN)
    {
        fprintf(stderr, "User needs to logout first.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "LOG %s %s\n", uid, password);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
    if (!strcmp(RESPONSE_BUFFER, "RLO OK\n"))
    {
        LOGGED_IN = true;
        strncpy(UID, uid, sizeof(UID) - 1);
        strncpy(PASSWORD, password, sizeof(PASSWORD) - 1);
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
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "OUT %s %s\n", UID, PASSWORD);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
    if (!strcmp(RESPONSE_BUFFER, "ROU OK\n"))
    {
        LOGGED_IN = false;
        bzero(UID, sizeof(UID));
        bzero(PASSWORD, sizeof(PASSWORD));
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
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    printf("[LOCAL] UID: %s\n", UID);
}

/**
 * @brief Lists all the groups on the server
 * 
 */
void groups()
{
    sprintf(COMMAND_BUFFER, "GLS\n");
    sendCommandUDP(COMMAND_BUFFER, RESPONSE_BUFFER);
    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Subscribes a user to a group
 *
 * @param gid
 * @param gname
 */
void subscribe(char *gid, char *gname)
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "GSR %s %02d %s\n", UID, atoi(gid), gname);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Unsubscribes a user from a group
 *
 * @param gid
 */
void unsubscribe(char *gid)
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "GUR %s %s\n", UID, gid);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Lists all the groups a user is subscribed to
 *
 */
void myGroups()
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "User needs to be logged in.\n");
        return;
    }

    sprintf(COMMAND_BUFFER, "GLM %s\n", UID);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Selects a group
 *
 * @param gid
 */
void selectGroup(char *gid)
{
    GROUP_SELECTED = true;
    sprintf(GID, "%02d", atoi(gid));
    printf("[LOCAL] %s is now selected.\n", GID);
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
        printf("[LOCAL] GID: %s\n", GID);
    }
}

/**
 * @brief Displays all the users subscribed to the select group
 * 
 */
void ulist()
{
    sprintf(COMMAND_BUFFER, "ULS %s\n", GID);

    if ((SOCKFD = openTCPSocket()) == -1)
    {
        return;
    }

    if (!GROUP_SELECTED)
    {
        fprintf(stderr, "Group needs to be selected.\n");
        return;
    }

    if (write(SOCKFD, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Couldn't send command_buffer.\n");
        return;
    }

    int n;
    bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    while ((n = read(SOCKFD, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER))) > 0)
    {
        printf("%s", RESPONSE_BUFFER);
        bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    }

    if (n == -1)
    {
        fprintf(stderr, "Error receiving server's response.\n");
    }

    close(SOCKFD);
    CONNECTED = false;
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

    if ((SOCKFD = openTCPSocket()) == -1)
    {
        return;
    }

    if (fname == NULL)
    {
        sprintf(COMMAND_BUFFER, "PST %s %s %lu %s\n", UID, GID, strlen(message), message);
        if (write(SOCKFD, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
        {
            close(SOCKFD);
            fprintf(stderr, "Couldn't send command_buffer.\n");
            return;
        }

        bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
        if (read(SOCKFD, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER)) == -1)
        {
            close(SOCKFD);
            fprintf(stderr, "Error receiving server's response.\n");
            return;
        }

        printf("%s", RESPONSE_BUFFER);
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

    sprintf(COMMAND_BUFFER, "PST %s %s %lu %s %s %lu ", UID, GID, strlen(message), message, basename(fname), fsize);
    printf("%s\n", COMMAND_BUFFER);
    if (write(SOCKFD, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Couldn't send command_buffer.\n");
        return;
    }

    int bytes_read;
    char file_buffer[1024];
    bzero(file_buffer, sizeof(file_buffer));
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), fptr)) > 0)
    {
        if (write(SOCKFD, file_buffer, bytes_read) == -1)
        {
            switch (errno)
            {
            case ECONNRESET:
                bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
                if (read(SOCKFD, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER)) == -1)
                {
                    close(SOCKFD);
                    fprintf(stderr, "Error receiving server's response.\n");
                    return;
                }
                printf("%s", RESPONSE_BUFFER);
                break;

            default:
                fprintf(stderr, "Couldn't send file: %s.\n", strerror(errno));
                break;
            }

            close(SOCKFD);
            return;
        }

        bzero(file_buffer, sizeof(file_buffer));
    }

    fclose(fptr);

    if (write(SOCKFD, "\n", 1) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Couldn't send '\\n' after file: %s.\n", strerror(errno));
        return;
    }

    bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    if (read(SOCKFD, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER)) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }

    close(SOCKFD);
    CONNECTED = false;

    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Retrieves messages from a group
 *
 * @param mid
 */
void retrieve(char *mid)
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

    if ((SOCKFD = openTCPSocket()) == -1)
    {
        return;
    }

    sprintf(COMMAND_BUFFER, "RTV %s %s %s\n", UID, GID, mid);
    if (write(SOCKFD, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Couldn't send command_buffer.\n");
        return;
    }

    bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    if (read(SOCKFD, RESPONSE_BUFFER, 7) == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }

    char op[4], status[4];
    sscanf(RESPONSE_BUFFER, "%s %s", op, status);
    if (strcmp(status, "OK"))
    {
        close(SOCKFD);
        printf("%s %s\n", op, status);
        return;
    }

    int bytes_read;
    char file_buffer[1024];
    bzero(file_buffer, sizeof(file_buffer));
    FILE *tmpfptr = fopen("temp.bin", "wb+");
    while ((bytes_read = read(SOCKFD, file_buffer, sizeof(file_buffer))) > 0)
    {
        fwrite(file_buffer, sizeof(char), bytes_read, tmpfptr);
        bzero(file_buffer, sizeof(file_buffer));
    }

    if (bytes_read == -1)
    {
        close(SOCKFD);
        fprintf(stderr, "Error receiving server's response.\n");
        return;
    }

    rewind(tmpfptr);

    int n_msg;
    fscanf(tmpfptr, "%d ", &n_msg);
    printf("%s %s %d\n", op, status, n_msg);

    for (int i = 0; i < n_msg; i++)
    {
        char curr_mid[5], uid[6], tsize[4];
        fscanf(tmpfptr, "%4s %5s %3s", curr_mid, uid, tsize);

        char path_buffer[256];
        sprintf(path_buffer, "GROUP%s_MSG%s.txt", GID, curr_mid);
        FILE *msgfptr = fopen(path_buffer, "w");

        fgetc(tmpfptr);
        bzero(file_buffer, 1024);
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

            printf(" %s %s", fname, fsize);

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
    close(SOCKFD);
    CONNECTED = false;
}