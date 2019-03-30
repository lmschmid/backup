#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char * bashifyPath(char *path);

int main(int argc, char const *argv[])
{
    /* code */
    char * t;
    asprintf(&t, "%s", "This is a test path/dean/to the goal/");
    t = bashifyPath(t);

    printf("%s\n", t);

    free(t);
    return 0;
}

char * bashifyPath(char *path) {
    int pathLen = strlen(path), newLen = strlen(path), i, index = 0;
    char * newPath = (char *)malloc(pathLen+1);

    for(i = 0; i < pathLen; i++) {
        if (path[i] == ' ') {
            newPath = (char *)realloc(newPath, newLen+2);
            newLen += 1;
            newPath[index++] = '\\';
            newPath[index++] = ' ';
        } else {
            newPath[index++] = path[i];
        }
    }

    for (;i < pathLen; i++) {
        newPath[index++] = path[i];
    }

    free(path);
    return newPath;
}
