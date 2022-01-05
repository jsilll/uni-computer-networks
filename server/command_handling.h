#ifndef RC_PROJECT_SERVER_COMMAND_HANDLING_H_
#define RC_PROJECT_SERVER_COMMAND_HANDLING_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <errno.h>

#include "state/parsing.h"
#include "state/operations.h"

#define MAX_INPUT_SIZE 1024

void handleCommandUDP(int udpfd, struct sockaddr_in cliaddr, bool verbose)
{
  socklen_t len = sizeof(cliaddr);
  char command_buffer[33];
  bzero(command_buffer, sizeof(command_buffer));

  char response_buffer[3175];
  recvfrom(udpfd, command_buffer, sizeof(command_buffer), 0, (struct sockaddr *)&cliaddr, &len);

  if (verbose)
  {
    printf("[UDP] IP: %s PORT: %lu ", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    printf("%s", command_buffer);
  }

  char op[5], uid[7], arg2[10], gname[26], arg4[2];
  bzero(op, sizeof(op));
  bzero(uid, sizeof(uid));
  bzero(arg2, sizeof(arg2));
  bzero(gname, sizeof(gname));
  bzero(arg4, sizeof(arg4));

  int numTokens = sscanf(command_buffer, "%4s %6s %9s %25s %1s", op, uid, arg2, gname, arg4);

  if (numTokens > 4)
  {
    strcpy(response_buffer, "ERR\n");
  }
  else if (!strcmp(op, "REG"))
  {
    if (parseUID(uid) == -1 || parsePassword(arg2) == REG_NOMATCH)
    {
      strcpy(response_buffer, "RRG NOK\n");
    }
    else if (REG(uid, arg2) == -1)
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
    if (parseUID(uid) == -1 || parsePassword(arg2) == REG_NOMATCH || UNR(uid, arg2) == -1)
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
    if (parseUID(uid) == -1 || parsePassword(arg2) == REG_NOMATCH || LOG(uid, arg2) == -1)
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
    if (parseUID(uid) == -1 || parsePassword(arg2) == REG_NOMATCH || OUT(uid, arg2) == -1)
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
    if (parseUID(uid) == -1)
    {
      strcpy(response_buffer, "RGS E_USR\n");
    }
    else if (parseGID(arg2) == -1)
    {
      strcpy(response_buffer, "RGS E_GRP\n");
    }
    else if (parseGName(gname) == REG_NOMATCH)
    {
      strcpy(response_buffer, "RGS E_GNAME\n");
    }
    else
    {
      int res;
      switch (res = GSR(uid, arg2, gname))
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
    if (parseUID(uid) == -1)
    {
      strcpy(response_buffer, "RGU E_USR\n");
    }
    else if (parseGID(arg2) == -1)
    {
      strcpy(response_buffer, "RGU E_GRP\n");
    }
    else if (GUR(uid, arg2) == -1)
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
    if (parseUID(uid) == -1 || GLM(uid, response_buffer) == -1)
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
  char command_buffer[36];
  char buffer[1024];
  bzero(buffer, sizeof(buffer));
  bzero(command_buffer, sizeof(command_buffer));

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
      strcpy(buffer, "RUL NOK\n");
    }
    else
    {
      ULS(gid, buffer);
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
      strcpy(buffer, "RPT NOK\n");
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
          strcpy(buffer, "RPT NOK\n");
        }
        else
        {
          sprintf(buffer, "RPT %s\n", mid);
        }
      }
      else if (c == ' ')
      {
        char fname[26], fsize[12];
        bzero(fname, sizeof(fname));
        bzero(fsize, sizeof(fsize));
        bzero(buffer, sizeof(buffer));

        bzero(command_buffer, sizeof(command_buffer));
        read(connfd, command_buffer, 36);

        sscanf(command_buffer, "%25s %11s", fname, fsize);

        if (verbose)
        {
          printf("CMD: %s %s %s %s %s %s\n", op, uid, gid, tsize, fname, fsize);
        }

        if (parseFileSize(fsize) == -1 || parseFName(fname) == -1)
        {
          printf("%s %s\n", fsize, fname);
          strcpy(buffer, "RPT NOK\n");
        }
        else
        {
          int size_read = 36 - (strlen(fname) + strlen(fsize) + 2);

          FILE *FPtr;
          if ((FPtr = PST(uid, gid, text, fname, mid)) == NULL)
          {
            strcpy(buffer, "RPT NOK\n");
          }
          else
          {
            WriteFile(FPtr, &command_buffer[strlen(fname) + strlen(fsize) + 2], size_read);
            int n, fsize_int = atoi(fsize);
            bzero(buffer, sizeof(buffer));
            while ((size_read < fsize_int) && (n = read(connfd, buffer, sizeof(buffer))) > 0)
            {
              size_read += n;
              WriteFile(FPtr, buffer, n);
              bzero(buffer, sizeof(buffer));
            }
            fclose(FPtr);
            sprintf(buffer, "RPT %s\n", mid);
          }
        }
      }
      else
      {
        strcpy(buffer, "RPT NOK\n");
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
      strcpy(buffer, "RRT NOK\n");
    }
    else if (n_msg == 0)
    {
      strcpy(buffer, "RRT EOF\n");
    }
    else
    {

      int base_msg = atoi(mid);
      sprintf(buffer, "RRT OK %d", n_msg);
      write(connfd, buffer, strlen(buffer));
      for (int i = 0; i < n_msg; i++)
      {
        FILE *FPtr = RTVAux(gid, base_msg + i, buffer);
        write(connfd, buffer, strlen(buffer));
        if (FPtr != NULL)
        {
          int bytes_read;
          bzero(buffer, sizeof(buffer));
          while ((bytes_read = ReadFile(FPtr, buffer, 1024)) > 0)
          {
            if (write(connfd, buffer, bytes_read) == -1)
            {
              close(connfd);
              fprintf(stderr, "Couldn't send command_buffer. Error sending command_buffer.\n");
            }
            bzero(buffer, sizeof(buffer));
          }
          fclose(FPtr);
        }
      }
      return;
    }
  }
  else
  {
    strcpy(buffer, "ERR\n");
  }

  write(connfd, buffer, strlen(buffer));
}

#endif //RC_PROJECT_SERVER_COMMAND_HANDLING_H_