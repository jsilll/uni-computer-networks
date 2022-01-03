#ifndef RC_PROJECT_SERVER_CONNECTION_H_
#define RC_PROJECT_SERVER_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

struct addrinfo *address_tcp, *address_udp;

void setupAddresses(char *PORT)
{
  struct addrinfo hints;
  int errcode;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP Socket
  hints.ai_flags = AI_PASSIVE;

  if ((errcode = getaddrinfo(NULL, PORT, &hints, &address_tcp)) != 0)
  {
    fprintf(stderr, "Error on getaddrinfo (tcp): %s\n", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }

  hints.ai_socktype = SOCK_DGRAM; // TCP Socket
  if ((errcode = getaddrinfo(NULL, PORT, &hints, &address_udp)) != 0)
  {
    fprintf(stderr, "Error on getaddrinfo (udp): %s", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }
}

int openSocket(int type)
{
  int fd;

  if ((fd = socket(AF_INET, type, 0)) < 0)
  {
    fprintf(stderr, "Error opening socket.\n");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
  {
    fprintf(stderr, "setsockopt(SO_REUSEADDR) failed.\n");
    exit(EXIT_FAILURE);
  }

  switch (type)
  {
  case SOCK_DGRAM:
    if (bind(fd, address_udp->ai_addr, address_udp->ai_addrlen) == -1)
    {
      fprintf(stderr, "Error binding socket\n");
      exit(EXIT_FAILURE);
    }
    break;

  case SOCK_STREAM:
    if (bind(fd, address_tcp->ai_addr, address_tcp->ai_addrlen) == -1)
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
