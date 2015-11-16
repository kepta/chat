#include <stdio.h>
#include <stdlib.h>
#include "library.h"

static void *listener(void *args) {

  int socket;
  int port = atoi((char *)args);
  init_udp (port, &socket);
  receive_messages (socket, 4096);
  return NULL;
}

void audio(char *peer, char *port) {

    printf("%s %s\n", peer, port);
    pthread_t thr_listen;
    pthread_create(&thr_listen, NULL, &listener, port);
}
