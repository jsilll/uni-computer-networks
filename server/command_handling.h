#ifndef RC_PROJECT_SERVER_COMMAND_HANDLING_H_
#define RC_PROJECT_SERVER_COMMAND_HANDLING_H_

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
    printf("[UDP] %s", command_buffer);
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
  read(connfd, command_buffer, MAX_INPUT_SIZE); // TODO varios reads?? ficheiro?

  // TODO feio? e nao funciona para o ficheiro
  // while (strtok(command_buffer, ' ') != NULL)) {

  // }

  char op[MAX_INPUT_SIZE] = {'\0'};
  char arg1[MAX_INPUT_SIZE] = {'\0'};
  char arg2[MAX_INPUT_SIZE] = {'\0'};
  char arg3[MAX_INPUT_SIZE] = {'\0'};
  char arg4[MAX_INPUT_SIZE] = {'\0'};
  char arg5[MAX_INPUT_SIZE] = {'\0'};
  char arg6[MAX_INPUT_SIZE] = {'\0'};
  int numTokens = sscanf(command_buffer, "%s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6);
  int index = strlen(op) + strlen(arg1) + strlen(arg2) + strlen(arg3) + strlen(arg4) + strlen(arg5) + strlen(arg6) + 7;
  char *arg7 = &command_buffer[index];

  if (verbose)
  {
    printf("[TCP] %s %s %s %s %s %s %s\n", op, arg1, arg2, arg3, arg4, arg5, arg6);
  }

  if (!strcmp(op, "ULS"))
  {
    if (parseGID(arg1) == -1)
    {
      strcpy(response_buffer, "RUL NOK\n");
    }
    else
    {
      ULS(arg1, response_buffer);
    }
  }
  else if (!strcmp(op, "PST"))
  {
    if (parseUID(arg1) == -1 || (parseGID(arg2) == -1) || parseMessageText(arg4, arg3) == -1)
    {
      strcpy(response_buffer, "RPT NOK\n");
    }
    if (numTokens == 5)
    {
      int n_msg = PST(arg1, arg2, atoi(arg3), arg4, NULL, 0, NULL);
      if (n_msg == -1)
      {
        strcpy(response_buffer, "RPT NOK\n");
      }
      else
      {
        char mid[5];
        sprintf(mid, "%04d", n_msg);
        sprintf(response_buffer, "RPT %s\n", mid);
      }
    }
    else
    {
      if (parseFileSize(arg6) == -1 || parseFName(arg5) == -1)
      {
        strcpy(response_buffer, "RPT NOK\n");
      }
      else
      {
        int size_read = strlen(arg7);
        int n_msg = PST(arg1, arg2, atoi(arg3), arg4, arg5, atoi(arg6), arg7);
        char mid[5];
        sprintf(mid, "%04d", n_msg);
        if (n_msg == -1)
        {
          strcpy(response_buffer, "RPT NOK\n");
        }
        else
        {
          int n, fsize = atoi(arg6);
          char data[1024];
          bzero(data, 1024);
          while (size_read < fsize && (n = read(connfd, data, 1024)) > 0)
          {
            size_read += n;
            PSTAux(arg2, mid, arg5, data);
            bzero(data, 1024);
          }
          sprintf(response_buffer, "RPT %s\n", mid);
        }
      }
    }
  }
  else if (!strcmp(op, "RTV"))
  {
    if (parseUID(arg1) == -1 || parseGID(arg1) == -1 || !parseMID(arg1))
    {
      strcpy(response_buffer, "RRT NOK\n");
    }
    else
    {
      RTV(arg1, arg2, arg3); // TODO
      strcpy(response_buffer, "RRT status [N[ MID UID Tsize text [/ Fname Fsize data]]*]\n");
    }
  }
  else
  {
    strcpy(response_buffer, "ERR\n");
  }

  write(connfd, response_buffer, strlen(response_buffer));
}

#endif //RC_PROJECT_SERVER_COMMAND_HANDLING_H_