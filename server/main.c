#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>


#include "parsing.h"
#include "commands.h"

#define DEFAULT_PORT "58006"
#define MAXLINE 1024

char PORT[MAXLINE];
bool VERBOSE = false;
int listenfd, connfd, udpfd;
fd_set rset;
struct addrinfo *address_tcp, *address_udp;
char response_buffer[MAXLINE];

void signal_handler(int signal_num);
void loadInitArgs(int argc, char* argv[]);
void setupAddresses();
void setupTCPSocket();
void setupUDPSocket();
void readCommand();
void tcpRequestHandler();
void parseCommandUDP(char *command);
void parseCommandTCP(char *command);

int main(int argc, char* argv[]) {
  signal(SIGTERM, signal_handler);

  strcpy(PORT, DEFAULT_PORT);
  loadInitArgs(argc, argv);
  printf("Centralized Messaging Server Initialized\n");
  printf("PORT: %s VERBOSE: %d\n", PORT, VERBOSE);

  setupAddresses();
  setupTCPSocket();
  setupUDPSocket();

  FD_ZERO(&rset);
  for (;;) {
    readCommand();
  }
}


void signal_handler(int signal_num) {
  close(listenfd);
  close(udpfd);
  printf("Signal Handler\n");
  exit(signal_num);
}

/**
 * Parses the initial arguments for the program.
 * @param argc number of arguments in argv
 * @param argv array passed arguments
 */
void loadInitArgs(int argc, char* argv[])
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

void setupAddresses() {
  struct addrinfo hints;
  int errcode;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP Socket
  hints.ai_flags = AI_PASSIVE;


  if ((errcode = getaddrinfo(NULL, PORT, &hints, &address_tcp)) != 0) {
    fprintf(stderr, "Error on getaddrinfo (tcp): %s\n", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }

  hints.ai_socktype = SOCK_DGRAM; // TCP Socket
  if ((errcode = getaddrinfo(NULL, PORT, &hints, &address_udp)) != 0) {
    fprintf(stderr, "Error on getaddrinfo (udp): %s", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }
}

void setupTCPSocket() {
  /* Create listening TCP socket */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  // Binding server addr structure to listenfd
  if (bind(listenfd, address_tcp->ai_addr, address_tcp->ai_addrlen)== -1) {
    fprintf(stderr, "Error binding TCP socket\n");
    exit(EXIT_FAILURE);
  }
  listen(listenfd, 5);
}

void setupUDPSocket() {
  /* Create UDP socket */
  udpfd = socket(AF_INET, SOCK_DGRAM, 0);
  // binding server addr structure to udp sockfd
  if (bind(udpfd, address_udp->ai_addr, address_udp->ai_addrlen) == -1) {
    fprintf(stderr, "Error binding UDP socket\n");
    exit(EXIT_FAILURE);
  }
}

void readCommand() {
  struct sockaddr_in cliaddr;

  // Set listenfd and udpfd in readset
  FD_SET(listenfd, &rset);
  FD_SET(udpfd, &rset);

  // Select the ready descriptor
  select(fmax(listenfd, udpfd) + 1, &rset, NULL, NULL, NULL);

  // If tcp socket is readable then handle it by accepting the connection
  if (FD_ISSET(listenfd, &rset)) {

    socklen_t len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);

    if (fork() == 0) {
      tcpRequestHandler();
      exit(EXIT_SUCCESS);
    }

    close(connfd);
  }

  // If udp socket is readable receive the message.
  if (FD_ISSET(udpfd, &rset)) {
    char buffer[MAXLINE];
    socklen_t len = sizeof(cliaddr);
    bzero(buffer, MAXLINE);
    if (recvfrom(udpfd, buffer, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len) == -1) {
      fprintf(stderr, "Error reading from UDP socket\n");
      // exit(EXIT_FAILURE); TODO
    }
    printf("Message from UDP client: %s", buffer);
    parseCommandUDP(buffer);
    if (sendto(udpfd, response_buffer, strlen(response_buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) == -1) {
      fprintf(stderr, "Error writing to UDP socket\n");
      // exit(EXIT_FAILURE); TODO
    }
  }
}

void tcpRequestHandler() {
  char buffer[MAXLINE];
  if (close(listenfd) == -1) {
    fprintf(stderr, "Error closing parent's TCP socket");
    exit(EXIT_FAILURE);
  }
  if (close(udpfd) == -1) {
    fprintf(stderr, "Error closing parent's UDP socket");
    exit(EXIT_FAILURE);
  }

  bzero(buffer, MAXLINE);

  if (read(connfd, buffer, MAXLINE) == -1) { // TODO, varios reads
    fprintf(stderr, "Error reading from TCP socket");
    exit(EXIT_FAILURE);
  }

  printf("Message From TCP client:  %s", buffer);
  parseCommandTCP(buffer);

  if (write(connfd, response_buffer, strlen(response_buffer)) == -1) {
    fprintf(stderr, "Error writing to TCP socket");
    exit(EXIT_FAILURE);
  }

  if (close(connfd) == -1) {
    fprintf(stderr, "Error closing child's TCP socket");
    exit(EXIT_FAILURE);
  }

  printf("TCP process killed\n"); // DEBUG TODO
}

void parseCommandUDP(char *command) {
  char op[MAXLINE], arg1[MAXLINE], arg2[MAXLINE], arg3[MAXLINE], arg4[MAXLINE], arg5[MAXLINE], arg6[MAXLINE], arg7[MAXLINE];
  sscanf(command, "%s %s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

  if (!strcmp(op, "REG")) {
    // RRG NOK
    if (REG(arg1, arg2)) strcpy(response_buffer, "RRG DUP\n");
    else strcpy(response_buffer, "RRG OK\n");
  }
  if (!strcmp(op, "UNR")) {
    // UID pass
    UNR(arg1, arg2);
  }
  if (!strcmp(op, "LOG")) {
    // UID pass
    LOG(arg1, arg2);
  }
  if (!strcmp(op, "OUT")) {
    // UID pass
    OUT(arg1, arg2);
  }
  if (!strcmp(op, "GLS")) {
    GLS();
  }
  if (!strcmp(op, "GSR")) {
    // UID GID GName
    GSR(arg1, arg2, arg3);
  }
  if (!strcmp(op, "GUR")) {
    // UID GID
    GUR(arg1, arg2);
  }
  if (!strcmp(op, "GLM")) {
    // UID
    GLM(arg1);
  }

}

void parseCommandTCP(char *command) {
  char op[MAXLINE], arg1[MAXLINE], arg2[MAXLINE], arg3[MAXLINE], arg4[MAXLINE], arg5[MAXLINE], arg6[MAXLINE], arg7[MAXLINE];
  int numTokens = sscanf(command, "%s %s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

  if (!strcmp(op, "ULS")) {
    ULS(arg1);
    // GID
  }
  if (!strcmp(op, "PST")) {
    // TODO usar numTokens
    // UID GID Tsize text [Fname Fsize data]
    PST(arg1, arg2, atoi(arg3), arg4, arg5, atoi(arg6), arg7);
  }
  if (!strcmp(op, "RTV")) {
    // UID GID MID
    RTV(arg1, arg2, arg3);
  }

}

