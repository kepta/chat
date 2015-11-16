#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "library.h"

static void *listener(void *args) {
  int socket;
  int port = atoi((char *)args);
  init_udp (port, &socket);
  receive_messages (socket, 4096);
  return NULL;
}
static connection_t *con;
void audio(char *peer, char *port) {
    printf("%s %s\n", peer, port);

    con = malloc(sizeof(connection_t));
    if (connect_udp(peer, port, con)) {
        fprintf(stderr, "Unable to connect to server.\n");
    } else {
        printf("udp established\n");
    }

    pthread_t thr_listen;
    pthread_create(&thr_listen, NULL, &listener, port);
    char buf[16];
    strcpy(buf, "hello world");
    while (1) {
        udp_send(con, buf, sizeof(buf));
    }
}
