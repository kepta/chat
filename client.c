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
	char connectTo[20];
	int list;
	char *names[20];
	int nameCounter;

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

int bytes;
struct THREAD_INFO thread_info = *(struct THREAD_INFO *) fd;
struct PACKET pack;

  while(1) {
        bytes = recv(thread_info.sockfd,  (void *)&pack, sizeof(struct PACKET), 0);
        if (!bytes) { 
	    perror("No bytes received at client");
	    exit(0);
       }
	if(pack.list == 1){
	printf("List of online users is:\n");
	int i;
	for( i = 0 ;i < pack.nameCounter ;i++){
		printf("%d %s",i+1,pack.names[i]);
	}
	else
    		printf("\nMessage received from %s: %s \n",pack.alias,pack.buff);
	
   }


}

int main(int argc, char *argv[])
{
	char buffer[1024];
	int sockfd, portno, n;
	struct PACKET packet;
        struct sockaddr_in serv_addr;
        struct hostent *server;
	
        if (argc <4) {
                fprintf(stderr,"usage %s port username connectTo\n", argv[0]);
                exit(0);
        }
        portno = atoi(argv[1]);

        /* Create a socket point */
        sockfd = connect_with_server(portno);
	
        if(sockfd >= 0) {
                // char *msg;
                // msg[BUFFSIZE] = 0;
		//getRequestList(sockfd);

		struct THREAD_INFO thread_info;
                thread_info.sockfd = sockfd;
		
                 pthread_create(&thread_info.thread_ID, NULL, server_handler, ( void *) &thread_info);
                while(1){
			sleep(1);
                        printf("1. See online users!\n2. Message user! \n");
                        bzero(buffer,1024);
                        fgets(buffer,1024,stdin);

			if(!strcmp(buffer,"1\n")){
					
				packet.list = 1;	
				packet.nameCounter = 0;
	                        send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
			}

			else if(strcmp(buffer,"2\n") == 0){
                        
			memset(&packet, 0, sizeof(struct PACKET));
                        strcpy(packet.alias, argv[2]);
			printf("Enter user name:");
			bzero(buffer,1024);
                        fgets(buffer,1024,stdin);
			strcpy(packet.connectTo, buffer);
			printf("Enter message:");
			bzero(buffer,1024);
                        fgets(buffer,1024,stdin);
                        strcpy(packet.buff,buffer);
			packet.list = 0;
			packet.nameCounter = 0;

                        int sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                        printf("%d\n",sockfd);
		 
 		}
			
			else{
				printf("Enter valid choice");
			}

		}
        
		
        }
        else {
                fprintf(stderr, "Connection rejected...\n");
        }

        return 0;
}
