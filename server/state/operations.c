#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "operations.h"

void listGroups(char *buffer, char *uid);
int createDir(char *DIRNAME);
int deleteDir(char *DIRNAME);
int createFile(char *FILENAME, char *data);
int checkFileContent(char *FILENAME, char *data);
int deleteFile(char *FILENAME);

int n_groups = 0;

/**
 * Registers a user on the DS server
 * @param uid
 * @param pass
 * @return
 */
int REG(char *uid, char *pass)
{
  char buffer1[12];
  sprintf(buffer1, "USERS/%s", uid);
  if (createDir(buffer1) == -1)
    return -1; // DUP
  char buffer2[29];
  sprintf(buffer2, "USERS/%s/%s_password.txt", uid, uid);
  createFile(buffer2, pass);
  return 0;
}

/**
 * Unregisters a user from the DS server
 * @param uid
 * @param pass
 * @return
 */
int UNR(char *uid, char *pass)
{
  char buffer2[29];
  sprintf(buffer2, "USERS/%s/%s_password.txt", uid, uid);
  if (checkFileContent(buffer2, pass) == -1)
    return -1;
  char buffer1[12];
  sprintf(buffer1, "USERS/%s", uid);
  deleteFile(buffer2);
  sprintf(buffer2, "USERS/%s/%s_login.txt", uid, uid);
  deleteFile(buffer2);
  deleteDir(buffer1);
  for (int i = 0; i < 10; i++)
  {
    sprintf(buffer2, "GROUPS/0%d/%s.txt", i, uid);
    deleteFile(buffer2);
  }
  for (int i = 10; i < 100; i++)
  {
    sprintf(buffer2, "GROUPS/%d/%s.txt", i, uid);
    deleteFile(buffer2);
  }
  return 0;
}

/**
 * Logs a user in the DS server
 * @param uid
 * @param pass
 * @return
 */
int LOG(char *uid, char *pass)
{
  char buffer1[29];
  sprintf(buffer1, "USERS/%s/%s_password.txt", uid, uid);
  if (checkFileContent(buffer1, pass) == -1)
    return -1;
  char buffer2[128];
  sprintf(buffer2, "USERS/%s/%s_login.txt", uid, uid);
  createFile(buffer2, "");
}

/**
 * Logs a user out of the DS server
 * @param uid
 * @param pass
 * @return
 */
int OUT(char *uid, char *pass)
{
  char buffer1[29];
  sprintf(buffer1, "USERS/%s/%s_password.txt", uid, uid);
  if (checkFileContent(buffer1, pass) == -1)
    return -1;
  char buffer2[128];
  sprintf(buffer2, "USERS/%s/%s_login.txt", uid, uid);
  deleteFile(buffer2);
}

int GLS(char *buffer)
{
  listGroups(buffer, NULL);
}

/**
 *
 * @param uid
 * @param gid
 * @param gname
 * @return
 */
int GSR(char *uid, char *gid, char *gname)
{
  char path_buffer[256];
  if (atoi(gid) > n_groups)
    return -1; // E_NOK

  if (!strcmp(gid, "0"))
  {
    if (n_groups < 99)
    {
      n_groups++;
      sprintf(path_buffer, "GROUPS/%02d", n_groups);
      createDir(path_buffer);
      sprintf(path_buffer, "GROUPS/%02d/MSG", n_groups);
      createDir(path_buffer);
      sprintf(path_buffer, "GROUPS/%02d/MSG/num_msg.txt", n_groups, n_groups);
      createFile(path_buffer, "0000");
      sprintf(path_buffer, "GROUPS/%02d/%02d_name.txt", n_groups, n_groups);
      createFile(path_buffer, gname);
      sprintf(path_buffer, "GROUPS/%02d/%s.txt", n_groups, uid);
      createFile(path_buffer, "");
      return n_groups; // NEW GID
    }
    else
    {
      return -2;
    } // E_FULL
  }
  sprintf(path_buffer, "GROUPS/%s/%s_name.txt", gid, gid);
  if (checkFileContent(path_buffer, gname) == -1)
    return -1;

  sprintf(path_buffer, "GROUPS/%s/%s.txt", gid, uid);
  createFile(path_buffer, "");
  return 0; // E_OK
}

/**
 *
 * @param uid
 * @param gid
 * @return
 */
int GUR(char *uid, char *gid)
{
  if (atoi(gid) > n_groups)
    return -1; // E_NOK

  char path_buffer[256];
  sprintf(path_buffer, "GROUPS/%s/%s.txt", gid, uid);
  return deleteFile(path_buffer);
}

/**
 *
 * @param uid
 * @return
 */
int GLM(char *uid, char *buffer)
{
  char path_buffer[256];
  sprintf(path_buffer, "/home/joao/Downloads/USERS/%s/%s_login.txt", uid, uid);
  if (fopen(path_buffer, "r") == NULL)
  {
    return -1;
  }
  listGroups(buffer, uid);
  return 0;
}

/**
 *
 * @param gid
 * @return
 */
int ULS(char *gid, char *buffer)
{
  if (atoi(gid) > n_groups)
  {
    strcpy(buffer, "RUL NOK\n");
    return 0;
  }

  char path_buffer[256], aux_buffer[599994] = {'\0'};
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s", gid);
  struct dirent *de;
  DIR *dr = opendir(path_buffer);
  sprintf(path_buffer, "%s_name.txt", gid);
  while ((de = readdir(dr)) != NULL)
  {
    if (!strcmp(de->d_name, path_buffer))
    {
      char path_buffer2[256], gname[25];
      sprintf(path_buffer2, "/home/joao/Downloads/GROUPS/%s/%s", gid, path_buffer);
      FILE *fPtr = fopen(path_buffer2, "r");
      fscanf(fPtr, "%24s", gname);
      fclose(fPtr);
      sprintf(buffer, "RUL OK %s", gname);
      continue;
    }

    if (de->d_name[0] == '.' || !strcmp(de->d_name, "MSG"))
      continue;

    char uid[7];
    snprintf(uid, 7, " %s", de->d_name);
    strcat(aux_buffer, uid);
  }
  strcat(aux_buffer, "\n");
  strcat(buffer, aux_buffer);
  return 0;
}

/**
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
FILE *PST(char *uid, char *gid, int tsize, char *text, char *fname, int fsize, char *data, int size_read, char *mid)
{
  char path_buffer[256];

  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/%s.txt", gid, uid);
  if (n_groups < atoi(gid) || fopen(path_buffer, "r") == NULL)
  {
    return NULL; // NOK
  }

  bzero(mid, 5);
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/num_msg.txt", gid);
  FILE *FPtr = fopen(path_buffer, "r+");
  fread(mid, sizeof(char), 4, FPtr);
  sprintf(mid, "%04d", atoi(mid) + 1);
  fseek(FPtr, 0, SEEK_SET);
  fputs(mid, FPtr);
  fclose(FPtr);

  sprintf(path_buffer, "GROUPS/%s/MSG/%s", gid, mid);
  createDir(path_buffer);
  sprintf(path_buffer, "GROUPS/%s/MSG/%s/A U T H O R.txt", gid, mid);
  createFile(path_buffer, uid);

  sprintf(path_buffer, "GROUPS/%s/MSG/%s/T E X T.txt", gid, mid);
  createFile(path_buffer, text);

  if (fname != NULL)
  {
    sprintf(path_buffer, "GROUPS/%s/MSG/%s/F I L E.txt", gid, mid);
    createFile(path_buffer, fname);
    sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/%s/%s", gid, mid, fname);
    FPtr = fopen(path_buffer, "wb");
    WriteFile(FPtr, data, size_read);
  }

  return FPtr;
}

/**
 *
 * @param uid
 * @param gid
 * @param mid
 * @return
 */
int RTV(char *uid, char *gid, char *mid)
{
  char path_buffer[256];
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/%s.txt", gid, uid);
  if (n_groups < atoi(gid) || fopen(path_buffer, "r") == NULL)
  {
    return -1;
  }

  char n_msg[5];
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/num_msg.txt", gid);
  FILE *FPtr = fopen(path_buffer, "r");
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

FILE *RTVAux(char *gid, int mid, char *buffer)
{
  char mid_aux[5];
  sprintf(mid_aux, "%04d", mid); // MID

  char path_buffer[256];
  FILE *FPtr;
  char text[240];
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/%s/T E X T.txt", gid, mid_aux);
  FPtr = fopen(path_buffer, "r");
  fread(text, sizeof(char), 240, FPtr); // TEXT
  fclose(FPtr);
  int len = strlen(text); // TSize

  char uid[6];
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/%s/A U T H O R.txt", gid, mid_aux);
  FPtr = fopen(path_buffer, "r");
  fread(uid, sizeof(char), 5, FPtr); // UID
  fclose(FPtr);

  char file_name_buffer[25];
  bzero(file_name_buffer, 25);
  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/%s/F I L E.txt", gid, mid_aux);
  FPtr = fopen(path_buffer, "r");
  fread(file_name_buffer, sizeof(char), 24, FPtr); // fname
  fclose(FPtr);

  sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG/%s/%s", gid, mid_aux, file_name_buffer);
  if ((FPtr = fopen(path_buffer, "rb")) != NULL)
  {
    printf("fseek");
    fseek(FPtr, 0L, SEEK_END);
    long size = ftell(FPtr);
    rewind(FPtr);
    sprintf(buffer, " %s %s %s %s %s %lu ", mid_aux, uid, len, text, file_name_buffer, size);
  }
  else
  {
    sprintf(buffer, " %s %s %s %s", mid_aux, uid, len, text);
  }

  printf("Acaba o RTVAux\n");
  fflush(stdout);

  return FPtr;
}

/**
 * @brief 
 * 
 * @param gid 
 * @param mid 
 * @param data 
 */
int ReadFile(FILE *FPtr, char *data, int size_read)
{
  return fread(data, sizeof(char), size_read, FPtr);
}

/**
 * @brief 
 * 
 * @param gid 
 * @param mid 
 * @param data 
 */
void WriteFile(FILE *FPtr, char *data, int size_read)
{
  fwrite(data, sizeof(char), size_read, FPtr);
}

/**
 * Lists Groups
 * @param buffer
 * @return
 */
void listGroups(char *buffer, char *uid)
{
  char gid[3], name_file[12], gname[25], mid[5], path_buffer[256], group_buffer[32], groups_buffer[3301] = {'\0'};
  int n_groups2 = 0, n_messages;

  struct dirent *de;
  DIR *dr = opendir("/home/joao/Downloads/GROUPS");
  while ((de = readdir(dr)) != NULL)
  {
    bool my_group = false;
    if (de->d_name[0] == '.')
      continue;

    n_groups2++;
    strcpy(gid, de->d_name);
    sprintf(name_file, "%s_name.txt", gid);
    sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s", gid);

    n_messages = 0;
    struct dirent *de2;
    DIR *dr2 = opendir(path_buffer);
    while ((de2 = readdir(dr2)) != NULL)
    {
      if (de2->d_name[0] == '.')
        continue;

      if (uid != NULL)
      {
        sprintf(path_buffer, "%s.txt", uid);
        if (!strcmp(de2->d_name, path_buffer))
        {
          my_group = true;
        }
      }

      if (!strcmp(de2->d_name, name_file))
      {
        sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/%s", gid, name_file);
        FILE *fPtr = fopen(path_buffer, "r");
        fscanf(fPtr, "%24s", gname);
        fclose(fPtr);
      }
      else if (!strcmp(de2->d_name, "MSG"))
      {
        sprintf(path_buffer, "/home/joao/Downloads/GROUPS/%s/MSG", gid);
        struct dirent *de3;
        DIR *dr3 = opendir(path_buffer);
        while ((de3 = readdir(dr3)) != NULL)
        {
          if (de3->d_name[0] == '.')
            continue;
          n_messages++;
        }
        sprintf(mid, "%04d", n_messages);
        closedir(dr3);
      }
    }

    if (uid == NULL || my_group)
    {
      sprintf(group_buffer, " %s %s %s", gid, gname, mid);
      strcat(groups_buffer, group_buffer);
    }
    else
    {
      n_groups2--;
    }

    closedir(dr2);
  }
  closedir(dr);

  if (uid == NULL)
    sprintf(buffer, "RGL %d%s\n", n_groups2, groups_buffer);
  else
    sprintf(buffer, "RGM %d%s\n", n_groups2, groups_buffer);
}

/**
 * Creates a directory
 * @param DIRNAME
 * @return
 */
int createDir(char *DIRNAME)
{
  char dir_name[128];
  sprintf(dir_name, "/home/joao/Downloads/%s", DIRNAME);
  return mkdir(dir_name, 0700);
}

/**
 * Delete directory
 * @param DIRNAME
 * @return
 */
int deleteDir(char *DIRNAME)
{
  char user_dirname[128];
  sprintf(user_dirname, "/home/joao/Downloads/%s", DIRNAME);
  return rmdir(user_dirname);
}

/**
 * Creates a file
 * @param FILENAME
 * @param data
 * @return
 */
int createFile(char *FILENAME, char *data)
{
  char pathname[128];
  sprintf(pathname, "/home/joao/Downloads/%s", FILENAME);
  FILE *fPtr = fopen(pathname, "w");
  if (fPtr == NULL)
    return -1;
  if (fputs(data, fPtr) == EOF)
    return -1;
  if (fclose(fPtr) == EOF)
    return -1;
  return 0;
}

/**
 * Checks if has a certain content
 * @param FILENAME
 * @param data
 * @return
 */
int checkFileContent(char *FILENAME, char *data)
{
  char pathname[128];
  sprintf(pathname, "/home/joao/Downloads/%s", FILENAME);
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
 * Deletes a file
 * @param FILENAME
 * @return
 */
int deleteFile(char *FILENAME)
{
  char pathname[128];
  sprintf(pathname, "/home/joao/Downloads/%s", FILENAME);
  return unlink(pathname);
}
