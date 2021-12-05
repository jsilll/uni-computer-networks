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
#include "centralized_messaging_api.h"

#define GROUP_NUMBER 6

#define MAX_INPUT_SIZE 128

#define CMD_REGISTER "reg"
#define CMD_UNREGISTER "unregister"
#define CMD_UNREGISTER_SHORT "unr"
#define CMD_LOGIN "login"
#define CMD_LOGOUT "logout"
#define CMD_EXIT "exit"
#define CMD_GROUPS "groups"
#define CMD_GROUPS_SHORT "gl"
#define CMD_SUBSCRIBE "subscribe"
#define CMD_SUBSCRIBE_SHORT "s"
#define CMD_UNSUBSCRIBE "unsubscribe"
#define CMD_UNSUBSCRIBE_SHORT "u"
#define CMD_MY_GROUPS "my_groups"
#define CMD_MY_GROUPS_SHORT "mgl"
#define CMD_SELECT "select"
#define CMD_SELECT_SHORT "sag"
#define CMD_ULIST "ulist"
#define CMD_ULIST_SHORT "ul"
#define CMD_POST "post"
#define CMD_RETRIEVE "retrieve"
#define CMD_RETRIEVE_SHORT "r"

#define MSG_UKNOWN_CMD "Uknown command\n"
#define MSG_INVALID_GID "Invalid GID argument\n"
#define MSG_INVALID_TXT_MSG "Invalid text message argument\n"
#define MSG_INVALID_MID "Invalid MID argument\n"
#define MSG_INVALID_UID "Invalid UID argument\n"
#define MSG_INVALID_PASSWD "Invalid password argument\n"
#define MSG_INVALID_GNAME "Invalid GName argument\n"
#define MSG_INVALID_FNAME "Invalid Fname argument\n"

unsigned short int PORT = 58000 + GROUP_NUMBER;
unsigned long IP;

void parseExecArgs(int argc, char *argv[]);
void parseIPArg(char *ip);
void parsePortArg(char *port);

void parseCommand(char *line);
int parseGID(char *GID);
int parseUID(char *UID);
int parsePassword(char *password);
int parseGName(char *GName);
int parseMessageText(char *message);
int parseFName(char *FName);
int parseMID(char *MID);

int main(int argc, char *argv[])
{
    parseExecArgs(argc, argv);
    char line[MAX_INPUT_SIZE];
    while (fgets(line, sizeof(line) / sizeof(char), stdin))
    {
        parseCommand(line);
    }
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

/* parseCommand */
void parseCommand(char *line)
{
    char op[MAX_INPUT_SIZE], arg1[MAX_INPUT_SIZE], arg2[MAX_INPUT_SIZE];
    int numTokens = sscanf(line, "%s %s %s", op, arg1, arg2); // TODO: Mais argumentos do que os necessarios causam msg de erro?

    switch (numTokens)
    {
    case 1:
        if (!strcmp(op, CMD_LOGOUT))
            return logout();
        else if (!strcmp(op, CMD_EXIT))
            return exitClient();
        else if (!strcmp(op, CMD_GROUPS) || !strcmp(op, CMD_GROUPS_SHORT))
            return groups();
        else if (!strcmp(op, CMD_MY_GROUPS) || !strcmp(op, CMD_MY_GROUPS_SHORT))
            return my_groups();
        else if (!strcmp(op, CMD_ULIST) || !strcmp(op, CMD_ULIST_SHORT))
            return ulist();
        else
        {
            fprintf(stderr, MSG_UKNOWN_CMD);
            return;
        }

    case 2:
        if (!strcmp(op, CMD_UNSUBSCRIBE) || !strcmp(op, CMD_UNSUBSCRIBE_SHORT))
        {
            int GID;
            if ((GID = parseGID(arg1)) <= 0)
            {
                fprintf(stderr, MSG_INVALID_GID);
                return;
            }
            return unsubscribe(GID);
        }
        else if (!strcmp(op, CMD_SELECT) || !strcmp(op, CMD_SELECT_SHORT))
        {
            int GID;
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
                fprintf(stderr, MSG_INVALID_TXT_MSG);
            else
                return post(arg1, NULL);
        }
        else if (!strcmp(op, CMD_RETRIEVE) || !strcmp(op, CMD_RETRIEVE_SHORT))
        {
            int MID;
            if (!(MID = parseMID(arg1)))
                fprintf(stderr, MSG_INVALID_TXT_MSG);
            else
                return retrieve(MID);
        }
        else
            fprintf(stderr, MSG_UKNOWN_CMD);

        return;

    case 3:
        if (!strcmp(op, CMD_REGISTER))
        {
            int UID;
            if ((UID = parseUID(arg1)) == -1)
                fprintf(stderr, MSG_INVALID_UID);
            else if (parsePassword(arg2) == REG_NOMATCH)
                fprintf(stderr, MSG_INVALID_PASSWD);
            else
                return registerUser(UID, arg2);
        }
        else if (!strcmp(op, CMD_REGISTER) || !strcmp(op, CMD_UNREGISTER_SHORT))
        {
            int UID;
            if ((UID = parseUID(arg1)) == -1)
                fprintf(stderr, MSG_INVALID_UID);
            else if (parsePassword(arg2) == REG_NOMATCH)
                fprintf(stderr, MSG_INVALID_PASSWD);
            else
                return unregisterUser(UID, arg2);
        }
        else if (!strcmp(op, CMD_LOGIN))
        {
            int UID;
            if ((UID = parseUID(arg1)) == -1)
                fprintf(stderr, MSG_INVALID_UID);
            else if (parsePassword(arg2) == REG_NOMATCH)
                fprintf(stderr, MSG_INVALID_PASSWD);
            else
                return login(UID, arg2);
        }
        else if (!strcmp(op, CMD_SUBSCRIBE) || !strcmp(op, CMD_SUBSCRIBE_SHORT))
        {
            int GID;
            if ((GID = parseGID(arg1)) == -1)
                fprintf(stderr, MSG_INVALID_GID);
            else if (parseGName(arg2) == REG_NOMATCH)
                fprintf(stderr, MSG_INVALID_GNAME);
            else
                return subscribe(GID, arg2);
        }
        else if (!strcmp(op, CMD_POST))
        {
            if (parseMessageText(arg1) == -1)
                fprintf(stderr, MSG_INVALID_TXT_MSG);
            else if (parseFName(arg2) == REG_NOMATCH)
                fprintf(stderr, MSG_INVALID_FNAME);
            else
                return post(arg1, arg2);
        }
        else
            fprintf(stderr, MSG_UKNOWN_CMD);

        return;

    default:
        fprintf(stderr, MSG_UKNOWN_CMD);
    }
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
    if (!uid_parsed && strcmp(UID, "00000") != 0)
        return -1;
    return uid_parsed;
}

/* parsePassword */
int parsePassword(char *password)
{
    regex_t re;
    if (regcomp(&re, "^[a-zA-Z0-9]{8}$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, password, 0, NULL, 0);
    regfree(&re);
    return res;
}

/* parseGName */
int parseGName(char *GName)
{
    regex_t re;
    if (regcomp(&re, "^[a-zA-Z0-9_-]{1,24}$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, GName, 0, NULL, 0);
    regfree(&re);
    return res;
}

/* parseMID */
int parseMID(char *MID)
{
    if (strlen(MID) != 4)
        return 0;
    return atoi(MID);
}

/* parseMessageText */
int parseMessageText(char *message)
{
    int len = strlen(message);
    if (len > 242 || message[0] != '"' || message[len - 1] != '"')
        return -1;
    return 0;
}

/* parseFName */
int parseFName(char *FName)
{
    regex_t re;
    if (strlen(FName) > 24 || regcomp(&re, "^[a-zA-Z0-9_.-]+\\.[A-Za-z]{3}$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, FName, 0, NULL, 0);
    regfree(&re);
    return res;
}