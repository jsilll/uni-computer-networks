#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define GROUP_NUMBER 6
#define MAX_INPUT_SIZE 128

unsigned short int PORT = 58000 + GROUP_NUMBER;
unsigned long IP;

void parseArgs(int argc, char *argv[]);
void parseIPArg(char *ip);
void parsePortArg(char *port);

void parseCommand();
void registerUser(int UID, char *pass);
void unregisterUser(int UID, char *pass);
void login(int UID, char *pass);
void logout();
void exitClient();
void groups();
void subscribe(int GID, char *GIDName);
void unsubscribe(int GID);
void my_groups();
void selectGroup(int GID);
void ulist();
int parseGID(char *GID);
int parseUID(char *UID);
int parsePassword(char *password);
int parseGName(char *GName);

int main(int argc, char *argv[])
{
    parseArgs(argc, argv);
    printf("%ld %d\n", IP, PORT);
    while (1)
    {
        parseCommand();
    }
}

void parseCommand()
{
    char line[MAX_INPUT_SIZE];

    while (fgets(line, sizeof(line) / sizeof(char), stdin))
    {
        char op[MAX_INPUT_SIZE], arg1[MAX_INPUT_SIZE], arg2[MAX_INPUT_SIZE];
        int numTokens = sscanf(line, "%s %s %s", op, arg1, arg2); // TODO: Mais argumentos do que os necessarios causam msg de erro?

        switch (numTokens)
        {
        case 1:
            if (strcmp(op, "logout") == 0)
            {
                logout();
                break;
            }
            else if (strcmp(op, "exit") == 0)
            {
                exitClient();
                break;
            }
            else if (strcmp(op, "groups") == 0 || strcmp(op, "gl") == 0)
            {
                groups();
                break;
            }
            else if (strcmp(op, "my_groups") == 0 || strcmp(op, "mgl") == 0)
            {
                my_groups();
                break;
            }
            else if (strcmp(op, "ulist") == 0 || strcmp(op, "ul") == 0)
            {
                ulist();
                break;
            }
            else
                fprintf(stderr, "nao sei o que isso é (1)\n");
            break;
        case 2:
            if (strcmp(op, "unsubscribe") == 0 || strcmp(op, "u") == 0)
            {
                int GID;
                if ((GID = parseGID(arg1)) <= 0)
                {
                    fprintf(stderr, "Invalid GID argument\n");
                    break;
                }
                unsubscribe(GID);
                break;
            }
            else if (strcmp(op, "select") == 0 || strcmp(op, "sag") == 0)
            {
                int GID;
                if ((GID = parseGID(arg1)) <= 0)
                {
                    fprintf(stderr, "Invalid GID argument\n");
                    break;
                }
                selectGroup(GID);
                break;
            }
            else
                fprintf(stderr, "nao sei o que isso é (2)\n");
            break;
        case 3:
            if (strcmp(op, "reg") == 0)
            {
                int UID;
                if ((UID = parseUID(arg1)) == -1)
                {
                    fprintf(stderr, "Invalid UID argument\n");
                    break;
                }
                else if (parsePassword(arg2) == REG_NOMATCH)
                {
                    fprintf(stderr, "Invalid password argument\n");
                    break;
                }
                registerUser(UID, arg2);
                break;
            }
            else if (strcmp(op, "unregister") == 0 || strcmp(op, "unr") == 0)
            {
                int UID;
                if ((UID = parseUID(arg1)) == -1)
                {
                    fprintf(stderr, "Invalid UID argument\n");
                    break;
                }
                else if (parsePassword(arg2) == REG_NOMATCH)
                {
                    fprintf(stderr, "Invalid password argument\n");
                    break;
                }
                unregisterUser(UID, arg2);
                break;
            }
            else if (strcmp(op, "login") == 0)
            {
                int UID;
                if ((UID = parseUID(arg1)) == -1)
                {
                    fprintf(stderr, "Invalid UID argument\n");
                    break;
                }
                else if (parsePassword(arg2) == REG_NOMATCH)
                {
                    fprintf(stderr, "Invalid password argument\n");
                    break;
                }
                login(UID, arg2);
            }
            if (strcmp(op, "subscribe") == 0 || strcmp(op, "s") == 0)
            {
                int GID;
                if ((GID = parseGID(arg1)) == -1)
                {
                    fprintf(stderr, "Invalid GID argument\n");
                    break;
                }
                else if (parseGName(arg2) == REG_NOMATCH)
                {
                    fprintf(stderr, "Invalid GName argument\n");
                    break;
                }
                subscribe(GID, arg2);
                break;
            }
            else
                fprintf(stderr, "nao sei o que isso é (3)\n");
            break;
        default:
            fprintf(stderr, "nao sei o que isso é\n");
            continue;
        }
    }
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

// • reg UID pass – following this command the User application sends a
// message to the DS server, using the UDP protocol, asking to register a new user,
// sending its identification UID and a selected password pass.
// The result of the DS registration request should be displayed.
void registerUser(int UID, char *pass)
{
    printf("registerUser\n");
}

// • unregister UID pass or unr UID pass – the User application sends
// a message to the DS server, using the UDP protocol, asking to unregister the
// user with identification UID and password pass. The DS server should
// unsubscribe this user from all groups in which it was subscribed. The result of
// the unregister request should be displayed.
// The commands related to user identification and session termination are:
void unregisterUser(int UID, char *pass)
{
    printf("unrgesterUser\n");
}

// • login UID pass – with this command the User application sends a
// message in UDP to the DS to validate the user credentials: UID and pass. The
// result of the DS validation should be displayed to the user.
// The User application memorizes the UID in usage.
void login(int UID, char *pass)
{
    printf("login\n");
}

// • logout – the User application (locally) forgets the credentials of the
// previously logged in user. A new login command, with different credentials, can
// then be issued.
void logout()
{
    printf("logout\n");
}

// • exit – the User application terminates, after making that all TCP connections
// are closed.
// 04/12/2021
// The commands related to group management are listed below.
void exitClient()
{
    printf("exit\n");
}

// • groups or gl – following this command the User application sends the DS
// server a message in UDP asking for the list of available groups. The reply
// should be displayed as a list of group IDs (GID) and names (GName).
// These following group management commands can only be issued after a user has
// logged in:
void groups()
{
    printf("groups\n");
}

// • subscribe GID GName or s GID GName – following this command the
// User application sends the DS server a message in UDP, including the user’s
// UID, asking to subscribe the desired group, identified by its GID and GName. If
// GID = 0 this corresponds to a request to create and subscribe to a new group
// named GName. The confirmation of successful subscription (or not) should be
// displayed.
void subscribe(int GID, char *GIDName)
{
    printf("subscribe\n");
}

// • unsubscribe GID or u GID – following this command the User
// application sends the DS server a message in UDP, including the user’s UID,
// asking to unsubscribe group GID. The confirmation of success (or not) should
// be displayed.
void unsubscribe(int GID)
{
    printf("unsubscribe\n");
}

// • my_groups or mgl – following this command the User application sends
// the DS server a message in UDP, including the user’s UID, asking the list of
// groups to which this user has already subscribed. The reply should be displayed
// as a list of group IDs and names.
void my_groups()
{
    printf("my_groups\n");
}

// • select GID or sag GID – following this command the User application
// locally memorizes GID as the ID of the active group. Subsequent ulist, post
// and retrieve messaging commands refer to this GID.
void selectGroup(int GID)
{
    printf("select\n");
}

// • ulist or ul – following this command the User application sends the DS
// server a message in TCP asking for the list of user UIDs that are subscribed to
// the currently subscribed group GID.
void ulist()
{
    printf("ulist\n");
}

/* parseGID */
int parseGID(char *GID)
{
    if (strlen(GID) == 1 && GID[0] == '0')
        return 0;
    int gid = atoi(GID);
    if (strlen(GID) == 2 && gid != 0)
        return gid;
    return -1;
}

/* parseUID */
int parseUID(char *UID)
{
    if (strlen(UID) != 5)
        return -1;
    int uid_parsed = atoi(UID);
    if (uid_parsed == 0 && strcmp(UID, "00000") != 0)
        return -1;
    return uid_parsed;
}

/* parsePassword */
int parsePassword(char *password)
{
    regex_t re;
    if (strlen(password) != 8 || regcomp(&re, "^[a-zA-Z0-9]*$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, password, 0, NULL, 0);
    regfree(&re);
    return res;
}

/* parsePassword */
int parseGName(char *GName)
{
    printf("[%s]\n", GName);
    regex_t re;
    if (strlen(GName) > 24 || regcomp(&re, "^[a-zA-Z0-9_-]*$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, GName, 0, NULL, 0);
    regfree(&re);
    return res;
}