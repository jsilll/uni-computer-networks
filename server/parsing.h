#ifndef RC_PROJECT_SERVER_PARSING_H_
#define RC_PROJECT_SERVER_PARSING_H_

#include <stdio.h>

/**
 * Validates the port option argument.
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
        fprintf(stderr, "Invalid port argument\n");
        exit(EXIT_FAILURE);
      }
      return;
    }
  }
}

#endif //RC_PROJECT_SERVER_PARSING_H_
