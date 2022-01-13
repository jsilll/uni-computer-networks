#ifndef TCP_HANDLING_H_
#define TCP_HANDLING_H_

#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "command_args_parsing.h"
#include "operations.h"

/**
 * @brief Handles incoming commands from a tcp connection
 *
 * @param connfd
 * @param verbose
 */
void handleTCPCommand(int connfd, bool verbose)
{
    char op[5];
    bzero(op, sizeof(op));
    read(connfd, op, 4);
    if (op[3] == ' ')
    {
        op[3] = '\0';
    }

    char command_buffer[37], buffer[1024];
    bzero(buffer, sizeof(buffer));
    if (!strcmp(op, "ULS"))
    {
        bzero(command_buffer, 4);
        read(connfd, command_buffer, 3);

        char gid[4];
        sscanf(command_buffer, "%3[^\n]", gid);

        if (verbose)
        {
            printf("CMD: %s %s\n", op, gid);
        }

        if (parseGID(gid) == -1)
        {
            strcpy(buffer, "RUL NOK\n");
            write(connfd, buffer, strlen(buffer));
        }
        else
        {
            DIR *dr;
            if ((dr = ulist(gid)) == NULL)
            {
                strcpy(buffer, "RUL NOK\n");
                write(connfd, buffer, strlen(buffer));
            }
            else
            {
                strcpy(buffer, "RUL OK");
                write(connfd, buffer, strlen(buffer));

                ulsGetGName(gid, buffer);
                write(connfd, buffer, strlen(buffer));

                struct dirent *de;
                bzero(buffer, sizeof(buffer));
                while ((de = readdir(dr)) != NULL)
                {
                    ulsAppendUser(de, buffer);

                    if (strlen(buffer) > 512)
                    {
                        write(connfd, buffer, strlen(buffer));
                        bzero(buffer, strlen(buffer));
                    }
                }
                write(connfd, buffer, strlen(buffer));
                write(connfd, "\n", 1);
            }
        }
    }
    else if (!strcmp(op, "PST"))
    {
        char uid[7], gid[4], tsize[5], text[241];

        bzero(command_buffer, 10);
        read(connfd, command_buffer, 9);

        sscanf(command_buffer, "%6s %3s", uid, gid);

        bzero(command_buffer, 5);
        read(connfd, command_buffer, 4);

        sscanf(command_buffer, "%4s", tsize);
        bzero(text, sizeof(text));
        strcpy(text, &command_buffer[strlen(tsize) + 1]);

        if (parseUID(uid) == -1 || parseGID(gid) == -1 || parseTSize(tsize) == -1)
        {
            if (verbose)
            {
                printf("CMD: %s %s %s %s\n", op, uid, gid, tsize);
            }

            strcpy(buffer, "RPT NOK\n");
            write(connfd, buffer, strlen(buffer));
        }
        else
        {
            char c, mid[5];
            if (atoi(tsize) == 1)
            {
                if (verbose)
                {
                    printf("CMD: %s %s %s %s\n", op, uid, gid, tsize);
                }

                text[1] = '\0';
                if (post(uid, gid, text, NULL, mid) == NULL)
                {
                    strcpy(buffer, "RPT NOK\n");
                    write(connfd, buffer, strlen(buffer));
                }
                else
                {
                    sprintf(buffer, "RPT %s\n", mid);
                    write(connfd, buffer, strlen(buffer));
                }
            }
            else
            {
                read(connfd, &text[strlen(text)], atoi(tsize) - strlen(text));
                read(connfd, &c, 1);
                if (c == '\n')
                {
                    if (verbose)
                    {
                        printf("CMD: %s %s %s %s\n", op, uid, gid, tsize);
                    }

                    if (post(uid, gid, text, NULL, mid) == NULL)
                    {
                        strcpy(buffer, "RPT NOK\n");
                        write(connfd, buffer, strlen(buffer));
                    }
                    else
                    {
                        sprintf(buffer, "RPT %s\n", mid);
                        write(connfd, buffer, strlen(buffer));
                    }
                }
                else if (c == ' ')
                {
                    bzero(command_buffer, sizeof(command_buffer));
                    int n = read(connfd, command_buffer, 36);

                    char fname[26], fsize[12];
                    sscanf(command_buffer, "%25s %11s", fname, fsize);

                    if (verbose)
                    {
                        printf("CMD: %s %s %s %s %s %s\n", op, uid, gid, tsize, fname, fsize);
                    }

                    if (parseFileSize(fsize) == -1 || parseFName(fname) == -1)
                    {
                        strcpy(buffer, "RPT NOK\n");
                        write(connfd, buffer, strlen(buffer));
                    }
                    else
                    {
                        FILE *FPtr;
                        if ((FPtr = post(uid, gid, text, fname, mid)) == NULL)
                        {
                            strcpy(buffer, "RPT NOK\n");
                            write(connfd, buffer, strlen(buffer));
                        }
                        else
                        {
                            int fbytes = atoi(fsize), fbytes_read = n - (strlen(fname) + strlen(fsize) + 2);

                            if (fbytes_read > 0)
                            {
                                writeToFile(FPtr, &command_buffer[strlen(fname) + strlen(fsize) + 2], fbytes_read);
                            }

                            bzero(buffer, sizeof(buffer));
                            while ((fbytes_read < fbytes) && (n = read(connfd, buffer, (((sizeof(buffer)) < (fbytes - fbytes_read)) ? (sizeof(buffer)) : (fbytes - fbytes_read)))) > 0)
                            {
                                writeToFile(FPtr, buffer, n);

                                fbytes_read += n;
                                bzero(buffer, n);
                            }
                            fclose(FPtr);

                            sprintf(buffer, "RPT %s\n", mid);
                            write(connfd, buffer, strlen(buffer));
                        }
                    }
                }
                else
                {
                    strcpy(buffer, "RPT NOK\n");
                    write(connfd, buffer, strlen(buffer));
                }
            }
        }
    }
    else if (!strcmp(op, "RTV"))
    {
        bzero(command_buffer, 15);
        read(connfd, command_buffer, 14);

        char uid[7], gid[4], mid[6];
        sscanf(command_buffer, "%6s %3s %5s", uid, gid, mid);

        if (verbose)
        {
            printf("CMD: %s %s %s %s\n", op, uid, gid, mid);
        }

        int n_msg = 0;
        if (parseUID(uid) == -1 || parseGID(gid) == -1 || parseMID(mid) == -1 || (n_msg = retrieve(uid, gid, mid)) == -1)
        {
            strcpy(buffer, "RRT NOK\n");
            write(connfd, buffer, strlen(buffer));
        }
        else if (n_msg == 0)
        {
            strcpy(buffer, "RRT EOF\n");
            write(connfd, buffer, strlen(buffer));
        }
        else
        {
            sprintf(buffer, "RRT OK %d", n_msg);
            write(connfd, buffer, strlen(buffer));

            int base_msg = atoi(mid);
            for (int i = 0; i < n_msg; i++)
            {
                bzero(buffer, sizeof(buffer));
                FILE *FPtr = retrieveAux(gid, base_msg + i, buffer);
                write(connfd, buffer, strlen(buffer));

                if (FPtr != NULL)
                {
                    int bytes_read;
                    bzero(buffer, sizeof(buffer));
                    while ((bytes_read = readFile(FPtr, buffer, 1024)) > 0)
                    {
                        if (write(connfd, buffer, bytes_read) == -1)
                        {
                            close(connfd);
                            fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
                        }
                        bzero(buffer, bytes_read);
                    }
                    fclose(FPtr);
                }
            }

            write(connfd, "\n", 1);
        }
    }
    else
    {
        strcpy(buffer, "ERR\n");
        write(connfd, buffer, strlen(buffer));
    }

    close(connfd);
}

#endif // TCP_HANDLING_H_