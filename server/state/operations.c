#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "operations.h"

int listGroups(char *buffer);
int createDir(char *DIRNAME);
int deleteDir(char *DIRNAME);
int createFile(char *FILENAME, char *data);
int checkFileContent(char *FILENAME, char *data);
int deleteFile(char *FILENAME);

/**
 * Registers a user on the DS server
 * @param uid
 * @param pass
 * @return
 */
int REG(char *uid, char *pass) {
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
int UNR(char *uid, char *pass) {
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
  for (int i = 0; i < 10; i++) {
    sprintf(buffer2, "GROUPS/0%d/%s.txt", i, uid);
    deleteFile(buffer2);
  }
  for (int i = 10; i < 100; i++) {
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
int LOG(char *uid, char *pass) {
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
int OUT(char *uid, char *pass) {
  char buffer1[29];
  sprintf(buffer1, "USERS/%s/%s_password.txt", uid, uid);
  if (checkFileContent(buffer1, pass) == -1)
    return -1;
  char buffer2[128];
  sprintf(buffer2, "USERS/%s/%s_login.txt", uid, uid);
  deleteFile(buffer2);
}

int GLS(char* buffer) {
  listGroups(buffer);
}

/**
 *
 * @param uid
 * @param gid
 * @param gname
 * @return
 */
int GSR(char *uid, char *gid, char *gname) {

}

/**
 *
 * @param uid
 * @param gid
 * @return
 */
int GUR(char *uid, char *gid) {

}

/**
 *
 * @param uid
 * @return
 */
int GLM(char *uid) {

}

/**
 *
 * @param gid
 * @return
 */
int ULS(char *gid) {

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
int PST(char *uid, char *gid, int tsize, char *text, char *fname, int fsize, char *data) {

}

/**
 *
 * @param uid
 * @param gid
 * @param mid
 * @return
 */
int RTV(char *uid, char *gid, char *mid) {

}

int listGroups(char *buffer) {
  char buffer74[1028];
  buffer74[0] = '\0';
  struct dirent *de;
  DIR *dr = opendir("/home/joao/Downloads/GROUPS");
  char gid[3], name_file[12], gname[25];
  int n_groups = 0, mid = 0;
  char mid_string[5];
  while ((de = readdir(dr)) != NULL) { // GRUPO
    mid = 0;
    if (de->d_name[0] == '.')
      continue;
    n_groups++;
    strcpy(gid, de->d_name);
    sprintf(name_file, "%s_name.txt", gid);
    char buffer0[128];
    sprintf(buffer0, "/home/joao/Downloads/GROUPS/%s", gid);
    struct dirent *de2;
    DIR *dr2 = opendir(buffer0);
    while ((de2 = readdir(dr2)) != NULL) { // Dentro grupo
      if (de2->d_name[0] == '.')
        continue;
      else if (!strcmp(de2->d_name, name_file)) {
        char buffer1[128];
        sprintf(buffer1, "/home/joao/Downloads/GROUPS/%s/%s", gid, name_file);
        FILE *fPtr = fopen(buffer1, "r");
        fscanf(fPtr, "%24s", gname);
        fclose(fPtr);
      } else if (!strcmp(de2->d_name, "MSG")) {
        char buffer2[128];
        sprintf(buffer2, "/home/joao/Downloads/GROUPS/%s/MSG", gid);
        DIR *dr3 = opendir(buffer2);
        while ((de = readdir(dr3)) != NULL) { // dentro de MSG
          if (de->d_name[0] == '.')
            continue;
          mid++;
        }
        snprintf(mid_string, 5, "%04d", mid);
        closedir(dr3);
      }
    }
    char buffer44[128];
    sprintf(buffer44, " %s %s %s", gid, gname, mid_string);
    strcat(buffer74, buffer44);
    closedir(dr2);
  }

  if (n_groups == 0)
    strcpy(buffer, "RGL 0\n");

  sprintf(buffer, "RGL %d%s\n", n_groups, buffer74);
  closedir(dr);
  return 0;
}

/**
 * Creates a directory
 * @param DIRNAME
 * @return
 */
int createDir(char *DIRNAME) {
  char dir_name[128];
  sprintf(dir_name, "/home/joao/Downloads/%s", DIRNAME);
  return mkdir(dir_name, 0700);
}

/**
 * Delete directory
 * @param DIRNAME
 * @return
 */
int deleteDir(char *DIRNAME) {
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
int createFile(char *FILENAME, char *data) {
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
int checkFileContent(char *FILENAME, char *data) {
  char pathname[128];
  sprintf(pathname, "/home/joao/Downloads/%s", FILENAME);
  FILE *fPtr = fopen(pathname, "r");
  if (fPtr == NULL)
    return -1;
  char file_data[9];
  fread(file_data, sizeof(char), 8, fPtr);
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
int deleteFile(char *FILENAME) {
  char pathname[128];
  sprintf(pathname, "/home/joao/Downloads/%s", FILENAME);
  return unlink(pathname);
}

