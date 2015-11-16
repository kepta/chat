#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "library.h"

static void *listen() {
  
  int socket;
  int port = atoi((char *)args);
  connection_init (port, &socket);
  receive_messages (&callback, socket, 4096);

}

void audio(char *peer, char *port) {
   
    printf("%s %s\n", peer, port);
    pthread_t thr_listen;
    pthread_create(&thr_listen, NULL, &listen, port);
    
}
