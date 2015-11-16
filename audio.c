#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "library.h"

void *listener() {
    return NULL;
}
void audio(char *peer, char *port) {
    printf("%s %s\n", peer, port);
    pthread_t thr_listen;
    pthread_create(&thr_listen, NULL, &listener, port);
}
