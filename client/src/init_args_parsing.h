#ifndef INIT_ARGS_PARSING_H_
#define INIT_ARGS_PARSING_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interface.h"

/**
 * @brief Validates the ip option argument.
 *
 * @param ip ip argument in string format
 */
void parseIPArg(char *ip)
{
  if (!inet_pton(AF_INET, ip, NULL))
  {
    fprintf(stderr, MSG_NOT_IP);
  }
  // exit(EXIT_FAILURE)
}

/**
 * @brief Validates the port option argument.
 *
 * @param port port argument in string format
 */
void parsePortArg(char *port)
{
  for (int i = 0; i < strlen(port); i++)
  {
    if (port[i] != '0')
    {
      long port_parsed = strtol(port, NULL, 10);
      if (port_parsed <= 0 || port_parsed > 65535)
      {
        fprintf(stderr, ERR_INVALID_PORT);
        exit(EXIT_FAILURE);
      }
      return;
    }
  }
}

#endif //INIT_ARGS_PARSING_H_
