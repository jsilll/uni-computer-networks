#ifndef RC_PROJECT_SERVER_COMMAND_ARG_PARSING_H_
#define RC_PROJECT_SERVER_COMMAND_ARG_PARSING_H_

#ifndef CENTRALIZED_MESSAGING_PARSING_H
#define CENTRALIZED_MESSAGING_PARSING_H

#include <regex.h>
#include <stdlib.h>
#include <string.h>

#define T_SIZE 240

/**
 * @brief Parses a string representing a group ID.
 * 2-digit number (01 – 99)
 *
 * @param gid string that represents a group ID
 * @return int
 */
int parseGID(char *gid) {
  if (strlen(gid) == 1 && gid[0] == '0')
    return 0;
  int gid_parsed = strtol(gid, NULL, 10);
  if (strlen(gid) == 2 && gid_parsed != 0)
    return gid_parsed;
  return -1;
}

/**
 * @brief Parses a string representing a user ID.
 * 5-digit IST student number
 *
 * @param uid string that represents a user ID
 * @return int
 */
int parseUID(char *uid) {
  if (strlen(uid) != 5)
    return -1;
  int uid_parsed = strtol(uid, NULL, 10);
  if (!uid_parsed && strcmp(uid, "00000") != 0)
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
int parsePassword(char *password) {
  regex_t re; // TODO: make this const
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
 * @param gname string that represents a group name
 * @return int
 */
int parseGName(char *gname) {
  regex_t re; // TODO: make this const
  if (regcomp(&re, "^[a-zA-Z0-9_-]{1,24}$", REG_EXTENDED | REG_NOSUB) != 0)
    return REG_NOMATCH;
  int res = regexec(&re, gname, 0, NULL, 0);
  regfree(&re);
  return res;
}

/**
 * @brief Parses a string representing a message ID.
 * 4-digit number
 *
 * @param mid string that represents a message ID
 * @return int
 */
int parseMID(char *mid) {
  if (strlen(mid) != 4)
    return 0;
  return strtol(mid, NULL, 10);
}

/**
 * @brief Parses a string representing a message text.
 * limited to a maximum of 240 characters
 *
 * @param message string that represents a message text
 * @return int
 */
int parseMessageText(char *message) {
  int len = strlen(message);
  if (len > (T_SIZE))
    return -1;
  return 0;
}

/**
 * @brief Parses a string representing a filename.
 * limited to a maximum of 24 alphanumerical characters
 * (and ‘-‘, ‘_’ and ‘.’), including the separating dot and
 * the 3-letter extension
 *
 * @param fname string that represents a filename
 * @return int
 */
int parseFName(char *fname) {
  regex_t re; // TODO: make this const
  if (strlen(fname) > 24 || regcomp(&re, "^[a-zA-Z0-9_.-]+[.]{1}[A-Za-z]{3}$", REG_EXTENDED | REG_NOSUB) != 0)
    return REG_NOMATCH;
  int res = regexec(&re, fname, 0, NULL, 0);
  regfree(&re);
  return res;
}

#endif //CENTRALIZED_MESSAGING_PARSING_H


#endif //RC_PROJECT_SERVER_COMMAND_ARG_PARSING_H_
