#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"

#define ADD_FLAG 'a'
#define REMOVE_FLAG 'r'

void parseOptions(int argc, char const *argv[]);
void addPathToTracked(char const *path);
void removePathFromTracked(char const *path);

#endif