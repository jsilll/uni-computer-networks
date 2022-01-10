#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "operations.h"

void listGroups(char *buffer, char *uid);
int createFile(char *FILENAME, char *data);
int checkFileContent(char *FILENAME, char *data);
int userLoggedIn(char *uid);

int N_GROUPS = 0;
char PATH_BUFFER[256];

/**
 * Registers a user on the DS server
 * @param uid
 * @param pass
 * @return
 */
int registerUser(char *uid, char *password)
{
  sprintf(PATH_BUFFER, "USERS/%s", uid);
  if (mkdir(PATH_BUFFER, 0700) == -1)
    return -1; // DUP
  sprintf(PATH_BUFFER, "USERS/%s/password.txt", uid);
  createFile(PATH_BUFFER, password);
  return 0;
}

/**
 * @brief Unregisters a user from the DS server
 *
 * @param uid
 * @param pass
 * @return
 */
int unregisterUser(char *uid, char *pass)
{
  sprintf(PATH_BUFFER, "USERS/%s/password.txt", uid);
  if (checkFileContent(PATH_BUFFER, pass) == -1)
    return -1; // wrong password NOK
  unlink(PATH_BUFFER);

  sprintf(PATH_BUFFER, "USERS/%s/login.txt", uid);
  unlink(PATH_BUFFER);

  sprintf(PATH_BUFFER, "USERS/%s", uid);
  rmdir(PATH_BUFFER);

  for (int i = 0; i < 100; i++)
  {
    sprintf(PATH_BUFFER, "GROUPS/%02d/%s.txt", i, uid);
    unlink(PATH_BUFFER);
  }

  return 0;
}

/**
 * @brief Logs a user in the DS server
 *
 * @param uid
 * @param pass
 * @return
 */
int login(char *uid, char *pass)
{
  sprintf(PATH_BUFFER, "USERS/%s/password.txt", uid);
  if (checkFileContent(PATH_BUFFER, pass) == -1)
    return -1; // wrong password NOK

  sprintf(PATH_BUFFER, "USERS/%s/login.txt", uid);
  createFile(PATH_BUFFER, "");
  return 0;
}

/**
 * @brief Logs a user out of the DS server
 *
 * @param uid
 * @param pass
 * @return
 */
int logout(char *uid, char *pass)
{
  sprintf(PATH_BUFFER, "USERS/%s/password.txt", uid);
  if (userLoggedIn(uid) == -1 || checkFileContent(PATH_BUFFER, pass) == -1)
    return -1; // wrong password NOK

  sprintf(PATH_BUFFER, "USERS/%s/login.txt", uid);
  unlink(PATH_BUFFER);
  return 0;
}

int groups(char *buffer)
{
  listGroups(buffer, NULL);
  return 0;
}

/**
 * @brief Subscribes a user to a group
 *
 * @param uid
 * @param gid
 * @param gname
 * @return
 */
int subscribe(char *uid, char *gid, char *gname)
{
  if (atoi(gid) > N_GROUPS || userLoggedIn(uid) == -1)
  {
    return -1; // E_NOK
  }

  if (!strcmp(gid, "00"))
  {
    if (N_GROUPS >= 99)
    {
      return -2; // E_FULL
    }
    else
    {
      N_GROUPS++;
      sprintf(PATH_BUFFER, "GROUPS/%02d", N_GROUPS);
      mkdir(PATH_BUFFER, 0700);
      sprintf(PATH_BUFFER, "GROUPS/%02d/MSG", N_GROUPS);
      mkdir(PATH_BUFFER, 0700);
      sprintf(PATH_BUFFER, "GROUPS/%02d/MSG/num_msg.txt", N_GROUPS);
      createFile(PATH_BUFFER, "0000");
      sprintf(PATH_BUFFER, "GROUPS/%02d/name.txt", N_GROUPS);
      createFile(PATH_BUFFER, gname);
      sprintf(PATH_BUFFER, "GROUPS/%02d/%s.txt", N_GROUPS, uid);
      createFile(PATH_BUFFER, "");

      return N_GROUPS; // NEW GID
    }
  }

  sprintf(PATH_BUFFER, "GROUPS/%s/name.txt", gid);
  if (checkFileContent(PATH_BUFFER, gname) == -1)
    return -1;

  sprintf(PATH_BUFFER, "GROUPS/%s/%s.txt", gid, uid);
  createFile(PATH_BUFFER, "");
  return 0; // E_OK
}

/**
 * @brief Unsubscribes a user from a group
 *
 * @param uid
 * @param gid
 * @return
 */
int unsubscribe(char *uid, char *gid)
{
  if (atoi(gid) > N_GROUPS || userLoggedIn(uid) == -1)
    return -1; // E_NOK

  sprintf(PATH_BUFFER, "GROUPS/%s/%s.txt", gid, uid);
  return unlink(PATH_BUFFER);
}

/**
 * @brief Lists all the groups a user is subscribed to
 *
 * @param uid
 * @return int
 */
int myGroups(char *uid, char *buffer)
{
  if (userLoggedIn(uid) == -1)
  {
    return -1;
  }

  listGroups(buffer, uid);
  return 0;
}

/**
 * @brief
 *
 * @param gid
 * @return
 */
DIR *ulist(char *gid)
{
  if (atoi(gid) > N_GROUPS)
  {
    return NULL;
  }

  sprintf(PATH_BUFFER, "GROUPS/%s", gid);
  return opendir(PATH_BUFFER);
}

/**
 * @brief
 *
 * @param gid
 * @param buffer
 */
void getGName(char *gid, char *buffer)
{
  char gname[25];
  sprintf(PATH_BUFFER, "GROUPS/%s/%s", gid, "name.txt");
  FILE *fPtr = fopen(PATH_BUFFER, "r");
  fscanf(fPtr, "%24s", gname);
  fclose(fPtr);
  sprintf(buffer, " %s", gname);
}

/**
 * @brief
 *
 * @param de
 * @param buffer
 */
void ulsAux(struct dirent *de, char *buffer)
{
  char uid[8];

  if (de->d_name[0] == '.' || !strcmp(de->d_name, "MSG") || !strcmp(de->d_name, "name.txt"))
    return;

  snprintf(uid, 7, " %s", de->d_name);
  strcat(buffer, uid);
}

/**
 * @brief Posts a message in a group
 *
 * @param uid
 * @param gid
 * @param tsize
 * @param text
 * @param fname
 * @param fsize
 * @param data
 * @return
 */
FILE *post(char *uid, char *gid, char *text, char *fname, char *mid)
{

  sprintf(PATH_BUFFER, "GROUPS/%s/%s.txt", gid, uid);
  if (N_GROUPS < atoi(gid) || userLoggedIn(uid) == -1 || fopen(PATH_BUFFER, "r") == NULL)
  {
    return NULL;
  }

  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/num_msg.txt", gid);
  FILE *FPtr = fopen(PATH_BUFFER, "r+");
  bzero(mid, 5);
  fread(mid, sizeof(char), 4, FPtr);
  sprintf(mid, "%04d", atoi(mid) + 1);
  fseek(FPtr, 0, SEEK_SET);
  fputs(mid, FPtr);
  fclose(FPtr);

  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%s", gid, mid);
  mkdir(PATH_BUFFER, 0700);
  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%s/A U T H O R.txt", gid, mid);
  createFile(PATH_BUFFER, uid);

  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%s/T E X T.txt", gid, mid);
  createFile(PATH_BUFFER, text);

  if (fname != NULL)
  {
    sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%s/F I L E.txt", gid, mid);
    createFile(PATH_BUFFER, fname);
    sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%s/%s", gid, mid, fname);
    return fopen(PATH_BUFFER, "wb");
  }

  return FPtr;
}

/**
 * @brief Performs the retrieve command
 *
 * @param uid
 * @param gid
 * @param mid
 * @return
 */
int retrieve(char *uid, char *gid, char *mid)
{
  sprintf(PATH_BUFFER, "GROUPS/%s/%s.txt", gid, uid);
  if (N_GROUPS < atoi(gid) || userLoggedIn(uid) == -1 || fopen(PATH_BUFFER, "r") == NULL)
  {
    return -1;
  }

  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/num_msg.txt", gid);
  FILE *FPtr = fopen(PATH_BUFFER, "r");

  char n_msg[5];
  fread(n_msg, sizeof(char), 5, FPtr);
  fclose(FPtr);

  if (atoi(n_msg) == 0)
  {
    return 0;
  }
  else if (atoi(n_msg) < atoi(mid))
  {
    return -1;
  }

  int to_read = atoi(n_msg) - atoi(mid) + 1;
  if (to_read > 20)
  {
    return 20;
  }

  return to_read;
}

FILE *retrieveAux(char *gid, int mid, char *buffer)
{
  char text[240];
  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%04d/T E X T.txt", gid, mid);
  FILE *FPtr = fopen(PATH_BUFFER, "r");
  bzero(text, sizeof(text));
  fread(text, sizeof(char), 240, FPtr); // TEXT
  fclose(FPtr);
  int len = strlen(text); // TSize

  char uid[6];
  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%04d/A U T H O R.txt", gid, mid);
  FPtr = fopen(PATH_BUFFER, "r");
  bzero(uid, sizeof(uid));
  fread(uid, sizeof(char), 5, FPtr); // UID
  fclose(FPtr);

  char file_name_buffer[25];
  sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%04d/F I L E.txt", gid, mid);
  if ((FPtr = fopen(PATH_BUFFER, "r")) != NULL)
  {
    bzero(file_name_buffer, sizeof(file_name_buffer));
    fread(file_name_buffer, sizeof(char), 24, FPtr); // fname
    fclose(FPtr);

    sprintf(PATH_BUFFER, "GROUPS/%s/MSG/%04d/%s", gid, mid, file_name_buffer);
    FPtr = fopen(PATH_BUFFER, "rb");
    fseek(FPtr, 0L, SEEK_END);
    long size = ftell(FPtr);
    rewind(FPtr);
    sprintf(buffer, " %04d %s %d %s %c %s %lu ", mid, uid, len, text, '/', file_name_buffer, size);
  }
  else
  {
    sprintf(buffer, " %04d %s %d %s", mid, uid, len, text);
  }

  return FPtr;
}

/**
 * @brief Reads from a file
 *
 * @param FPtr
 * @param data
 * @param max_size_read
 * @return int
 */
int ReadFile(FILE *FPtr, char *data, int max_size_read)
{
  return fread(data, sizeof(char), max_size_read, FPtr);
}

/**
 * @brief Writes to a file
 *
 * @param FPtr
 * @param data
 * @param size_write
 */
void WriteToFile(FILE *FPtr, char *data, int size_write)
{
  fwrite(data, sizeof(char), size_write, FPtr);
}

/**
 * @brief Auxiliar function that lists all the groups if uid = NULL, else only lists
 * the groups the user is subscribed to
 *
 * @param buffer
 * @return
 */
void listGroups(char *buffer, char *uid)
{
  char gname[25], mid[5], group_buffer[34], groups_buffer[3267];
  int n_groups = 0, n_messages;

  bzero(groups_buffer, sizeof(groups_buffer));
  for (int i = 1; i <= N_GROUPS; i++)
  {
    n_groups++;
    bool my_group = false;
    sprintf(PATH_BUFFER, "GROUPS/%02d", i);

    n_messages = 0;
    struct dirent *group_entry;
    DIR *group_dir = opendir(PATH_BUFFER);
    while ((group_entry = readdir(group_dir)) != NULL)
    {
      if (group_entry->d_name[0] == '.')
        continue;

      if (uid != NULL)
      {
        sprintf(PATH_BUFFER, "%s.txt", uid);
        if (!strcmp(group_entry->d_name, PATH_BUFFER))
        {
          my_group = true;
        }
      }

      if (!strcmp(group_entry->d_name, "name.txt"))
      {
        sprintf(PATH_BUFFER, "GROUPS/%02d/name.txt", i);
        FILE *fPtr = fopen(PATH_BUFFER, "r");
        fscanf(fPtr, "%24s", gname);
        fclose(fPtr);
      }
      else if (!strcmp(group_entry->d_name, "MSG"))
      {
        sprintf(PATH_BUFFER, "GROUPS/%02d/MSG", i);
        struct dirent *msg_entry;
        DIR *msg_dir = opendir(PATH_BUFFER);
        while ((msg_entry = readdir(msg_dir)) != NULL)
        {
          if (msg_entry->d_name[0] == '.' || msg_entry->d_name[0] == 'n')
            continue;
          n_messages++;
        }
        sprintf(mid, "%04d", n_messages);
        closedir(msg_dir);
      }
    }

    if (uid == NULL || my_group)
    {
      sprintf(group_buffer, " %02d %s %s", i, gname, mid);
      strcat(groups_buffer, group_buffer);
    }
    else
    {
      n_groups--;
    }

    closedir(group_dir);
  }

  if (uid == NULL)
  {
    sprintf(buffer, "RGL %d%s\n", n_groups, groups_buffer);
  }
  else
  {
    sprintf(buffer, "RGM %d%s\n", n_groups, groups_buffer);
  }
}

/**
 * @brief Creates a file with and writes data argument
 *
 * @param filename
 * @param data
 * @return
 */
int createFile(char *filename, char *data)
{
  FILE *fPtr = fopen(filename, "w");
  if (fPtr == NULL)
    return -1;
  if (fputs(data, fPtr) == EOF)
    return -1;
  if (fclose(fPtr) == EOF)
    return -1;
  return 0;
}

/**
 * @brief Checks if file content matches data argument
 *
 * @param filename
 * @param data
 * @return
 */
int checkFileContent(char *filename, char *data)
{
  char pathname[128];
  sprintf(pathname, "%s", filename);
  FILE *fPtr = fopen(pathname, "r");
  if (fPtr == NULL)
    return -1;
  char file_data[25];
  bzero(file_data, 25);
  fread(file_data, sizeof(char), 24, fPtr);
  if (strcmp(data, file_data))
    return -1;
  if (fclose(fPtr) == EOF)
    return -1;
  return 0;
}

/**
 * @brief Verifies if user is logged in
 *
 * @param uid
 * @return int
 */
int userLoggedIn(char *uid)
{
  sprintf(PATH_BUFFER, "USERS/%s/login.txt", uid);
  if (fopen(PATH_BUFFER, "r") == NULL)
  {
    return -1;
  }
  return 0;
}