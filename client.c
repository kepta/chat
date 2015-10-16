#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define SERVERIP "localhost"
// #define SERVERPORT 8004

#define ALIASLEN 40
#define BUFFSIZE 1024
#define OPTLEN 16

struct PACKET {
        char option[OPTLEN]; // instruction
        char alias[ALIASLEN]; // client's alias
        char buff[BUFFSIZE]; // payload
};

struct THREAD_INFO {
    pthread_t thread_ID; // thread's pointer
    int sockfd; // socket file descriptor
    char alias[ALIASLEN]; // client's alias
};


int connect_with_server(int SERVERPORT) {
        int newfd, err_ret;
        struct sockaddr_in serv_addr;
        struct hostent *to;

        /* generate address */
        if((to = gethostbyname(SERVERIP))==NULL) {
                err_ret = errno;
                fprintf(stderr, "gethostbyname() error...\n");
                return err_ret;
        }

        /* open a socket */
        if((newfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                err_ret = errno;
                fprintf(stderr, "socket() error...\n");
                return err_ret;
        }

        /* set initial values */
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(SERVERPORT);
        serv_addr.sin_addr = *((struct in_addr *)to->h_addr);
        memset(&(serv_addr.sin_zero), 0, 8);

        /* try to connect with server */
        if(connect(newfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
                err_ret = errno;
                fprintf(stderr, "connect() error...\n");
                return err_ret;
        }
        else {
                printf("Connected to server at %s:%d\n", SERVERIP, SERVERPORT);
                return newfd;
        }
}

void *server_handler(void *fd) {

struct THREAD_INFO thread_info = *(struct THREAD_INFO *) fd;
struct PACKET pack;

 int n;
 while ((n = recv(thread_info.sockfd, (void *)&pack, sizeof(struct PACKET),0)) > 0)
    {
	if(n < 0){
		perror("Receive from server fail");
		exit(1);	
	}
       printf("%s: %s \n",pack.alias,pack.buff);
    } 
	
	
}
int main(int argc, char *argv[])
{
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;
	
        char buffer[1024];

        if (argc <3) {
                fprintf(stderr,"usage %s hostname port username\n", argv[0]);
                exit(0);
        }
        portno = atoi(argv[2]);

        /* Create a socket point */
        // sockfd = socket(AF_INET, SOCK_STREAM, 0);


        server = gethostbyname(argv[1]);

        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }

        sockfd = connect_with_server(portno);
        if(sockfd >= 0) {
                // char *msg;
                // msg[BUFFSIZE] = 0;

		struct THREAD_INFO thread_info;
                thread_info.sockfd = sockfd;
		
                 pthread_create(&thread_info.thread_ID, NULL, server_handler, ( void *) &thread_info);
                
                while(1) {

			sleep(1);
                        printf("\nPlease enter the messagesss: ");
                        bzero(buffer,1024);
                        struct PACKET packet;
                        fgets(buffer,1024,stdin);
                        memset(&packet, 0, sizeof(struct PACKET));
                        strcpy(packet.option, "Message from user:");
                        strcpy(packet.alias, argv[3]);
                        strcpy(packet.buff, buffer);

/* send request to close this connetion */
                        int sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                        printf("%d\n",sent);

		
                }
		
        }
        else {
                fprintf(stderr, "Connection rejected...\n");
        }

        return 0;
}