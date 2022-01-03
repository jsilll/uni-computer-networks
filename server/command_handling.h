#ifndef RC_PROJECT_SERVER_COMMAND_HANDLING_H_
#define RC_PROJECT_SERVER_COMMAND_HANDLING_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "state/parsing.h"
#include "state/operations.h"

#define MAX_INPUT_SIZE 1024
#define MAX_RESPONSE_SIZE 3307

void handleCommandUDP(int udpfd, struct sockaddr_in cliaddr, bool verbose)
{
  socklen_t len = sizeof(cliaddr);
  char command_buffer[MAX_INPUT_SIZE];
  char response_buffer[MAX_RESPONSE_SIZE];

  bzero(command_buffer, MAX_INPUT_SIZE);
  recvfrom(udpfd, command_buffer, MAX_INPUT_SIZE, 0, (struct sockaddr *)&cliaddr, &len);

  if (verbose)
  {
    printf("[UDP] IP: %s PORT: %lu ", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    printf("%s", command_buffer);
  }

  char op[MAX_INPUT_SIZE] = {'\0'};
  char arg1[MAX_INPUT_SIZE] = {'\0'};
  char arg2[MAX_INPUT_SIZE] = {'\0'};
  char arg3[MAX_INPUT_SIZE] = {'\0'};
  char arg4[MAX_INPUT_SIZE] = {'\0'};
  char arg5[MAX_INPUT_SIZE] = {'\0'};
  char arg6[MAX_INPUT_SIZE] = {'\0'};
  char arg7[MAX_INPUT_SIZE] = {'\0'};
  sscanf(command_buffer, "%s %s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

  if (!strcmp(op, "REG"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH)
    {
      strcpy(response_buffer, "RRG NOK\n");
    }
    else if (REG(arg1, arg2) == -1)
    {
      strcpy(response_buffer, "RRG DUP\n");
    }
    else
    {
      strcpy(response_buffer, "RRG OK\n");
    }
  }
  else if (!strcmp(op, "UNR"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH || UNR(arg1, arg2) == -1)
    {
      strcpy(response_buffer, "RUN NOK\n");
    }
    else
    {
      strcpy(response_buffer, "RUN OK\n");
    }
  }
  else if (!strcmp(op, "LOG"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH || LOG(arg1, arg2) == -1)
    {
      strcpy(response_buffer, "RLO NOK\n");
    }
    else
    {
      strcpy(response_buffer, "RLO OK\n");
    }
  }
  else if (!strcmp(op, "OUT"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH || OUT(arg1, arg2) == -1)
    {
      strcpy(response_buffer, "ROU NOK\n");
    }
    else
    {
      strcpy(response_buffer, "ROU OK\n");
    }
  }
  else if (!strcmp(op, "GLS"))
  {
    GLS(response_buffer);
  }
  else if (!strcmp(op, "GSR"))
  {
    if (parseUID(arg1) == -1)
    {
      strcpy(response_buffer, "RGS E_USR\n");
    }
    else if (parseGID(arg2) == -1)
    {
      strcpy(response_buffer, "RGS E_GRP\n");
    }
    else if (parseGName(arg3) == REG_NOMATCH)
    {
      strcpy(response_buffer, "RGS E_GNAME\n");
    }
    else
    {
      int res;
      switch (res = GSR(arg1, arg2, arg3))
      {
      case 0:
        strcpy(response_buffer, "RGS OK\n");
        break;
      case -1:
        strcpy(response_buffer, "RGS NOK\n");
        break;
      case -2:
        strcpy(response_buffer, "RGS FULL\n");
        break;
      default:
        sprintf(response_buffer, "RGS NEW %02d\n", res);
        break;
      }
    }
  }
  else if (!strcmp(op, "GUR"))
  {
    if (parseUID(arg1) == -1)
    {
      strcpy(response_buffer, "RGU E_USR\n");
    }
    else if (parseGID(arg2) == -1)
    {
      strcpy(response_buffer, "RGU E_GRP\n");
    }
    else if (GUR(arg1, arg2) == -1)
    {
      strcpy(response_buffer, "RGU NOK\n");
    }
    else
    {
      strcpy(response_buffer, "RGU OK\n");
    }
  }
  else if (!strcmp(op, "GLM"))
  {
    if (parseUID(arg1) == -1 || GLM(arg1, response_buffer) == -1)
    {
      strcpy(response_buffer, "RGM E_USR\n");
    }
  }
  else
  {
    strcpy(response_buffer, "ERR\n");
  }

  sendto(udpfd, response_buffer, strlen(response_buffer), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}

void handleTCPCommand(int connfd, bool verbose)
{
  char command_buffer[MAX_INPUT_SIZE];
  char response_buffer[MAX_INPUT_SIZE];
  bzero(command_buffer, MAX_INPUT_SIZE);

  char op[5];
  bzero(op, sizeof(op));
  read(connfd, op, 4);
  if (op[3] == ' ')
  {
    op[3] = '\0';
  }

  if (!strcmp(op, "ULS"))
  {
    char gid[4];
    bzero(gid, sizeof(gid));

    read(connfd, command_buffer, 3);
    sscanf(command_buffer, "%3[^\n]", gid);

    if (verbose)
    {
      printf("CMD: %s %s\n", op, gid);
    }

    if (parseGID(gid) == -1)
    {
      strcpy(response_buffer, "RUL NOK\n");
    }
    else
    {
      ULS(gid, response_buffer);
    }
  }
  else if (!strcmp(op, "PST"))
  {
    char uid[7], gid[4], tsize[5], text[240];
    bzero(uid, sizeof(uid));
    bzero(gid, sizeof(gid));
    bzero(tsize, sizeof(tsize));
    bzero(text, sizeof(text));

    bzero(command_buffer, sizeof(command_buffer));
    read(connfd, command_buffer, 9);
    sscanf(command_buffer, "%6s %3s", uid, gid);
    bzero(command_buffer, sizeof(command_buffer));
    read(connfd, command_buffer, 4);
    sscanf(command_buffer, "%4s", tsize);

    if (parseUID(uid) == -1 || (parseGID(gid) == -1) || parseTSize(tsize) == -1 || atoi(tsize) < strlen(text))
    {
      strcpy(response_buffer, "RPT NOK\n");
    }
    else
    {
      strcpy(text, &command_buffer[strlen(tsize) + 1]);
      read(connfd, &text[strlen(text)], atoi(tsize) - strlen(text));

      char mid[5];
      char c;
      read(connfd, &c, 1);
      if (c == '\n')
      {
        if (verbose)
        {
          printf("CMD: %s %s %s %s\n", op, uid, gid, tsize);
        }

        if (PST(uid, gid, text, NULL, mid) == NULL)
        {
          strcpy(response_buffer, "RPT NOK\n");
        }
        else
        {
          sprintf(response_buffer, "RPT %s\n", mid);
        }
      }
      else if (c == ' ')
      {
        char fname[26], fsize[12], data[1024];
        bzero(fname, sizeof(fname));
        bzero(fsize, sizeof(fsize));
        bzero(data, sizeof(data));

        bzero(command_buffer, sizeof(command_buffer));
        read(connfd, command_buffer, 36);

        sscanf(command_buffer, "%25s %11s", fname, fsize);

        if (verbose)
        {
          printf("CMD: %s %s %s %s %s %s\n", op, uid, gid, tsize, fname, fsize);
        }

        if (parseFileSize(fsize) == -1 || parseFName(fname) == -1)
        {
          strcpy(response_buffer, "RPT NOK\n");
        }
        else
        {
          strcpy(data, &command_buffer[strlen(fname) + strlen(fsize) + 2]);
          int size_read = 36 - (strlen(fname) + strlen(fsize) + 2);

          FILE *FPtr;
          if ((FPtr = PST(uid, gid, text, fname, mid)) == NULL)
          {
            strcpy(response_buffer, "RPT NOK\n");
          }
          else
          {
            WriteFile(FPtr, data, size_read);
            int n, fsize_int = atoi(fsize);
            bzero(data, sizeof(data));
            while ((size_read < fsize_int) && (n = read(connfd, data, sizeof(data))) > 0)
            {
              // printf("fsize: %d size_read: %d\n", fsize, size_read);
              size_read += n;
              WriteFile(FPtr, data, n);
              bzero(data, sizeof(data));
            }
            fclose(FPtr);
            sprintf(response_buffer, "RPT %s\n", mid);
          }
        }
      }
      else
      {
        strcpy(response_buffer, "RPT NOK\n");
      }
    }
  }
  else if (!strcmp(op, "RTV"))
  {
    int n_msg = 0;

    char uid[7], gid[4], mid[6];
    bzero(uid, sizeof(uid));
    bzero(gid, sizeof(gid));
    bzero(mid, sizeof(mid));

    bzero(command_buffer, sizeof(command_buffer));
    read(connfd, command_buffer, 14);

    sscanf(command_buffer, "%6s %3s %5s", uid, gid, mid);

    if (verbose)
    {
      printf("CMD: %s %s %s %s\n", op, uid, gid, mid);
    }

    if (parseUID(uid) == -1 || parseGID(gid) == -1 || parseMID(mid) == -1 || (n_msg = RTV(uid, gid, mid)) == -1)
    {
      strcpy(response_buffer, "RRT NOK\n");
    }
    else if (n_msg == 0)
    {
      strcpy(response_buffer, "RRT EOF\n");
    }
    else
    {
      int base_msg = atoi(mid);
      sprintf(response_buffer, "RRT OK %d", n_msg);
      write(connfd, response_buffer, strlen(response_buffer));
      for (int i = 0; i < n_msg; i++)
      {
        bzero(response_buffer, MAX_INPUT_SIZE);
        FILE *FPtr = RTVAux(uid, base_msg + i, response_buffer);
        write(connfd, response_buffer, strlen(response_buffer));
        if (FPtr != NULL)
        {
          char data[1024];
          int bytes_read;
          while ((bytes_read = ReadFile(FPtr, data, 1024)) > 0)
          {
            if (write(connfd, data, bytes_read) == -1)
            {
              close(connfd);
              fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
            }
            bzero(data, 1024);
          }
          fclose(FPtr);
        }
      }
      return;
    }
  }
  else
  {
    strcpy(response_buffer, "ERR\n");
  }

  write(connfd, response_buffer, strlen(response_buffer));
}

#endif //RC_PROJECT_SERVER_COMMAND_HANDLING_H_