/* centralized_messaging_parsing.h */
#ifndef CENTRALIZED_MESSAGING_PARSING_H
#define CENTRALIZED_MESSAGING_PARSING_H

#include <regex.h>
#include <stdlib.h>
#include <string.h>

#define TSIZE 240

/**
 * @brief Parses a string representing a group ID.
 * 2-digit number (01 – 99)
 * 
 * @param GID string that represents a group ID
 * @return int
 */
int parseGID(char *GID)
{
    if (strlen(GID) == 1 && GID[0] == '0')
        return 0;
    int gid = atoi(GID);
    if (strlen(GID) == 2 && gid != 0)
        return gid;
    return -1;
}

/**
 * @brief Parses a string representing a user ID.
 * 5-digit IST student number
 * 
 * @param UID string that represents a user ID
 * @return int 
 */
int parseUID(char *UID)
{
    if (strlen(UID) != 5)
        return -1;
    int uid_parsed = atoi(UID);
    if (!uid_parsed && strcmp(UID, "00000") != 0)
        return -1;
    return uid_parsed;
}

/**
 * @brief Parses a string representing a password.
 * 8 alphanumerical characters, restricted to
 * letters and numbers
 * 
 * @param password string that represents a password
 * @return int 
 */
int parsePassword(char *password)
{
    regex_t re;
    if (regcomp(&re, "^[a-zA-Z0-9]{8}$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, password, 0, NULL, 0);
    regfree(&re);
    return res;
}

/**
 * @brief Parses a string representing a group name.
 * limited to a total of 24 alphanumerical characters
 * (plus ‘-‘, and ‘_’)
 * 
 * @param GName string that represents a group name
 * @return int 
 */
int parseGName(char *GName)
{
    regex_t re;
    if (regcomp(&re, "^[a-zA-Z0-9_-]{1,24}$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, GName, 0, NULL, 0);
    regfree(&re);
    return res;
}

/**
 * @brief Parses a string representing a message ID.
 * 4-digit number 
 * 
 * @param MID string that represents a message ID
 * @return int 
 */
int parseMID(char *MID)
{
    if (strlen(MID) != 4)
        return 0;
    return atoi(MID);
}

/**
 * @brief Parses a string representing a message text.
 * limited to a maximum of 240 characters 
 * 
 * @param message string that represents a message text
 * @return int 
 */
int parseMessageText(char *message)
{
    int len = strlen(message);
    if (len > (TSIZE + 2) || message[0] != '"' || message[len - 1] != '"')
        return -1;
    return 0;
}

/**
 * @brief Parses a string representing a filename.
 * limited to a maximum of 24 alphanumerical characters 
 * (and ‘-‘, ‘_’ and ‘.’), including the separating dot and
 * the 3-letter extension
 * 
 * @param FName string that represents a filename
 * @return int 
 */
int parseFName(char *FName)
{
    regex_t re;
    if (strlen(FName) > 24 || regcomp(&re, "^[a-zA-Z0-9_.-]+[.]{1}[A-Za-z]{3}$", REG_EXTENDED | REG_NOSUB) != 0)
        return REG_NOMATCH;
    int res = regexec(&re, FName, 0, NULL, 0);
    regfree(&re);
    return res;
}

#endif /* centralized_messaging_parsing.h */