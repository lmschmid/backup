#include "linkedlist.h"

pathNode * createNode(char *path) {
    pathNode * newNode = (pathNode *)malloc(sizeof (pathNode));
    newNode->localPath = NULL;
    newNode->usbPath = NULL;

    asprintf(&newNode->localPath, "%s%s", LOCAL_START, path);
    asprintf(&newNode->usbPath, "%s%s", USB_START, path);

    newNode->next = NULL;

    return newNode;
}

void freeNode(pathNode *n) {
    free(n->localPath);
    free(n->usbPath);
    free(n);
}

pathNode * addPath(pathNode *head, char *path) {
    pathNode *temp = NULL;

    if (!head) {
      return createNode(path);
    }

    temp = head;
    while(temp->next) {
      temp = temp->next;
    }

    temp->next = createNode(path);

    return head;
}

pathNode * removePath(pathNode *head, char *path) {
    pathNode *temp, *prev;

    if (!head) {
        return head;
    }

    if (strcmp(path, head->localPath) == 0) {
        temp = head->next;
        freeNode(head);
        return temp;
    }

    temp = head->next;
    prev = head;

    while (temp) {
        if (strcmp(path, temp->localPath) == 0) {
            prev->next = temp->next;
            freeNode(temp);
            break;
        }

        prev = temp;
        temp = temp->next;
    }

    return head;
}

int listSize(pathNode *head) {
    pathNode *temp = head;
    int size = 0;

    while (temp) {
        size++;
        temp = temp->next;
    }

    return size;
}

int listIsEmpty(pathNode *head) {
  return !head;
}

void freePathList(pathNode *head) {
    pathNode *temp = head, *next;

    while (temp) {
        next = temp->next;

        freeNode(temp);
        temp = next;
    }
}

void printPL(pathNode * head) {
    pathNode *temp = head;

    while (temp) {
        printf("loc:%s usb:%s -> \n", temp->localPath, temp->usbPath);
        temp = temp->next;
    }
    printf("NULL\n");
}
