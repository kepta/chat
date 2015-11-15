#include <stdio.h>
#include <string.h>

#include "definition.h"


void remove_nasty(char *str, int len) {
    for(int i = 0 ; i < len ; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            return;
        }
        if (str[i] == '\0') {
            return ;
        }
    }
}
void clean_arguments() {
    for (int i = 1 ; i < ARGC ; i++) {
        remove_nasty(ARGV[i], strlen(ARGV[i]));
    }
}

int arg_finder(char *match) {
    for(int i = 1; i < ARGC; i++) {
        if (strcmp(match, ARGV[i]) == 0) {
            return i;
        }
    }
    return -1;
}
