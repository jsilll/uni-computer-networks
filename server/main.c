#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define GROUP_NUMBER 6
#define BOOL_TO_STR(b) (b ? "On" : "Off")

unsigned short int PORT = 58000 + GROUP_NUMBER;
bool VERBOSE = false;

/* Argument Parsing Functions */
void parseExecArgs(int argc, char* argv[]);
unsigned short int parsePortArg(char* port);

int main(int argc, char* argv[])
{
	parseExecArgs(argc, argv);
	printf("Centralized Messaging Server Initialized\n");
	printf("PORT: %d VERBOSE: %s\n", PORT, BOOL_TO_STR(VERBOSE));
	return 1;
}

/*
 * Parses the initial arguments for the program.
 * Input:
 *  - argc: number of arguments in argv
 *  - argv: array passed arguments
 */
void parseExecArgs(int argc, char* argv[])
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
