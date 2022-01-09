#ifndef RC_PROJECT_SERVER_CONNECTION_H_
#define RC_PROJECT_SERVER_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

struct addrinfo *ADDR_TCP, *ADDR_UDP; // TODO put in connection.c

/**
 * @brief Sets up the server addresses for 
 * establishing TCP and UDP connections
 * 
 * @param PORT 
 */
void setupAddresses(char *PORT)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int errcode;
  if ((errcode = getaddrinfo(NULL, PORT, &hints, &ADDR_TCP)) != 0)
  {
    fprintf(stderr, "Error on getaddrinfo (tcp): %s\n", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }

  hints.ai_socktype = SOCK_DGRAM;
  if ((errcode = getaddrinfo(NULL, PORT, &hints, &ADDR_UDP)) != 0)
  {
    fprintf(stderr, "Error on getaddrinfo (udp): %s", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Frees the server addresses
 * 
 */
void freeAddresses()
{
  free(ADDR_TCP);
  free(ADDR_UDP);
}

/**
 * @brief Opens a socket
 * 
 * @param socktype 
 * @return int 
 */
int openSocket(int socktype)
{
  int fd;
  if ((fd = socket(AF_INET, socktype, 0)) < 0)
  {
    fprintf(stderr, "Error opening socket.\n");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
  {
    fprintf(stderr, "setsockopt(SO_REUSEADDR) failed.\n");
    exit(EXIT_FAILURE);
  }

  switch (socktype)
  {
  case SOCK_DGRAM:
    if (bind(fd, ADDR_UDP->ai_addr, ADDR_UDP->ai_addrlen) == -1)
    {
      fprintf(stderr, "Error binding socket\n");
      exit(EXIT_FAILURE);
    }
    break;

  case SOCK_STREAM:
    if (bind(fd, ADDR_TCP->ai_addr, ADDR_TCP->ai_addrlen) == -1)
    {
      fprintf(stderr, "Error binding socket\n");
      exit(EXIT_FAILURE);
    }
    break;

  default:
    fd = -1;
  }

  return fd;
}

#endif //RC_PROJECT_SERVER_CONNECTION_H_
