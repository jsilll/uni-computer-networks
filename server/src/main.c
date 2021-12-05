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

unsigned short int PORT = 58000 + GROUP_NUMBER;
bool VERBOSE = false;

/* Argument Parsing Functions */
void parseExecArgs(int argc, char *argv[]);
void parsePortArg(char *port);

int main(int argc, char *argv[])
{
    parseExecArgs(argc, argv);
    printf("%d %d\n", PORT, VERBOSE);
}

/*
 * Parses the initial arguments for the program.
 * Input:
 *  - argc: number of arguments in argv
 *  - argv: array passed arguments
 */
void parseExecArgs(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, ":vp:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            parsePortArg(optarg);
            break;
        case 'v':
            VERBOSE = true;
            break;
        case ':':
            fprintf(stderr, "Missing argument for port (-p) option\n");
            exit(EXIT_FAILURE);
            break;
        case '?':
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (optind < argc)
    {
        fprintf(stderr, "Unecessary extra argument: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
}

/*
 * Validates the port option argument.
 * Input:
 *  - port: port argument in string format
 */
void parsePortArg(char *port)
{
    for (int i = 0; i < strlen(port); i++)
    {
        if (port[i] != '0')
        {
            int port_parsed = atoi(port);
            if (port_parsed > 0 && port_parsed <= 65535)
            {
                PORT = port_parsed;
                return;
            }
            else
            {
                fprintf(stderr, "Invalid value for port argument\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    PORT = 0;
}