#include "options.h"


/*  ***************************************************************************
    Scans command line arguments provided and performs actions as needed
    ***************************************************************************
*/
void parseOptions(int argc, char const *argv[]) {
    int i;

    for (i = 1; i < argc; i++) {
        if ((int)argv[i][0] == '-' && strlen(argv[i]) > 1 && argc > i+1) {
            switch (tolower((int)argv[i][1])) {
                case ADD_FLAG:
                    addPathToTracked(argv[++i]);
                    break;
                case REMOVE_FLAG:
                    removePathFromTracked(argv[++i]);
                    break;
            }
        } else {
            printf("Ambiguous arg '%s' passed\n", argv[i]);
        }
    }
}

/*  ***************************************************************************
    Adds path to the paths file specified in macros.h
    - Notes: will make sure the path is a valid directory on the local machine
    ***************************************************************************
*/
void addPathToTracked(char const *path) {
    FILE *pathFile = NULL;

    if (access(path, F_OK) == -1) {
        printf("Invlaid path '%s' passed\n", path);
        return;
    } 
    
    pathFile = fopen(PATH_FILE, "a");
    fprintf(pathFile, "\n%s", path);

    printf("Success adding path '%s'\n", path);

    fclose(pathFile);
}

/*  ***************************************************************************
    Removes path from paths file specified in macros.h
    ***************************************************************************
*/
void removePathFromTracked(char const *path) {
    char * filePath = NULL;
    size_t len = 0;
    ssize_t read = 0;
    FILE *pathFile = fopen(PATH_FILE, "r");

    while ((read = getline(&filePath, &len, pathFile)) != -1) {
        if (filePath[read-1] == '\n') { // last path in file may not have newline
            filePath[read-1] = '\0';
        }

        
    }

    if (filePath)
        free(filePath);
    fclose(pathFile);

    fclose(pathFile);
}