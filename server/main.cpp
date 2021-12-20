#include <iostream>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>

#define PORT "58006"
#define MAXLINE 1024

using namespace std;

int main() {
  int listenfd, connfd, udpfd;
  fd_set rset;
  ssize_t n;
  socklen_t len;
  const int on = 1;
  struct sockaddr_in cliaddr{}, servaddr{};
  struct addrinfo hints{}, *res;
  char *message;
  [[maybe_unused]] void sig_chld(int);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP Socket
  hints.ai_flags = AI_PASSIVE;
  int errcode = getaddrinfo(nullptr, PORT, &hints, &res);
  if (errcode != 0) /* error */ exit(1);

  /* Create listening TCP socket */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  // Binding server addr structure to listenfd
  n = bind(listenfd, res->ai_addr, res->ai_addrlen);
  if (n == -1) /* error */ exit(1);
  listen(listenfd, 5);

  /* Create UDP socket */
  udpfd = socket(AF_INET, SOCK_DGRAM, 0);
  // binding server addr structure to udp sockfd
  n = bind(udpfd, res->ai_addr, res->ai_addrlen);
  if (n == -1) /* error */ exit(1);

  // Clear the descriptor set
  FD_ZERO(&rset);

  for (;;) {

    // Set listenfd and udpfd in readset
    FD_SET(listenfd, &rset);
    FD_SET(udpfd, &rset);

    // Select the ready descriptor
    select(max(listenfd, udpfd) + 1, &rset, NULL, NULL, NULL);

    char buffer[MAXLINE];

    // If tcp socket is readable then handle it by accepting the connection
    if (FD_ISSET(listenfd, &rset)) {
      len = sizeof(cliaddr);
      connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);

      if (fork() == 0) {
        close(listenfd);
        printf("Message From TCP client: ");
        bzero(buffer, sizeof(buffer));
        read(connfd, buffer, MAXLINE); // TODO, varios reads
        printf("%s", buffer);
        write(connfd, buffer, strlen(buffer));
        close(connfd);
        printf("TCP process killed\n");
        exit(0);
      }
      close(connfd);
    }

    // If udp socket is readable receive the message.
    if (FD_ISSET(udpfd, &rset)) {
      len = sizeof(cliaddr);
      bzero(buffer, sizeof(buffer));
      printf("Message from UDP client: ");
      n = recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &len);
      if (n == -1) /* error */ exit(1);
      printf("%s\n", buffer);
      sendto(udpfd, (const char *) message, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    }
  }
}