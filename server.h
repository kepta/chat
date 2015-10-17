#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define MAXSIZE 1024
typedef struct user {
        char name[MAXSIZE];
        int fd;
        struct user *next;
} User;

void server(int argc, char *argv[], int sockfd, int port);
void *client_handler(void *fd);
void login_user(User *user, char *name, int fd) ;
