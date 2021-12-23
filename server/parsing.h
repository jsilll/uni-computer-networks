#ifndef RC_PROJECT_SERVER_PARSING_H_
#define RC_PROJECT_SERVER_PARSING_H_

#include <stdio.h>

/**
 * Validates the port option argument.
 * @param port port argument in string format
 */
void parsePortArg(char* port)
{
  for (int i = 0; i < strlen(port); i++)
  {
    if (port[i] != '0')
    {
      char* ptr;
      int port_parsed = strtol(port, &ptr, 10);
      if (port_parsed <= 0 || port_parsed > 65535)
      {
        fprintf(stderr, "Invalid value for port argument\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#endif //RC_PROJECT_SERVER_PARSING_H_
