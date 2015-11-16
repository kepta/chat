#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "library.h"

static void *listen() {

}
void audio(char *peer, char *port) {
    printf("%s %s\n", peer, port);
    pthread_t thr_listen;
    pthread_create(&thr_listen, NULL, &listen, port);
}
