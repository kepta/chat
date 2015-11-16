#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#ifndef LIB_H
#define LIB_H

typedef struct {
  int socket;
  struct sockaddr_in addr;
  socklen_t addr_len;
} connection_t;

#endif
void test();
int arg_finder(char *match,int argc, char *argv[]);
int connect_udp(char *server, char *port, connection_t *con);
int init_udp(int port, int *sockfd);
