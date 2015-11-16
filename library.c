#include <stdio.h>
#include <string.h>
#include "library.h"

int arg_finder(char *match,int argc, char *argv[]) {
    for(int i = 1; i < argc; i++) {
        if (strcmp(match, argv[i]) == 0) {
            return i;
        }
    }
    return -1;
}
