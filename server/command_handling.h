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
      switch (res = GSR(arg1, arg2, arg3)) {
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
    else
    {
      GUR(arg1, arg2);
      strcpy(response_buffer, "RGU OK\n");
    }
  }
  else if (!strcmp(op, "GLM"))
  {
    if (parseUID(arg1) == -1 || GLM(arg1, response_buffer) == -1)
    {
      strcpy(response_buffer, "RGM E_USR\n");
    }
  } else {
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

  if (verbose)
  {
    printf("[TCP] %s", command_buffer);
  }

  // TODO feio? e nao funciona para o ficheiro
  char op[MAX_INPUT_SIZE], arg1[MAX_INPUT_SIZE], arg2[MAX_INPUT_SIZE], arg3[MAX_INPUT_SIZE], arg4[MAX_INPUT_SIZE],
      arg5[MAX_INPUT_SIZE], arg6[MAX_INPUT_SIZE],
      arg7[MAX_INPUT_SIZE];
  int numTokens = sscanf(command_buffer, "%s %s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

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
    // TODO usar numTokens
    // UID GID Tsize text [Fname Fsize data]
    PST(arg1, arg2, atoi(arg3), arg4, arg5, atoi(arg6), arg7);
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
  } else {
    strcpy(response_buffer, "ERR\n");
  }

  write(connfd, response_buffer, strlen(response_buffer));
}

#endif //RC_PROJECT_SERVER_COMMAND_HANDLING_H_
