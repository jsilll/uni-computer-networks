/* main.c */
/* 0 copyright/licensing */
/* 1 includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 2 defines */
/* 3 external declarations */
/* 4 typedefs */

/* 5 global variable declarations */
int PORT = 58000 + 6;
int VERBOSE = 0;

/* 6 function prototypes */
void parseArgs(int argc, char *argv[])
{
    /* Too many arguments */
    if (argc > 4)
    {
        fprintf(stderr, "Too many arguments\n");
        return exit(1);
    }

    int seen_v = 0;
    int seen_p = 0;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
        {
            if (seen_v == 1)
            {
                fprintf(stderr, "Repeated -v argument\n");
                return exit(1);
            }
            seen_v = 1;
            VERBOSE = 1;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            if (seen_p == 1)
            {
                fprintf(stderr, "Repeated -p argument\n");
                return exit(1);
            }

            seen_p = 1;
            if (i > argc - 2)
            {
                fprintf(stderr, "Missing port number\n");
                return exit(1);
            }

            int PORT_ARG = -1;
            if (strcmp(argv[i + 1], "0") == 0)
            {
                PORT = 0;
            }
            else
            {
                PORT_ARG = atoi(argv[i + 1]);
                if (PORT_ARG == 0 || PORT_ARG > 65535 || PORT_ARG < 0)
                {
                    fprintf(stderr, "Port must be between 0 and 65535\n");
                    return exit(1);
                }

                PORT = PORT_ARG;
            }
        }
    }

    if (seen_v + seen_p * 2 != argc - 1)
    {
        fprintf(stderr, "Uknown argument\n");
        return exit(1);
    }
}

int main(int argc, char *argv[])
{
    parseArgs(argc, argv);
    printf("%d %d\n", PORT, VERBOSE);
}

/* 8 function declarations */