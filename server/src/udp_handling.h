#ifndef UDP_HANDLING_H_
#define UDP_HANDLING_H_

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
 * @brief Handles incoming commands from a udp connection
 *
 * @param udpfd
 * @param cliaddr
 * @param verbose
 */
void handleCommandUDP(int udpfd, struct sockaddr_in cliaddr, bool verbose)
{
  socklen_t len = sizeof(cliaddr);
  char command_buffer[38], response_buffer[3274];

  bzero(command_buffer, sizeof(command_buffer));
  if (recvfrom(udpfd, command_buffer, sizeof(command_buffer), 0, (struct sockaddr *)&cliaddr, &len) == -1)
  {
    if (verbose)
    {
      fprintf(stderr, "[ERROR] recvfrom(udpfd)\n");
    }
    return;
  }

  if (verbose)
  {
    printf("[UDP] IP: %s PORT: %05u ", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    printf("CMD: %s", command_buffer);
  }

  char op[5], arg1[7], arg2[10], arg3[26], arg4[2];
  int numTokens = sscanf(command_buffer, "%4s %6s %9s %25s %1s", op, arg1, arg2, arg3, arg4);
  if (numTokens > 4)
  {
    strcpy(response_buffer, "ERR\n");
  }
  else if (!strcmp(op, "REG"))
  {
    if (parseUID(arg1) == -1 || parsePassword(arg2) == -1)
    {
      strcpy(response_buffer, "RRG NOK\n");
    }
    else if (registerUser(arg1, arg2) == -1)
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
    if (parseUID(arg1) == -1 || parsePassword(arg2) == -1 || unregisterUser(arg1, arg2) == -1)
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
    if (parseUID(arg1) == -1 || parsePassword(arg2) == -1 || login(arg1, arg2) == -1)
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
    if (parseUID(arg1) == -1 || parsePassword(arg2) == -1 || logout(arg1, arg2) == -1)
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
    groups(response_buffer);
  }
  else if (!strcmp(op, "GSR"))
  {
    if (parseUID(arg1) == -1)
    {
      strcpy(response_buffer, "RGS E_USR\n");
    }
    else if (parseGID(arg2) == -1 && strcmp(arg2, "00"))
    {
      strcpy(response_buffer, "RGS E_GRP\n");
    }
    else if (parseGName(arg3) == -1)
    {
      strcpy(response_buffer, "RGS E_GNAME\n");
    }
    else
    {
      int res;
      switch (res = subscribe(arg1, arg2, arg3))
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
    else if (unsubscribe(arg1, arg2) == -1)
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
    if (parseUID(arg1) == -1 || myGroups(arg1, response_buffer) == -1)
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

#endif // UDP_HANDLING_H_