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
#include "commands.h"

bool LOGGED_IN = false;
char UID[6], PASSWORD[9], GID[3];                // TODO 8 16 4
char COMMAND_BUFFER[512], RESPONSE_BUFFER[3275]; // TODO 4096
struct addrinfo *ADDR_UDP, *ADDR_TCP;

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
 * @brief Frees the server adresses
 */
void freeServerAddress()
{
    free(ADDR_UDP);
    free(ADDR_TCP);
}

/**
 * @brief Sends a command to the server using UDP protocol
 *
 * @param command_buffer
 */
void sendCommandUDP()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating UDP socket.\n");
        return;
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
 * @brief Sends a command to the server using TCP protocol
 *
 * @param command_buffer
 * @param response_buffer
 */
void sendCommandTCP()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating TCP socket.\n");
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

    if (connect(sockfd, ADDR_TCP->ai_addr, ADDR_TCP->ai_addrlen) == -1)
    {
        close(sockfd);
        fprintf(stderr, "connect(ADDR_TCP) failed.\n");
        return;
    }

    if (write(sockfd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
    {
        close(sockfd);
        fprintf(stderr, "write(COMMAND_BUFFER) failed.\n");
        return;
    }

    bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
    if (read(sockfd, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER)) == -1)
    {
        close(sockfd);
        fprintf(stderr, "read(RESPONSE_BUFFER) failed.\n");
        return;
    }

    close(sockfd);
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
}

/**
 * @brief Logs a user in the server
 *
 * @param uid
 * @param password
 */
void login(char *uid, char *password)
{
    sprintf(COMMAND_BUFFER, "LOG %s %s\n", uid, password);
    sendCommandUDP();
    printf("%s", RESPONSE_BUFFER);
    if (!strcmp(RESPONSE_BUFFER, "RLO OK\n"))
    {
        LOGGED_IN = true;
        strcpy(UID, uid);
        strcpy(PASSWORD, password);
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
    }
    else
    {
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
    }
    else
    {
        printf("[LOCAL] UID: %s\n", UID);
    }
}

/**
 * @brief Exits the client
 * 
 */
void exitClient()
{
    printf("[LOCAL] exit.\n"); // TODO, close all TCP connections
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
    }
    else
    {
        sprintf(COMMAND_BUFFER, "GSR %s %s %s\n", UID, gid, gname);
        sendCommandUDP();
        printf("%s", RESPONSE_BUFFER);
    }
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
        fprintf(stderr, "User need to be logged in.\n");
    }
    else
    {
        sprintf(COMMAND_BUFFER, "GUR %s %s\n", UID, gid);
        sendCommandUDP();
        printf("%s", RESPONSE_BUFFER);
    }
}

/**
 * @brief Lists all the groups a user is subscribed to
 *
 */
void myGroups()
{
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
    strcpy(GID, gid);
    printf("[LOCAL] %s is now selected.\n", gid);
}

/**
 * @brief Displays the selected GID
 */
void showGID()
{
    printf("[LOCAL] GID: %s\n", GID);
}

/**
 * @brief Lists all users subscribed to a group
 *
 */
void ulist()
{
    sprintf(COMMAND_BUFFER, "ULS %s\n", GID);
    sendCommandTCP();
    printf("%s", RESPONSE_BUFFER);
}

/**
 * @brief Posts a message to a group
 * 
 * @param message
 * @param fname
 */
void post(char *message, char *fname) /* TODO size não pode exceder tamanho */
{
    if (!LOGGED_IN)
    {
        fprintf(stderr, "User need to be logged in.\n");
    }
    else
    {
        if (fname != NULL)
        {
            FILE *FPtr = fopen(fname, "rb");
            if (FPtr == NULL)
            {
                fprintf(stderr, "Couldn't open file. File does not exist.\n");
                return;
            }

            fseek(FPtr, 0L, SEEK_END);
            long size = ftell(FPtr);
            rewind(FPtr);
            sprintf(COMMAND_BUFFER, "PST %s %s %lu %s %s %lu ", UID, GID, strlen(message), message, basename(fname), size);

            int fd;
            bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));

            if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                fprintf(stderr, "Couldn't send command_buffer. Error creating TCP socket.\n");
                return;
            }

            struct timeval tmout;
            memset((char *)&tmout, 0, sizeof(tmout));
            tmout.tv_sec = 15;
            if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) < 0)
            {
                fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
                exit(EXIT_FAILURE);
            }

            if (connect(fd, ADDR_TCP->ai_addr, ADDR_TCP->ai_addrlen) == -1)
            {
                close(fd);
                fprintf(stderr, "Couldn't send command_buffer. Error establishing a connection with server.\n");
                return;
            }

            if (write(fd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
            {
                close(fd);
                fprintf(stderr, "Couldn't send command_buffer.\n");
                return;
            }

            char data[1024];
            bzero(data, sizeof(data));
            int bytes_read;
            while ((bytes_read = fread(data, 1, sizeof(data), FPtr)) > 0)
            {
                if (write(fd, data, bytes_read) == -1)
                {
                    // Ler o RPT NOK
                    switch (errno)
                    {
                    case ECONNRESET:
                        read(fd, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
                        printf("%s", RESPONSE_BUFFER);
                        break;

                    default:
                        fprintf(stderr, "Couldn't send file: %s.\n", strerror(errno));
                        break;
                    }
                    close(fd);
                    return;
                }
                bzero(data, sizeof(data));
            }
            fclose(FPtr);
            if (write(fd, "\n", 1) == -1)
            {
                fprintf(stderr, "Couldn't send file %s.\n", strerror(errno));
            }

            if (read(fd, RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER)) == -1)
            {
                close(fd);
                fprintf(stderr, "Error receiving server's response.\n");
                return;
            }

            close(fd);
            printf("%s", RESPONSE_BUFFER);
        }
        else
        {
            sprintf(COMMAND_BUFFER, "PST %s %s %lu %s\n", UID, GID, strlen(message), message);
            sendCommandTCP();
            printf("%s", RESPONSE_BUFFER);
        }
    }
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
        fprintf(stderr, "User need to be logged in.\n");
    }
    else
    {
        sprintf(COMMAND_BUFFER, "RTV %s %s %s\n", UID, GID, mid);

        int fd;

        bzero(RESPONSE_BUFFER, sizeof(RESPONSE_BUFFER));
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            fprintf(stderr, "Error creating TCP socket.\n");
            return;
        }

        struct timeval tmout;
        memset((char *)&tmout, 0, sizeof(tmout));
        tmout.tv_sec = 15;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) < 0)
        {
            fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
            exit(EXIT_FAILURE);
        }

        if (connect(fd, ADDR_TCP->ai_addr, ADDR_TCP->ai_addrlen) == -1)
        {
            close(fd);
            fprintf(stderr, "connect(ADDR_TCP) failed.\n");
            return;
        }

        if (write(fd, COMMAND_BUFFER, strlen(COMMAND_BUFFER)) == -1)
        {
            close(fd);
            fprintf(stderr, "Couldn't send command_buffer.\n");
            return;
        }

        int n;
        if ((n = read(fd, RESPONSE_BUFFER, 7)) == -1)
        {
            close(fd);
            fprintf(stderr, "Error receiving server's response.\n");
            return;
        }
        else
        {
            char op[4], status[4];
            sscanf(RESPONSE_BUFFER, "%s %s", op, status);

            if (!strcmp(status, "OK"))
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
                printf("%s %s %d\n", op, status, num_msg);

                for (int i = 0; i < num_msg; i++)
                {
                    char mid[5], uid[6], tsize[4];
                    fscanf(TempFile, "%s %s %s", mid, uid, tsize);
                    char path_buffer[256];
                    sprintf(path_buffer, "GROUP%s_MSG%s.txt", GID, mid);
                    FILE *MessageFile = fopen(path_buffer, "w");
                    bzero(data, 1024);
                    fgetc(TempFile);
                    fread(data, sizeof(char), atoi(tsize), TempFile);
                    printf("%s", data);
                    fwrite(data, sizeof(char), atoi(tsize), MessageFile);
                    fclose(MessageFile);

                    fgetc(TempFile);
                    char c = fgetc(TempFile);
                    if (c == '/')
                    {
                        char fname[25], fsize[11];
                        fgetc(TempFile);
                        fscanf(TempFile, "%s %s", fname, fsize);
                        printf(" %s", fname);
                        fgetc(TempFile);
                        FILE *DataFile = fopen(fname, "wb");
                        int iter = atoi(fsize);

                        for (int j = 0; j < iter; j += 1024)
                        {
                            size_t size = fread(data, sizeof(char), ((1024) < (iter - j)) ? (1024) : (iter - j), TempFile);
                            fwrite(data, sizeof(char), size, DataFile);
                        }
                        fclose(DataFile);
                    }
                    else
                    {
                        fseek(TempFile, -1L, SEEK_CUR);
                    }

                    printf("\n");
                    bzero(data, 1024);
                }

                fclose(TempFile);
                unlink("temp.bin");
            }
            else
            {
                printf(RESPONSE_BUFFER);
            }
        }

        close(fd);
    }
}