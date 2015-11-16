#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <time.h>
#include "library.h"

int arg_finder(char *match,int argc, char *argv[]) {
    for(int i = 1; i < argc; i++) {
        if (strcmp(match, argv[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void test() {
    printf("shit works\n");
}

int connect_udp(char *server_name, char *server_port, connection_t *con) {
  struct addrinfo protocol_spec;
  struct addrinfo *possible_addrs, *curr_addr;
  int err = 0;
  memset(&protocol_spec, 0, sizeof(struct addrinfo));

  protocol_spec.ai_family = AF_INET;
  protocol_spec.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  protocol_spec.ai_flags = AI_PASSIVE; /* For wildcard IP address */
  protocol_spec.ai_protocol = IPPROTO_UDP; /* UDP protocol */
  protocol_spec.ai_canonname = NULL;
  protocol_spec.ai_addr = NULL;
  protocol_spec.ai_next = NULL;

  if ((err = getaddrinfo(server_name, server_port,
          &protocol_spec, &possible_addrs)) != 0) {
    fprintf(stderr, "error in getaddrinfo %s\n", strerror(errno));
    return (err);
  }

  curr_addr = possible_addrs;
  while (curr_addr != NULL) {
    con->socket = socket(curr_addr->ai_family, curr_addr->ai_socktype, curr_addr->ai_protocol);
    if (con->socket > 0) {
      break;
    }
    curr_addr = curr_addr->ai_next;
  }

  if (curr_addr == NULL) {
    fprintf(stderr, "unable to find a server in create_client\n");
    freeaddrinfo(possible_addrs);
    return (ENOTCONN);
  }

  int port;
  sscanf(server_port, "%d", &port);
  con->addr.sin_family = curr_addr->ai_family;
  con->addr.sin_port = htons(port);

  if (curr_addr->ai_family == AF_INET) {
    memcpy((void *)&con->addr.sin_addr, &((struct sockaddr_in *)curr_addr->ai_addr)->sin_addr, curr_addr->ai_addrlen);
  } else {
    fprintf(stderr, "Unable to use ai_family returned.");
    freeaddrinfo(possible_addrs);
    return (EINVAL);
  }

  /* TODO why does this need to be commented out? */
  //freeaddrinfo(possible_addrs);
  con->addr_len = sizeof(con->addr);

  return 0;
}
int init_udp(int port, int *sockfd) {
  int _sockfd_local;
  struct sockaddr_in me;

  /* create socket */
  if ((_sockfd_local = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    fprintf(stderr, "error in opening socket: %s\n", strerror(errno));
    return (errno);
  }

  /* If caller wants, populate socket fd */

  if (sockfd != NULL) {
    *sockfd = _sockfd_local;
  }

  /* XXX: Make sure these fields are OK, possibly switch to variables.*/
  memset(&me, 0, sizeof(me));
  me.sin_family = AF_INET;
  me.sin_port = htons(port);
  me.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(_sockfd_local, (struct sockaddr *)&me, sizeof(me)) == -1) {
    fprintf(stderr, "error in binding socket: %s\n", strerror(errno));
    return (errno);
  }

  /* success */
  return 0;
}


int udp_send(connection_t *con, const void *buf, size_t buflen) {
    return sendto(con->socket, buf, buflen, UDP_FLAGS, (struct sockaddr *)&con->addr, con->addr_len);
}
// UDP Listener code!

int receive_messages (
    void (*callback)(connection_t *, void *, size_t),
    int socket,
    unsigned long max_packet_size)               {

  /* A stack allocated connection struct to store any data
     about the connection we recieve. */

  connection_t con;
  char buf[max_packet_size];

  /* Loop on recvfrom. */
  while (1) {
    memset(buf, 0, max_packet_size);
    int recv_len = recvfrom(socket, buf, max_packet_size, 0, (struct sockaddr *)&(con.addr), &(con.addr_len));

// #ifdef __linux__
// /* Temporarily disable bandwidth.  Broken for OSX. */
//     if (delta == -1) {
//       clock_gettime(CLOCK_MONOTONIC, &prevPacket);
//       delta = 0;
//     } else {
//       clock_gettime(CLOCK_MONOTONIC, &currPacket);
//       delta = currPacket.tv_nsec - prevPacket.tv_nsec;
//       clock_gettime(CLOCK_MONOTONIC, &prevPacket);
//       bandwidth_index = (bandwidth_index + 1) % BANDWIDTH_BUFLEN;
//       bandwidth_buf[bandwidth_index] = (double)recv_len/delta;
//     }
// #endif

    /* Handle error UDP style (try again). */
    if (recv_len < 0) {
      fprintf(stderr, "Recieve failed. errno: %d\n", errno);
      continue;
    }
//    printf("%s\n",buf);
    (*callback)(&con, buf, recv_len);

  }

  return -1;
}
