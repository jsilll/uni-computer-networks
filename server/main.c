#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <signal.h>
#include "parsing.h"
#include "connection.h"
#include "command_handling.h"

#define DEFAULT_PORT "58006"

char PORT[6];
bool VERBOSE;
int TCPFD, UDPFD;
fd_set RSET;

void exitClient(int signal_num);
void loadInitArgs(int argc, char *argv[]);
void readCommand();

int main(int argc, char *argv[])
{
  signal(SIGINT, exitClient);

  strcpy(PORT, DEFAULT_PORT);
  VERBOSE = false;
  loadInitArgs(argc, argv);

  printf("Centralized Messaging Server\n");
  printf("PORT %s VERBOSE %s\n", PORT, VERBOSE ? "On" : "Off");

  setupAddresses(PORT);
  UDPFD = openSocket(SOCK_DGRAM);
  TCPFD = openSocket(SOCK_STREAM);
  if (listen(TCPFD, 5) == -1)
  {
    fprintf(stderr, "Error on listen(TCPFD)\n");
    exit(EXIT_FAILURE);
  }

  FD_ZERO(&RSET);
  for (;;)
  {
    readCommand();
  }
}

/**
 * @brief Signal Handler for terminating the server
 * 
 * @param signal_num
 */
void exitClient(int signal_num)
{
  close(TCPFD);
  close(UDPFD);
  freeAddresses();
  exit(signal_num);
}

/**
 * @brief Parses the initial arguments for the program.
 * 
 * @param argc number of arguments in argv
 * @param argv array passed arguments
 */
void loadInitArgs(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, ":vp:")) != -1)
  {
    switch (opt)
    {
    case 'p':
      parsePortArg(optarg);
      strcpy(PORT, optarg);
      break;
    case 'v':
      VERBOSE = true;
      break;
    case ':':
      fprintf(stderr, "Missing argument for port (-p) option\n");
      exit(EXIT_FAILURE);
    case '?':
      fprintf(stderr, "Unknown option: -%c\n", optopt);
      exit(EXIT_FAILURE);
    default:
      fprintf(stderr, "Unknown error\n");
      exit(EXIT_FAILURE);
    }
  }

  if (optind < argc)
  {
    fprintf(stderr, "Unnecessary extra argument: %s\n", argv[optind]);
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Reads commands from udp and tcp clients
 * 
 */
void readCommand()
{
  FD_SET(TCPFD, &RSET);
  FD_SET(UDPFD, &RSET);
  select(((TCPFD > UDPFD) ? TCPFD : UDPFD) + 1, &RSET, NULL, NULL, NULL);

  struct sockaddr_in cliaddr;
  if (FD_ISSET(TCPFD, &RSET))
  {
    socklen_t len = sizeof(cliaddr);
    int connfd = accept(TCPFD, (struct sockaddr *)&cliaddr, &len);

    if (fork() != 0)
    {
      close(connfd);
    }
    else
    {
      close(TCPFD);
      close(UDPFD);

      // TODO
      // struct timeval tmout;
      // memset((char *)&tmout, 0, sizeof(tmout));
      // tmout.tv_sec = 15;
      // if (setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) < 0)
      // {
      //   fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
      //   exit(EXIT_FAILURE);
      // }

      // TODO
      // memset((char *)&tmout, 0, sizeof(tmout));
      // tmout.tv_sec = 15;
      // if (setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)) < 0)
      // {
      //   fprintf(stderr, "setsockopt(SO_RCVTIMEO) failed.\n");
      //   exit(EXIT_FAILURE);
      // }

      if (VERBOSE)
      {
        printf("[TCP] IP: %s PORT: %lu ", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
      }

      handleTCPCommand(connfd, VERBOSE);
      close(connfd);
      exit(EXIT_SUCCESS);
    }
  }

  if (FD_ISSET(UDPFD, &RSET))
  {
    handleCommandUDP(UDPFD, cliaddr, VERBOSE);
  }
}