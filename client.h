#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>


void client(int argc, char *argv[], int sockfd, int port);
void connect_with_server(int sockfd, int SERVERPORT, char *SERVERIP);
void *server_handler(void *fd);
