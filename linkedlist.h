#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "macros.h"

typedef struct pathNode {
  char * localPath;
  char * usbPath;
  struct pathNode * next;
} pathNode;

pathNode * createNode(char *path);
void freeNode(pathNode *n);
pathNode * addPath(pathNode *head, char *path);
pathNode * removePath(pathNode *head, char *path);
int listSize(pathNode *head);
int listIsEmpty(pathNode *head);
void freePathList(pathNode *head);
void printPL(pathNode * head);

#endif