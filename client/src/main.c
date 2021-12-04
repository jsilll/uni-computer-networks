#include <arpa/inet.h>
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
unsigned long IP;

void parseArgs(int argc, char *argv[]);
void parseIPArg(char *ip);
void parsePortArg(char *port);

int main(int argc, char *argv[])
{
    parseArgs(argc, argv);
    printf("%ld %d\n", IP, PORT);
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
    while ((opt = getopt(argc, argv, ":n:p:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            parseIPArg(optarg);
            break;
        case 'p':
            parsePortArg(optarg);
            break;
        case ':':
            fprintf(stderr, "Missing value for ip (-n) or for port (-p) option\n");
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
 * Validates the ip option argument.
 * Input:
 *  - ip: ip argument in string format
 */
void parseIPArg(char *ip)
{
    unsigned long ip_parsed = 0;

    if (!inet_pton(AF_INET, ip, &ip_parsed))
    {
        fprintf(stderr, "Invalid value for ip argument\n");
        exit(EXIT_FAILURE);
    }

    IP = ip_parsed;
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