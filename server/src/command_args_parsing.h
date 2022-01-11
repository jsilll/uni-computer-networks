#ifndef COMMAND_ARGS_PARSING_H_
#define COMMAND_ARGS_PARSING_H_

#include <regex.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parses a string representing a group ID.
 * 2-digit number (01 – 99)
 *
 * @param gid string that represents a group ID
 * @return int
 */
int parseGID(char *gid)
{
  int gid_parsed = atoi(gid);
  if (strlen(gid) == 2 && gid_parsed > 0 && gid_parsed <= 99)
    return 0;
  return -1;
}

/**
 * @brief Parses a string representing a user ID.
 * 5-digit IST student number
 *
 * @param uid string that represents a user ID
 * @return int
 */
int parseUID(char *uid)
{
  if (strlen(uid) != 5)
    return -1;
  int uid_parsed = atoi(uid);
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
int parsePassword(char *password)
{
  regex_t re;
  if (regcomp(&re, "^[a-zA-Z0-9]{8}$", REG_EXTENDED | REG_NOSUB) != 0 || regexec(&re, password, 0, NULL, 0) != 0)
  {
    regfree(&re);
    return -1;
  }
  regfree(&re);
  return 0;
}

/**
 * @brief Parses a string representing a group name.
 * limited to a total of 24 alphanumerical characters
 * (plus ‘-‘, and ‘_’)
 *
 * @param gname string that represents a group name
 * @return int
 */
int parseGName(char *gname)
{
  regex_t re;
  if (regcomp(&re, "^[a-zA-Z0-9_-]{1,24}$", REG_EXTENDED | REG_NOSUB) != 0 || regexec(&re, gname, 0, NULL, 0) != 0)
  {
    regfree(&re);
    return -1;
  }
  regfree(&re);
  return 0;
}

/**
 * @brief Parses a string representing a message ID.
 * 4-digit number
 *
 * @param mid string that represents a message ID
 * @return int
 */
int parseMID(char *mid)
{
  if (strlen(mid) != 4 || strtol(mid, NULL, 10) <= 0)
    return -1;
  return 0;
}

/**
 * @brief Parses Tsize argument
 *
 * @param tsize
 * @return int
 */
int parseTSize(char *tsize)
{
  int size = atoi(tsize);
  if (strlen(tsize) > 3 || size > 240 || size <= 0)
  {
    return -1;
  }
  return 0;
}

/**
 * @brief Parses the file size
 *
 * @param size
 * @return int
 */
int parseFileSize(char *size)
{
  if (strlen(size) > 10)
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
int parseFName(char *fname)
{
  regex_t re;
  if (strlen(fname) > 24)
    return -1;
  if (regcomp(&re, "^[a-zA-Z0-9_.-]+[.]{1}[a-zA-Z0-9]{3}$", REG_EXTENDED | REG_NOSUB) != 0 || regexec(&re, fname, 0, NULL, 0) != 0)
  {
    regfree(&re);
    return -1;
  }
  regfree(&re);
  return 0;
}

#endif // COMMAND_ARGS_PARSING_H_