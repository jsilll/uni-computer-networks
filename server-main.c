#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define GROUP_NUMBER 6
#define BOOL_TO_STR(b) (b ? "On" : "Off")

unsigned short int PORT = 58000 + GROUP_NUMBER;
bool VERBOSE = false;

/* Argument Parsing Functions */
void loadInitArgs(int argc, char* argv[]);
unsigned short int parsePortArg(char* port);

int main(int argc, char* argv[])
{
  loadInitArgs(argc, argv);
  printf("Centralized Messaging Server Initialized\n");
  printf("PORT: %d VERBOSE: %s\n", PORT, BOOL_TO_STR(VERBOSE));

  int fd, errcode;
  struct addrinfo hints, * res;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  char buffer[128];

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) /* error */ exit(1);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_DGRAM; // UDP Socket
  hints.ai_flags = AI_PASSIVE;

  char PORT_BUFFER[128];
  sprintf(PORT_BUFFER, "%d", PORT);
  errcode = getaddrinfo(NULL, PORT_BUFFER, &hints, &res);
  if (errcode != 0) /* error */ exit(1);

  ssize_t n = bind(fd, res->ai_addr, res->ai_addrlen);
  if (n == -1) /* error */ exit(1);

  while (1)
  {
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) /* error */ exit(1);

    printf("%s", buffer);
    // n = sendto(fd, buffer, n, 0,(struct sockaddr*)&addr, addrlen);
    // if (n == -1) /* error */ exit(1);
  }

  // TODO SIGINT Handler
  // freeaddrinfo(res);
  // close(fd);
}

/*
 * Parses the initial arguments for the program.
 * Input:
 *  - argc: number of arguments in argv
 *  - argv: array passed arguments
 */
void loadInitArgs(int argc, char* argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, ":vp:")) != -1)
  {
    switch (opt)
    {
      case 'p':
        PORT = parsePortArg(optarg);
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

/*
 * Validates the port option argument.
 * Input:
 *  - port: port argument in string format
 */
unsigned short int parsePortArg(char* port)
{
  for (int i = 0; i < strlen(port); i++)
  {
    if (port[i] != '0')
    {
      char* ptr;
      int port_parsed = strtol(port, &ptr, 10);
      if (port_parsed > 0 && port_parsed <= 65535)
      {
        return port_parsed;
      }
      else
      {
        fprintf(stderr, "Invalid value for port argument\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  return 0;
}