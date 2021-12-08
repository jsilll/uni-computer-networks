#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "centralized_messaging_api.h"
#include "centralized_messaging_parsing.h"
#include "client_interface.h"

#define DEFAULT_PORT "58006"
#define MAX_INPUT_SIZE T_SIZE + 32 /* 'post "240" 24' */

char PORT[MAX_INPUT_SIZE], IP[MAX_INPUT_SIZE];

/* Execution Arguments Parsing */
void parseExecArgs(int argc, char* argv[]);
void parseIPArg(char* ip);
void parsePortArg(char* port);

/* Commands Parsing */
void parseCommand(char* line);

void getLocalIP()
{
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_CANONNAME;

	char hostname_buffer[128];
	if (gethostname(hostname_buffer, 128) == -1)
		fprintf(stderr, "error: %s\n", strerror(errno));

	int errcode;
	struct addrinfo* client_addrinfo;
	if ((errcode = getaddrinfo(hostname_buffer, NULL, &hints, &client_addrinfo)))
	{
		fprintf(stderr, "error: getaddrinfo: %s\n", gai_strerror(errcode));
		exit(EXIT_FAILURE);
	}
	else
	{
		char buffer[INET_ADDRSTRLEN];

		struct in_addr* addr = &((struct sockaddr_in*)client_addrinfo->ai_addr)->sin_addr;
		strcpy(IP, inet_ntop(client_addrinfo->ai_family, addr, buffer, sizeof buffer));
		return;
	}
}

int main(int argc, char* argv[])
{
	strcpy(PORT, DEFAULT_PORT); // PORT -> 58006
	getLocalIP(); // IP -> "192.168.1.100"

	parseExecArgs(argc, argv);

	printf("Centralized Messaging Client Initialized\n");
	printf("PORT: %s IP: %s\n", PORT, IP);

	setupSocketUDP(IP, PORT);

	char line[MAX_INPUT_SIZE];
	while (fgets(line, sizeof(line) / sizeof(char), stdin))
	{
		parseCommand(line);
	}
	return 1;
}

/**
 * @brief Parses the initial arguments for the program
 *
 * @param argc number of arguments in argv
 * @param argv array passed arguments
 */
void parseExecArgs(int argc, char* argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, ":n:p:")) != -1)
	{
		switch (opt)
		{
		case 'n':
			parseIPArg(optarg);
			strcpy(IP, optarg);
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
 * @brief Validates the ip option argument.
 *
 * @param ip ip argument in string format
 */
void parseIPArg(char* ip)
{
	unsigned long ip_parsed = 0;
	if (!inet_pton(AF_INET, ip, &ip_parsed))
	{
		fprintf(stderr, "Invalid value for ip argument\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Validates the port option argument.
 *
 * @param port port argument in string format
 */
void parsePortArg(char* port)
{
	for (int i = 0; i < strlen(port); i++)
	{
		if (port[i] != '0')
		{
			int port_parsed = strtol(port, NULL, 10);
			if (port_parsed <= 0 || port_parsed > 65535)
			{
				fprintf(stderr, "Invalid value for port argument\n");
				exit(EXIT_FAILURE);
			}
			return;
		}
	}
}

/**
 * @brief Parses string representing a command.
 *
 * @param line string that represents a command
 */
void parseCommand(char* line)
{
	int UID, GID, MID;
	char op[MAX_INPUT_SIZE], arg1[MAX_INPUT_SIZE], arg2[MAX_INPUT_SIZE];
	// TODO: Mais argumentos do que os necessarios causam msg de erro?
	int numTokens = sscanf(line, "%s %s %s", op, arg1, arg2);

	switch (numTokens)
	{
	case 1:
		if (!strcmp(op, CMD_LOGOUT))
		{
			return logout();
		}
		else if (!strcmp(op, CMD_EXIT))
		{
			return exitClient();
		}
		else if (!strcmp(op, CMD_GROUPS) || !strcmp(op, CMD_GROUPS_SHORT))
		{
			return groups();
		}
		else if (!strcmp(op, CMD_MY_GROUPS) || !strcmp(op, CMD_MY_GROUPS_SHORT))
		{
			return my_groups();
		}
		else if (!strcmp(op, CMD_ULIST) || !strcmp(op, CMD_ULIST_SHORT))
		{
			return ulist();
		}
		else if (!strcmp(op, CMD_SHOW_UID) || !strcmp(op, CMD_SHOW_UID_SHORT))
		{
			return showUID();
		}
		else if (!strcmp(op, CMD_SHOW_GID) || !strcmp(op, CMD_SHOW_GID_SHORT))
		{
			return showGID();
		}
		fprintf(stderr, MSG_UNKNOWN_CMD);
		return;

	case 2:
		if (!strcmp(op, CMD_UNSUBSCRIBE) || !strcmp(op, CMD_UNSUBSCRIBE_SHORT))
		{
			if ((GID = parseGID(arg1)) <= 0)
			{
				fprintf(stderr, MSG_INVALID_GID);
				return;
			}
			return unsubscribe(GID);
		}
		else if (!strcmp(op, CMD_SELECT) || !strcmp(op, CMD_SELECT_SHORT))
		{
			if ((GID = parseGID(arg1)) <= 0)
			{
				fprintf(stderr, MSG_INVALID_GID);
				return;
			}
			return selectGroup(GID);
		}
		else if (!strcmp(op, CMD_POST))
		{
			if (parseMessageText(arg1) == -1)
			{
				fprintf(stderr, MSG_INVALID_TXT_MSG);
				return;
			}
			return post(arg1, NULL);
		}
		else if (!strcmp(op, CMD_RETRIEVE) || !strcmp(op, CMD_RETRIEVE_SHORT))
		{
			if (!(MID = parseMID(arg1)))
			{
				fprintf(stderr, MSG_INVALID_MID);
				return;
			}
			return retrieve(MID);
		}
		fprintf(stderr, MSG_UNKNOWN_CMD);
		return;

	case 3:
		if (!strcmp(op, CMD_REGISTER))
		{
			if ((UID = parseUID(arg1)) == -1)
			{
				fprintf(stderr, MSG_INVALID_UID);
				return;
			}
			else if (parsePassword(arg2) == REG_NOMATCH)
			{
				fprintf(stderr, MSG_INVALID_PASSWD);
				return;
			}
			return registerUser(UID, arg2);
		}
		else if (!strcmp(op, CMD_UNREGISTER) || !strcmp(op, CMD_UNREGISTER_SHORT))
		{
			if ((UID = parseUID(arg1)) == -1)
			{
				fprintf(stderr, MSG_INVALID_UID);
				return;
			}
			else if (parsePassword(arg2) == REG_NOMATCH)
			{
				fprintf(stderr, MSG_INVALID_PASSWD);
				return;
			}
			return unregisterUser(UID, arg2);
		}
		else if (!strcmp(op, CMD_LOGIN))
		{
			if ((UID = parseUID(arg1)) == -1)
			{
				fprintf(stderr, MSG_INVALID_UID);
				return;
			}
			else if (parsePassword(arg2) == REG_NOMATCH)
			{
				fprintf(stderr, MSG_INVALID_PASSWD);
				return;
			}
			return login(UID, arg2);
		}
		else if (!strcmp(op, CMD_SUBSCRIBE) || !strcmp(op, CMD_SUBSCRIBE_SHORT))
		{
			if ((GID = parseGID(arg1)) == -1)
			{
				fprintf(stderr, MSG_INVALID_GID);
				return;
			}
			else if (parseGName(arg2) == REG_NOMATCH)
			{
				fprintf(stderr, MSG_INVALID_GNAME);
				return;
			}
			return subscribe(GID, arg2);
		}
		else if (!strcmp(op, CMD_POST))
		{
			if (parseMessageText(arg1) == -1)
			{
				fprintf(stderr, MSG_INVALID_TXT_MSG);
				return;
			}
			else if (parseFName(arg2) == REG_NOMATCH)
			{
				fprintf(stderr, MSG_INVALID_FNAME);
				return;
			}
			return post(arg1, arg2);
		}
		fprintf(stderr, MSG_UNKNOWN_CMD);
		return;

	default:
		fprintf(stderr, MSG_UNKNOWN_CMD);
	}
}
