#include "backup.h"

/*  ***************************************************************************
    Updates the usb files which are outdated.

    Notes: 
        - Assumes dirs branch from starting paths configurable from backup.h
    ***************************************************************************
*/ 
int main(int argc, char const *argv[]) {
    pathNode *pathList = NULL;
    clock_t t;

    parseOptions(argc, argv);

    fillPathsList(&pathList);
    checkBaseDirs(pathList);

    t = clock();
    processPaths(pathList);
    t = clock() - t;
    printf("Backup executed in %f seconds\n", ((double)t)/CLOCKS_PER_SEC);
    freePathList(pathList);
    return 0;
}

/*  ***************************************************************************
    Loops through the paths in pathList, and spawns a unique thread to update
    each path
    ***************************************************************************
*/
void processPaths(pathNode *pathList) {
    pathNode *current = pathList;
    pthread_t threadIds[listSize(pathList)];
    threadArgs tArgs[listSize(pathList)];
    int numThreads = 0, i;

    while (current) {
        asprintf(&(tArgs[numThreads].localPath), "%s", current->localPath);
        asprintf(&(tArgs[numThreads].usbPath), "%s", current->usbPath);

        if (pthread_create(&threadIds[numThreads], NULL, thrUpdate,
         &tArgs[numThreads])) {
            fprintf(stderr, "error: pthread_create\n");
            exit(-1);
        }

        numThreads++;
        current = current->next;
    }

    for (i = 0; i < numThreads; i++) { // wait for all threads to complete
        pthread_join(threadIds[i], NULL);
        // printf("thread %d joined\n", i); // DEBUGG
		free(tArgs[i].localPath);
		free(tArgs[i].usbPath);
    }
}

/*  ***************************************************************************
    Intermediate function to update for use with multithreading
    ***************************************************************************
*/
void *thrUpdate(void *args) {
    threadArgs *tArgs = (threadArgs *)args;

    // printf("thread local: %s\nthread usb: %s\n", tArgs->localPath,
    //  tArgs->usbPath); // DEBUG
    update(tArgs->localPath, tArgs->usbPath);
    pthread_exit(NULL);
}


/*  ***************************************************************************
    Updates the given path, regardless of file or dir
    - used to eliminate duplicate code of stat calls and file/dir check
    ***************************************************************************
*/
void update(char *local, char *usb) {
    struct stat localStat, usbStat;
    char *gitPath = NULL, *localPath = NULL, *usbPath = NULL, *dirCom = NULL;

    // printf("checking to update: %s & %s\n", localPath, usbPath); // DEBUG

    asprintf(&localPath, "%s", local);
    asprintf(&usbPath, "%s", usb);

    if (stat(localPath, &localStat) != 0) {
        fprintf(stderr, "Stat call failed in updateDir for file: %s\n",
         localPath);
        exit(-1);
    }

    asprintf(&gitPath, "%s/.git", localPath);
    if (access(gitPath, F_OK) == 0) { // if directory contains .git, return
        free(gitPath);
        return;
    }

    if (stat(usbPath, &usbStat) != 0) { // if doesnt exist on usb, create it
        if (S_ISDIR(localStat.st_mode)) {
            mkdir(usbPath, 0700);
        } else {
            updateFile(localPath, usbPath);
            free(gitPath);
            return;
        }
    }

    if (S_ISDIR(localStat.st_mode)) {
        if (localPath[strlen(localPath)-1] != '/') {
            asprintf(&localPath, "%s/", localPath);
            asprintf(&usbPath, "%s/", usbPath);
        }

        updateDirectory(localPath, usbPath);
    }
    // if it is more recent on local update it
    else if (checkAccessDate(&localStat, &usbStat) != 0) {
        // printf("local %s more recent than usb %s (%ld > %ld)\n", localPath, // DEBUG
        //   usbPath, localStat.st_mtime, usbStat.st_mtime);
        updateFile(localPath, usbPath);
    }

    if (dirCom)
        free(dirCom);

    free(usbPath);
    free(localPath);
    free(gitPath);
}

/*  ***************************************************************************
    Updates files in a given directory if the access date is more recent
    ***************************************************************************
*/
void updateDirectory(char *localDir, char *usbDir) {
    struct dirent *dirEntry = NULL;
    DIR *dirPointer = NULL;
    char *localPath = NULL, *usbPath = NULL;
    
    if ((dirPointer = opendir(localDir)) == NULL) {
        fprintf(stderr, "could not open dir: %s\n", localDir);
        return;
    }

    while ((dirEntry = readdir(dirPointer)) != NULL) { // get next file/dir in current dir
        if (dirEntry->d_name[0] == '.') {
            continue;
        }
        asprintf(&localPath, "%s%s", localDir, dirEntry->d_name);
        asprintf(&usbPath, "%s%s", usbDir, dirEntry->d_name);
        
        update(localPath, usbPath);
    }
    
    if (localPath)
        free(localPath);
    if (usbPath)
        free(usbPath);
    closedir(dirPointer); 
}

/*  ***************************************************************************
    Updates the given file on the usb
    ***************************************************************************
*/
void updateFile(char *localFile, char *usbFile) {
    char *command = NULL;

    asprintf(&command, "cp \"%s\" \"%s\"", localFile, usbFile);

    // printf("updating file: %s\n", command); // DEBUGG
    
    if (system(command) < 0) {
        fprintf(stderr, "system call failed in updateFile\n");
        exit(-1);
    }

    // printf("*** Copied %s to usb. *** \n\n", localFile);

    free(command);
}

/*  ***************************************************************************
    Compares the given stats for if local is more recently updated
    - Return:   0 for equal
                -1 for unequal
    ***************************************************************************
*/
int checkAccessDate(struct stat *localStat, struct stat *usbStat) {
    return ((*localStat).st_mtime > (*usbStat).st_mtime+LENIANCY) ? -1 : 0;
}

/*  ***************************************************************************
    Fills a linked list with paths located in PATH_FILE for later processing
    ***************************************************************************
*/
void fillPathsList(pathNode ** pathList) {
    FILE * pathsFile = NULL;
    char * path = NULL;
    size_t len = 0;
    ssize_t read = 0;

    if ((pathsFile = fopen(PATH_FILE, "r")) == NULL) {
        fprintf(stderr, "Could not open file %s\n", PATH_FILE);
        exit(-1);
    }

    while ((read = getline(&path, &len, pathsFile)) != -1) {
        if (path[read-1] == '\n') { // last path in file may not have newline
            path[read-1] = '\0';
        }
        *pathList = addPath(*pathList, path);
    }

    if (path)
        free(path);
    fclose(pathsFile);
}

/*  ***************************************************************************
    Creates the base dir of the path on usb if it does not exist
    - solves problem of multi-level dirs in .paths/paths.txt where
        intermediates do not exist
    ***************************************************************************
*/
void checkBaseDirs(pathNode * pathList) {
    struct stat pathStat;
    char *dirCommand = NULL;
    pathNode *current = pathList;
    
    while (current) {
        if (stat(current->usbPath, &pathStat) != 0) {
            asprintf(&dirCommand, "mkdir -p \"%s\"", current->usbPath);
            if (system(dirCommand) < 0) {
                fprintf(stderr, "system call failed in checkBaseDirs\n");
                exit(-1);
            }
        }

        current = current->next;
    }

    if (dirCommand)
        free(dirCommand);
}