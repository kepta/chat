#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
  int socket;
  struct sockaddr_in addr;
  socklen_t addr_len;
} connection_t;

void test();
int arg_finder(char *match,int argc, char *argv[]);
int connection_init(int port, int *sockfd);
