#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define ALIASLEN 40
#define BUFFSIZE 1024
#define OPTLEN 16

int globalList[10];
char *globalListName[10];
int tracker = 0;

void sigintHandler(int sig_num)
{
        /* Reset handler to catch SIGINT next time.
           Refer http://en.cppreference.com/w/c/program/signal */
        signal(SIGINT, sigintHandler);
        printf("\n Termination \n");
        // close(client_sockfd);
        exit(1);
}

struct THREAD_INFO {
    pthread_t thread_ID; // thread's pointer
    int sockfd; // socket file descriptor
    char alias[ALIASLEN]; // client's alias
  
};

struct PACKET {
        char option[OPTLEN]; // instruction
        char alias[ALIASLEN]; // client's alias
        char buff[BUFFSIZE];
	char connectTo[20];
	int list;
	char *names[20];
	
 // payload
};

void *client_handler(void *fd) {
    struct THREAD_INFO thread_info = *(struct THREAD_INFO *) fd;
    int bytes;
    struct PACKET packet;
    //struct PACKET packetToSend;
    
  
    while(1) {
        bytes = recv(thread_info.sockfd,  (void *)&packet, sizeof(struct PACKET), 0);
        if (!bytes) { 
	    perror("No bytes received at server");
	    exit(0);
       }
	
	globalList[tracker] = thread_info.sockfd;
	globalListName[tracker] = packet.alias;
	tracker++;

	if(packet.list == 1){
		int i;
		for( i = 0 ; i< tracker ; i++)
			packet.names[i] = globalListName[i];

		if(send(thread_info.sockfd, (void *)&packet, sizeof(struct PACKET), 0) < 0){
			perror("Send to client error");
			exit(1);
		}
			
	}
	else
	 {
	int i;
	for(i = 0; i < tracker ; i++){

	  if(strcmp(packet.connectTo,globalListName[i]) == 0){	
		if(send(globalList[i], (void *)&packet, sizeof(struct PACKET), 0) < 0){
			
			perror("Send to next client error");
			exit(1);
		}
		break;
	  }
		
	}

	if(i == tracker){
		
			strcpy(packet.buff,"User is offline!");
			if(send(thread_info.sockfd, (void *)&packet, sizeof(struct PACKET), 0) < 0){
			perror("Send to client error");
			exit(1);
			}
		 }
		
    	}
    }
}
int main( int argc, char *argv[] )
{
         int sockfd,newsockfd, portno, clilen;
        // char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        // int n;
        /* First call to socket() function */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        signal(SIGINT, sigintHandler);
        if (sockfd < 0)
        {
                perror("ERROR opening socket");
                exit(1);
        }

        /* Initialize socket structure */
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        /* Initialize main thread */
        printf("Starting loggin interface...\n");


        /* Now bind the host address using bind() call.*/
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
                perror("ERROR on binding");
                exit(1);
        }


        /* Now start listening for the clients, here process will
         * go in sleep mode and will wait for the incoming connection
         */

        listen(sockfd,5);


        /* Accept actual connection from the client */
        // if (newsockfd < 0)
        // {
        //         perror("ERROR on accept");
        //         exit(1);
        // }
        while(1) {
                clilen = sizeof(cli_addr);
                if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen)) == -1) {

                    perror("Error accepting connection");
                    exit(1);
                }
                else {
                    printf("Connection requested received...\n");
                    struct THREAD_INFO thread_info;
                    thread_info.sockfd = newsockfd;
                    pthread_create(&thread_info.thread_ID, NULL, client_handler, ( void *) &thread_info);
                }
                // bzero(buffer,256);
                // n = read( newsockfd,buffer,255 );
                //
                // if (n < 0)
                // {
                //         perror("ERROR reading from socket");
                //         exit(1);
                // }
                //
                // printf("Here is the message: %s\n",buffer);
                //
                // /* Write a response to the client */
                // n = write(newsockfd,"I got your message",18);
                // if (n < 0) {
                //         perror("ERROR writing to socket");
                //         exit(1);
                // }
        }
        return 0;
}
