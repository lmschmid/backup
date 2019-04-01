#ifndef BACKUP_H
#define BACKUP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h> 
#include <dirent.h> 

#include "macros.h"
#include "linkedlist.h"

typedef struct threadArgs {
    char *localPath;
    char *usbPath;
} threadArgs;

void fillPathsList(pathNode ** pathList);
void checkBaseDirs(pathNode * pathList);
void processPaths(pathNode *pathList);
void update(char *localPath, char *usbPath);
void *thrUpdate(void *args);
void updateDirectory(char *localDir, char *usbDir);
void updateFile(char *localFile, char *usbFile);
int checkAccessDate(struct stat *localStat, struct stat *usbStat);

#endif