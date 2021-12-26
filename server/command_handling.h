#ifndef RC_PROJECT_SERVER_COMMAND_HANDLING_H_
#define RC_PROJECT_SERVER_COMMAND_HANDLING_H_

#include "state/parsing.h"
#include "state/operations.h"

#define MAX_INPUT_SIZE 1024

void handleCommandUDP(int udpfd, struct sockaddr_in cliaddr, bool verbose)
{
  socklen_t len = sizeof(cliaddr);
  char command_buffer[MAX_INPUT_SIZE];
  char response_buffer[MAX_INPUT_SIZE];
  bzero(command_buffer, MAX_INPUT_SIZE);
  recvfrom(udpfd, command_buffer, MAX_INPUT_SIZE, 0, (struct sockaddr *)&cliaddr, &len);

  printf("[UDP] %s", command_buffer);

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
    else if (REG(arg1, arg2))
    {
      strcpy(response_buffer, "RRG DUP\n"); // TODO NOK case, too many users registered
    }
    else
    {
      strcpy(response_buffer, "RRG OK\n");
    }
  }
  else if (!strcmp(op, "UNR"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH)
    {
      strcpy(response_buffer, "RRG NOK\n");
    }
    else
    {
      UNR(arg1, arg2); // TODO
      strcpy(response_buffer, "RRG OK\n");
    }
  }
  else if (!strcmp(op, "LOG"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH)
    { // ROU OK
      strcpy(response_buffer, "RRG NOK\n");
    }
    LOG(arg1, arg2); // TODO
    strcpy(response_buffer, "ROL OK\n");
  }
  else if (!strcmp(op, "OUT"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == REG_NOMATCH)
    { // ROU OK
      strcpy(response_buffer, "ROU NOK\n");
    }
    else
    {
      OUT(arg1, arg2); // TODO
      strcpy(response_buffer, "ROU OK\n");
    }
  }
  else if (!strcmp(op, "GLS"))
  {
    GLS(); // TODO, tem de retornar char*
    strcpy(response_buffer, "RGL N[GID GName MID]*\n");
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
      GSR(arg1, arg2, arg3); // TODO NOK case
      strcpy(response_buffer, "RGS NEW\n");
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
      GUR(arg1, arg2); // TODO NOK CASE
      strcpy(response_buffer, "RGU OK\n");
    }
  }
  else if (!strcmp(op, "GLM"))
  {
    if (parseUID(arg1) == -1)
    {
      strcpy(response_buffer, "RGM E_USR\n");
    }
    else
    {
      GLM(arg1); // TODO
      strcpy(response_buffer, "RGM N[ GID GName MID]*\n");
    }
  }
  sendto(udpfd, response_buffer, strlen(response_buffer), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}

void handleTCPCommand(int connfd, bool verbose)
{
  char command_buffer[MAX_INPUT_SIZE];
  char response_buffer[MAX_INPUT_SIZE];

  bzero(command_buffer, MAX_INPUT_SIZE);
  read(connfd, command_buffer, MAX_INPUT_SIZE); // TODO varios reads?? ficheiro?

  printf("[TCP] %s", command_buffer);

  // TODO feio? e nao funciona para o ficheiro
  char op[MAX_INPUT_SIZE], arg1[MAX_INPUT_SIZE], arg2[MAX_INPUT_SIZE], arg3[MAX_INPUT_SIZE], arg4[MAX_INPUT_SIZE],
      arg5[MAX_INPUT_SIZE], arg6[MAX_INPUT_SIZE],
      arg7[MAX_INPUT_SIZE];
  int numTokens = sscanf(command_buffer, "%s %s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

  if (!strcmp(op, "ULS"))
  {
    if (parseGID(arg1))
    {
      strcpy(response_buffer, "RGM NOK\n");
    }
    else
    {
      ULS(arg1); // TODO NOK
      strcpy(response_buffer, "RGM N[ GID GName MID]*\n");
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
  }

  write(connfd, response_buffer, strlen(response_buffer));
}

#endif //RC_PROJECT_SERVER_COMMAND_HANDLING_H_
