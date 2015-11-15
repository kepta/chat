#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "definition.h"
#include "server.h"
#include "client.h"
#include "misc.h"


void interrup_handler(int sig_num);

int main( int argc, char *argv[]) {
    ARGC = argc;
    ARGV = argv;
    clean_arguments();

    int sockfd, portno;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
            perror("ERROR opening socket");
            exit(1);
    }

    if (arg_finder("-p") != -1) {
        portno = atoi(argv[arg_finder("-p") +1]);
        printf("on port %d\n", portno);
    } else {
        perror("Please specify a port with -p XXXX\n");
        exit(1);
    }

    signal(SIGINT, interrup_handler);



    if (arg_finder("-server") != -1) {
        server(argc, argv, sockfd, portno);
    }
    else {
        client(argc, argv, sockfd, portno);
    }
}

void interrup_handler(int sig_num) {
        signal(SIGINT, interrup_handler);
        printf("\n Termination \n");
        exit(1);
}
