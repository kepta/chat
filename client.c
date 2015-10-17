/**
 * client
 */
 #include "definition.h"
 #include "client.h"
 #include "misc.h"

struct THREAD_INFO {
        pthread_t thread_ID;  // thread's pointer
        int sockfd;  // socket file descriptor
        char alias[ALIASLEN];  // client's alias
};

void client(int argc, char *argv[], int sockfd, int port) {
        printf("This is the client\n");
        char host[1024], buffer[1024];
        struct PACKET packet;
        if (arg_finder("-host") != -1) {
                strcpy(host, argv[arg_finder("-host") +1]);
                printf("lookign for host: %s\n", host);
        } else {
                perror("Error! use -host HOSTNAME\n");
                exit(1);
        }
        connect_with_server(sockfd, port, host);
        struct THREAD_INFO thread_info;
        thread_info.sockfd = sockfd;
        pthread_create(&thread_info.thread_ID, NULL, server_handler, ( void *) &thread_info);

        while(1) {
                // printf("1. See online users!\n2. Message user! \n");
                bzero(buffer,1024);
                fgets(buffer,1024,stdin);
                if (strcmp(buffer,"list\n") == 0) {
                        strcpy(packet.option, "list");
                        printf("lister\n");
                        send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
                else if (strcmp(buffer,"login\n") == 0) {
                        strcpy(packet.option, "login");
                        printf("logging in.....\n");
                        send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
                else if(!strcmp(buffer,"1\n")) {
                        packet.list = 1;
                        packet.nameCounter = 0;
                        send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }

                else if(strcmp(buffer,"2\n") == 0) {

                        memset(&packet, 0, sizeof(struct PACKET));
                        strcpy(packet.alias, argv[2]);
                        printf("Enter user name:");
                        bzero(buffer,1024);
                        fgets(buffer,1024,stdin);

                        strcpy(packet.connectTo,argv[3]);
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


void connect_with_server(int sockfd, int SERVERPORT, char *SERVERIP) {
        int err_ret;
        struct sockaddr_in serv_addr;
        struct hostent *to;

        /* generate address */
        if((to = gethostbyname(SERVERIP))==NULL) {
                err_ret = errno;
                perror("gethostbyname() error...\n");
                exit(1);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(SERVERPORT);
        serv_addr.sin_addr = *((struct in_addr *)to->h_addr);
        memset(&(serv_addr.sin_zero), 0, 8);

        /* try to connect with server */
        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
                err_ret = errno;
                perror("connect() error...\n");
                exit(1);
        }
        else {
                printf("Connected to server at %s:%d\n", SERVERIP, SERVERPORT);
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
                if(pack.list == 1) {
                        printf("List of online users is:\n");
                        int i;
                        for( i = 0; i < pack.nameCounter; i++) {
                                // printf("%d %s",(i+1),pack.names[i]);
                        }
                }
                else {
                        printf("\nMessage received from %s: %s \n",pack.alias,pack.buff);
                }
        }
}
