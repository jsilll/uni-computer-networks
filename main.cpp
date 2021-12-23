#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <csignal>
#include <vector>
#include <sstream>

#include "server/parsing.h"
#include "server/commands.h"

#define DEFAULT_PORT "58006"
#define MAXLINE 1024

using namespace std;

char PORT[MAXLINE];
bool VERBOSE = false;
int listenfd, connfd, udpfd;
fd_set rset;
struct addrinfo *address_tcp, *address_udp;

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
        cerr << "Missing argument for port (-p) option" << endl;
        exit(EXIT_FAILURE);
      case '?':
        cerr << "Unknown option: -" << optopt << endl;
        exit(EXIT_FAILURE);
      default:
        cerr << "Unknown error" << endl;
        exit(EXIT_FAILURE);
    }
  }

  if (optind < argc)
  {
    cerr << "Unnecessary extra argument: " << argv[optind] << endl;
    exit(EXIT_FAILURE);
  }
}

void setupAddresses() {
  struct addrinfo hints{};
  int errcode;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP Socket
  hints.ai_flags = AI_PASSIVE;


  if ((errcode = getaddrinfo(nullptr, PORT, &hints, &address_tcp)) != 0) {
    cerr << "Error on getaddrinfo (tcp): " << gai_strerror(errcode) << endl;
    exit(EXIT_FAILURE);
  }

  hints.ai_socktype = SOCK_DGRAM; // TCP Socket
  if ((errcode = getaddrinfo(nullptr, PORT, &hints, &address_udp)) != 0) {
    cerr << "Error on getaddrinfo (udp): " << gai_strerror(errcode) << endl;
    exit(EXIT_FAILURE);
  }
}

void setupTCPSocket() {
  /* Create listening TCP socket */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  // Binding server addr structure to listenfd
  if (bind(listenfd, address_tcp->ai_addr, address_tcp->ai_addrlen)== -1) {
    cerr << "Error binding TCP socket" << endl;
    exit(EXIT_FAILURE);
  }
  listen(listenfd, 5);
}

void setupUDPSocket() {
  /* Create UDP socket */
  udpfd = socket(AF_INET, SOCK_DGRAM, 0);
  // binding server addr structure to udp sockfd
  if (bind(udpfd, address_udp->ai_addr, address_udp->ai_addrlen) == -1) {
    cerr << "Error binding UDP socket" << endl;
    exit(EXIT_FAILURE);
  }
}

void readCommand() {
  struct sockaddr_in cliaddr{};

  // Set listenfd and udpfd in readset
  FD_SET(listenfd, &rset);
  FD_SET(udpfd, &rset);

  // Select the ready descriptor
  select(max(listenfd, udpfd) + 1, &rset, nullptr, nullptr, nullptr);

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
      cerr << "Error reading from UDP socket" << endl;
      // exit(EXIT_FAILURE); TODO
    }
    printf("Message from UDP client: %s", buffer);
    parseCommandUDP(buffer);
    if (sendto(udpfd, (const char *) buffer, strlen(buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) == -1) {
      cerr << "Error writing to UDP socket" << endl;
      // exit(EXIT_FAILURE); TODO
    }
  }
}

void tcpRequestHandler() {
  char buffer[MAXLINE];
  if (close(listenfd) == -1) {
    cerr << "Error closing parent's TCP socket" << endl;
    exit(EXIT_FAILURE);
  }
  if (close(udpfd) == -1) {
    cerr << "Error closing parent's UDP socket" << endl;
    exit(EXIT_FAILURE);
  }

  bzero(buffer, MAXLINE);

  if (read(connfd, buffer, MAXLINE) == -1) { // TODO, varios reads
    cerr << "Error reading from TCP socket" << endl;
    exit(EXIT_FAILURE);
  }

  printf("Message From TCP client:  %s", buffer);
  parseCommandTCP(buffer);

  if (write(connfd, buffer, strlen(buffer)) == -1) {
    cerr << "Error writing to TCP socket" << endl;
    exit(EXIT_FAILURE);
  }

  if (close(connfd) == -1) {
    cerr << "Error closing child's TCP socket" << endl;
    exit(EXIT_FAILURE);
  }

  printf("TCP process killed\n"); // DEBUG TODO
}

void parseCommandUDP(char *command) {
  char op[MAXLINE], arg1[MAXLINE], arg2[MAXLINE], arg3[MAXLINE], arg4[MAXLINE], arg5[MAXLINE], arg6[MAXLINE], arg7[MAXLINE];
  sscanf(command, "%s %s %s %s %s %s %s %s", op, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

  if (!strcmp(op, "REG")) {
    // UID pass
    REG(arg1, arg2);
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