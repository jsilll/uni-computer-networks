#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GROUP_NUMBER 6

int PORT = 58000 + GROUP_NUMBER;
bool VERBOSE = false;

/* Argument Parsing Functions */
void parseArgs(int argc, char *argv[]);
int validatePortArg(char *port);

int main(int argc, char *argv[])
{
    parseArgs(argc, argv);
    printf("%d %d\n", PORT, VERBOSE);
}

/*
 * Parses the initial arguments for the program.
 * Input:
 *  - argc: number of arguments in argv
 *  - argv: array passed arguments
 */
void parseArgs(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, ":vp:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            PORT = validatePortArg(optarg);
            break;
        case 'v':
            VERBOSE = true;
            break;
        case ':':
            fprintf(stderr, "Missing value for port (-p) option\n");
            exit(EXIT_FAILURE);
            break;
        case '?':
            fprintf(stderr, "Unknown option: %c\n", optopt);
            exit(EXIT_FAILURE);
            break;
        }
    }

    for (; optind < argc; optind++)
    {
        fprintf(stderr, "Unecessary extra argument: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
}

/*
 * Validates the port argument.
 * Input:
 *  - port: port argument in string format
 */
int validatePortArg(char *port)
{
    int is_zero = 1, port_length = strlen(port), port_parsed;
    for (int i = 0; i < port_length; i++)
    {
        if (port[i] != '0')
        {
            is_zero = 0;
            break;
        }
    }

    if (is_zero)
        return 0;

    port_parsed = atoi(port);
    if (port_parsed > 0 && port_parsed <= 65535)
        return port_parsed;
    else
    {
        fprintf(stderr, "Invalid port value\n");
        exit(EXIT_FAILURE);
    }
}