#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <libgen.h>

#include "parsing.h"
#include "centralized_messaging/commands.h"
#include "command_args_parsing.h"

#define DEFAULT_PORT "58006"
#define DEFAULT_IP "localhost"
#define MAX_INPUT_SIZE 240 + 32

char PORT[6], ADDRESS[INET6_ADDRSTRLEN];

void exitClient(int signum);
void loadInitArgs(int argc, char *argv[]);
void readCommand(char *line);

int main(int argc, char *argv[])
{
  signal(SIGINT, exitClient);

  strcpy(ADDRESS, DEFAULT_IP);
  strcpy(PORT, DEFAULT_PORT);

  loadInitArgs(argc, argv);

  printf("Centralized Messaging Client\n");
  printf("ADDRESS:%s PORT:%s\n", ADDRESS, PORT);

  if (setupServerAddresses(ADDRESS, PORT) == -1)
  {
    exit(EXIT_FAILURE);
  }

  char line[MAX_INPUT_SIZE];
  while (fgets(line, sizeof(line) / sizeof(char), stdin))
  {
    readCommand(line);
  }
}

/**
 * @brief Signal Handler for terminating the client
 *
 * @param signum
 */
void exitClient(int signum)
{
  printf("Exiting Centralized Messaging Client.\n");
  freeServerAddresses();
  exit(signum);
}

/**
 * @brief Loads the initial arguments given by the
 * user to the program
 *
 * @param argc number of arguments in argv
 * @param argv array passed arguments
 */
void loadInitArgs(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, ":n:p:")) != -1)
  {
    switch (opt)
    {
    case 'n':
      parseIPArg(optarg);
      strcpy(ADDRESS, optarg);
      break;
    case 'p':
      parsePortArg(optarg);
      strcpy(PORT, optarg);
      break;
    case ':':
      fprintf(stderr, "Missing value for ip (-n) or for port (-p) option\n");
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
 * @brief Executes a command represented by a string.
 *
 * @param line string that represents a command
 */
void readCommand(char *line)
{
  char op[MAX_INPUT_SIZE] = {'\0'}, arg1[MAX_INPUT_SIZE] = {'\0'}, arg2[MAX_INPUT_SIZE] = {'\0'}, arg3[MAX_INPUT_SIZE] = {'\0'};

  int numTokens = sscanf(line, "%s %s %s %s", op, arg1, arg2, arg3);
  if (!strcmp(op, CMD_POST))
  {
    arg2[0] = '\0';

    numTokens = sscanf(line, "%s \"%[^\"]\" %s", op, arg1, arg2);
    if (numTokens < 2)
    {
      fprintf(stderr, ERR_INVALID_POST_CMD);
    }
    else if ((!strlen(arg2) && line[strlen(line) - 2] != '"'))
    {
      fprintf(stderr, ERR_INVALID_POST_CMD);
    }
    else if (strlen(arg2))
    {
      if (strlen(arg1) > 240)
      {
        fprintf(stderr, ERR_INVALID_TXT_MSG);
      }
      else if (parseFName(basename(arg2)) == -1)
      {
        fprintf(stderr, ERR_INVALID_FNAME);
      }
      else
      {
        post(arg1, arg2);
      }
    }
    else
    {
      if (strlen(arg1) > 240)
      {
        fprintf(stderr, ERR_INVALID_TXT_MSG);
      }
      post(arg1, NULL);
    }
  }
  else if (numTokens == 4)
  {
    fprintf(stderr, ERR_UNKNOWN_CMD);
  }
  else
  {
    switch (numTokens)
    {
    case 1:
      if (!strcmp(op, CMD_LOGOUT))
      {
        logout();
      }
      else if (!strcmp(op, CMD_EXIT))
      {
        exitClient(EXIT_SUCCESS);
      }
      else if (!strcmp(op, CMD_GROUPS) || !strcmp(op, CMD_GROUPS_SHORT))
      {
        groups();
      }
      else if (!strcmp(op, CMD_MY_GROUPS) || !strcmp(op, CMD_MY_GROUPS_SHORT))
      {
        myGroups();
      }
      else if (!strcmp(op, CMD_ULIST) || !strcmp(op, CMD_ULIST_SHORT))
      {
        ulist();
      }
      else if (!strcmp(op, CMD_SHOW_UID) || !strcmp(op, CMD_SHOW_UID_SHORT))
      {
        showUID();
      }
      else if (!strcmp(op, CMD_SHOW_GID) || !strcmp(op, CMD_SHOW_GID_SHORT))
      {
        showGID();
      }
      else
      {
        fprintf(stderr, ERR_UNKNOWN_CMD);
      }
      break;

    case 2:
      if (!strcmp(op, CMD_UNSUBSCRIBE) || !strcmp(op, CMD_UNSUBSCRIBE_SHORT))
      {
        if (parseGID(arg1) == -1)
        {
          fprintf(stderr, ERR_INVALID_GID);
        }
        else
        {
          unsubscribe(atoi(arg1));
        }
      }
      else if (!strcmp(op, CMD_SELECT) || !strcmp(op, CMD_SELECT_SHORT))
      {
        if (parseGID(arg1) == -1)
        {
          fprintf(stderr, ERR_INVALID_GID);
        }
        else
        {
          selectGroup(atoi(arg1));
        }
      }
      else if (!strcmp(op, CMD_RETRIEVE) || !strcmp(op, CMD_RETRIEVE_SHORT))
      {
        if (parseMID(arg1) == -1)
        {
          fprintf(stderr, ERR_INVALID_MID);
        }
        else
        {
          retrieve(atoi(arg1));
        }
      }
      else
      {
        fprintf(stderr, ERR_UNKNOWN_CMD);
      }
      break;

    case 3:
      if (!strcmp(op, CMD_REGISTER))
      {
        if (parseUID(arg1) == -1)
        {
          fprintf(stderr, ERR_INVALID_UID);
        }
        else if (parsePassword(arg2) == -1)
        {
          fprintf(stderr, ERR_INVALID_PASSWD);
        }
        else
        {
          registerUser(atoi(arg1), arg2);
        }
      }
      else if (!strcmp(op, CMD_UNREGISTER) || !strcmp(op, CMD_UNREGISTER_SHORT))
      {
        if (parseUID(arg1) == -1)
        {
          fprintf(stderr, ERR_INVALID_UID);
        }
        else if (parsePassword(arg2) == -1)
        {
          fprintf(stderr, ERR_INVALID_PASSWD);
        }
        else
        {
          unregisterUser(atoi(arg1), arg2);
        }
      }
      else if (!strcmp(op, CMD_LOGIN))
      {
        if (parseUID(arg1) == -1)
        {
          fprintf(stderr, ERR_INVALID_UID);
        }
        else if (parsePassword(arg2) == -1)
        {
          fprintf(stderr, ERR_INVALID_PASSWD);
        }
        else
        {
          login(atoi(arg1), arg2);
        }
      }
      else if (!strcmp(op, CMD_SUBSCRIBE) || !strcmp(op, CMD_SUBSCRIBE_SHORT))
      {

        if (parseGID(arg1) == -1 && strcmp(arg1, "0") && strcmp(arg1, "00"))
        {
          fprintf(stderr, ERR_INVALID_UID);
        }
        else if (parseGName(arg2) == -1)
        {
          fprintf(stderr, ERR_INVALID_PASSWD);
        }
        else
        {
          subscribe(atoi(arg1), arg2);
        }
      }
      else
      {
        fprintf(stderr, ERR_UNKNOWN_CMD);
      }
      break;

    default:
      fprintf(stderr, ERR_UNKNOWN_CMD);
    }
  }
}