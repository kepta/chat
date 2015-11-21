#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define UDP_FLAGS 0

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
int receive_messages (  void (*callback)(connection_t *, void *, size_t),int socket, unsigned long max_packet_size  ) ;
int udp_send(connection_t *con, const void *buf, size_t buflen);

