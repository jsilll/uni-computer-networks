#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "parsing.h"
#include "centralized_messaging/commands.h"
#include "centralized_messaging/parsing.h"

#define DEFAULT_PORT "58006"
#define MAX_INPUT_SIZE T_SIZE + 32

char PORT[MAX_INPUT_SIZE], ADDRESS[MAX_INPUT_SIZE];

/* Execution Arguments */
void setDefaultAddress();
void loadInitArgs(int argc, char *argv[]);

/* Command Execution */
void execCommand(char *line);

int main(int argc, char *argv[]) {
  strcpy(PORT, DEFAULT_PORT);
  setDefaultAddress();

  loadInitArgs(argc, argv);

  printf("Centralized Messaging Client Initialized\n");
  printf("PORT: %s ADDRESS: %s\n", PORT, ADDRESS);

  if (setupServerAddress(ADDRESS, PORT) != 0) {
    exit(EXIT_FAILURE);
  }

  char line[MAX_INPUT_SIZE];
  while (fgets(line, sizeof(line) / sizeof(char), stdin))
    execCommand(line);

  // TODO SIGINT Handler (centralized messaging) freeServerAddress();
}

/**
 * @brief Gets the local machine's ADDRESS
 */
void setDefaultAddress() {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_CANONNAME;

  char hostname_buf[128];
  if (gethostname(hostname_buf, 128) == -1) {
    fprintf(stderr, "error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  int errcode;
  struct addrinfo *host_addrinfo;
  if ((errcode = getaddrinfo(hostname_buf, NULL, &hints, &host_addrinfo))) {
    fprintf(stderr, "error: getaddrinfo: %s\n", gai_strerror(errcode));
    exit(EXIT_FAILURE);
  }

  char buffer[INET_ADDRSTRLEN];
  struct in_addr *addr = &((struct sockaddr_in *) host_addrinfo->ai_addr)->sin_addr;
  strcpy(ADDRESS, inet_ntop(host_addrinfo->ai_family, addr, buffer, sizeof buffer));
}

/**
 * @brief Loads the initial arguments given by the
 * user to the program
 *
 * @param argc number of arguments in argv
 * @param argv array passed arguments
 */
void loadInitArgs(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, ":n:p:")) != -1) {
    switch (opt) {
      case 'n': parseIPArg(optarg);
        strcpy(ADDRESS, optarg);
        break;
      case 'p': parsePortArg(optarg);
        strcpy(PORT, optarg);
        break;
      case ':': fprintf(stderr, "Missing value for ip (-n) or for port (-p) option\n");
        exit(EXIT_FAILURE);
      case '?': fprintf(stderr, "Unknown option: -%c\n", optopt);
        exit(EXIT_FAILURE);
      default: fprintf(stderr, "Unknown error\n");
        exit(EXIT_FAILURE);
    }
  }

  if (optind < argc) {
    fprintf(stderr, "Unnecessary extra argument: %s\n", argv[optind]);
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Executes a command represented by a string.
 *
 * @param line string that represents a command
 */
void execCommand(char *line) {
  char op[MAX_INPUT_SIZE] = {'\0'};
  char arg1[MAX_INPUT_SIZE] = {'\0'};
  char arg2[MAX_INPUT_SIZE] = {'\0'};
  char arg3[MAX_INPUT_SIZE] = {'\0'};

  int numTokens = sscanf(line, "%s %s %s %s", op, arg1, arg2, arg3);

  if (!strcmp(op, CMD_POST)) {
    op[0] = arg1[0] = arg2[0] = '\0';
    numTokens = sscanf(line, "%s \"%[^\"]\" %s", op, arg1, arg2);

    if (numTokens < 2) {
      fprintf(stderr, MSG_INVALID_POST_CMD);
      return;
    }

    if ((!strlen(arg2) && line[strlen(line) - 2] != '"') || (parseMessageText(arg1) == -1)) {
      fprintf(stderr, MSG_INVALID_TXT_MSG);
      return;
    }

    if (strlen(arg2)) {
      if (parseFName(arg2) == REG_NOMATCH) {
        fprintf(stderr, MSG_INVALID_FNAME);
        return;
      }
      return post(arg1, arg2);
    }

    return post(arg1, NULL);
  }

  if (numTokens == 4) {
    fprintf(stderr, MSG_UNKNOWN_CMD);
    return;
  }

  switch (numTokens) {
    case 1:
      if (!strcmp(op, CMD_LOGOUT)) {
        return logout();
      } else if (!strcmp(op, CMD_EXIT)) {
        return exitClient();
      } else if (!strcmp(op, CMD_GROUPS) || !strcmp(op, CMD_GROUPS_SHORT)) {
        return groups();
      } else if (!strcmp(op, CMD_MY_GROUPS) || !strcmp(op, CMD_MY_GROUPS_SHORT)) {
        return my_groups();
      } else if (!strcmp(op, CMD_ULIST) || !strcmp(op, CMD_ULIST_SHORT)) {
        return ulist();
      } else if (!strcmp(op, CMD_SHOW_UID) || !strcmp(op, CMD_SHOW_UID_SHORT)) {
        return showUID();
      } else if (!strcmp(op, CMD_SHOW_GID) || !strcmp(op, CMD_SHOW_GID_SHORT)) {
        return showGID();
      }
      fprintf(stderr, MSG_UNKNOWN_CMD);
      return;

    case 2:
      if (!strcmp(op, CMD_UNSUBSCRIBE) || !strcmp(op, CMD_UNSUBSCRIBE_SHORT)) {
        if ((parseGID(arg1)) <= 0) {
          fprintf(stderr, MSG_INVALID_GID);
          return;
        }
        return unsubscribe(arg1);
      } else if (!strcmp(op, CMD_SELECT) || !strcmp(op, CMD_SELECT_SHORT)) {
        if ((parseGID(arg1)) <= 0) {
          fprintf(stderr, MSG_INVALID_GID);
          return;
        }
        return selectGroup(arg1);
      } else if (!strcmp(op, CMD_RETRIEVE) || !strcmp(op, CMD_RETRIEVE_SHORT)) {
        if (!(parseMID(arg1))) {
          fprintf(stderr, MSG_INVALID_MID);
          return;
        }
        return retrieve(arg1);
      }
      fprintf(stderr, MSG_UNKNOWN_CMD);
      return;

    case 3:
      if (!strcmp(op, CMD_REGISTER)) {
        if ((parseUID(arg1)) == -1) {
          fprintf(stderr, MSG_INVALID_UID);
          return;
        } else if (parsePassword(arg2) == REG_NOMATCH) {
          fprintf(stderr, MSG_INVALID_PASSWD);
          return;
        }
        return registerUser(arg1, arg2);
      } else if (!strcmp(op, CMD_UNREGISTER) || !strcmp(op, CMD_UNREGISTER_SHORT)) {
        if ((parseUID(arg1)) == -1) {
          fprintf(stderr, MSG_INVALID_UID);
          return;
        } else if (parsePassword(arg2) == REG_NOMATCH) {
          fprintf(stderr, MSG_INVALID_PASSWD);
          return;
        }
        return unregisterUser(arg1, arg2);
      } else if (!strcmp(op, CMD_LOGIN)) {
        if ((parseUID(arg1)) == -1) {
          fprintf(stderr, MSG_INVALID_UID);
          return;
        } else if (parsePassword(arg2) == REG_NOMATCH) {
          fprintf(stderr, MSG_INVALID_PASSWD);
          return;
        }
        return login(arg1, arg2);
      } else if (!strcmp(op, CMD_SUBSCRIBE) || !strcmp(op, CMD_SUBSCRIBE_SHORT)) {
        if ((parseGID(arg1)) == -1) {
          fprintf(stderr, MSG_INVALID_GID);
          return;
        } else if (parseGName(arg2) == REG_NOMATCH) {
          fprintf(stderr, MSG_INVALID_GNAME);
          return;
        }
        return subscribe(arg1, arg2);
      }
      fprintf(stderr, MSG_UNKNOWN_CMD);
      return;

    default: fprintf(stderr, MSG_UNKNOWN_CMD);
  }
}