// /**
//  * server
//  */
 #include "definition.h"
 #include "server.h"
 #include "misc.h"

int globalList[10];
char *globalListName[10];
int tracker;

struct THREAD_INFO {
        pthread_t thread_ID;  // thread's pointer
        int sockfd;  // socket file descriptor
        char alias[ALIASLEN];  // client's alias
};
User local_user ;
void server(int argc, char *argv[], int sockfd, int port) {
        printf("This is server\n");
        int newsockfd, portno, clilen;

        struct sockaddr_in serv_addr, cli_addr;

        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        /* Initialize main thread */
        printf("Starting loggin interface...\n");

        /* Now bind the host address using bind() call.*/
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
                perror("ERROR on binding");
                exit(1);
        }

        listen(sockfd,5);

        strcpy(local_user.name, "default");
        local_user.fd =  -1;
        local_user.next = NULL;

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
        }
}


void login_user(User *user, char *name, int fd) {
        while(user->next != NULL) {
                user = user->next;
        }
        User *newUser = malloc(sizeof(User));
        strcpy(newUser->name, name);
        newUser->fd = fd;
        newUser->next = NULL;
        user->next = newUser;
}

void *client_handler(void *fd) {
        struct THREAD_INFO thread_info = *(struct THREAD_INFO *) fd;
        int bytes;
        struct PACKET packet;

        while(1) {
                bytes = recv(thread_info.sockfd,  (void *)&packet, sizeof(struct PACKET), 0);
                if (!bytes) {
                        perror("No bytes received at server");
                        exit(0);
                }
                printf("opt:%s %d\n",packet.option, strcmp(packet.option, "List"));
                if (strcmp(packet.option, "login") == 0 ) {
                    printf("user: %s loged in\n",packet.alias);
                    login_user(&local_user, packet.alias, thread_info.sockfd);
                }
                else if (strcmp(packet.option, "pm") == 0){
                    int i;
                    for(i = 0; i < tracker; i++) {

                            if(strcmp(packet.connectTo,globalListName[i]) == 0) {
                                    if(send(globalList[i], (void *)&packet, sizeof(struct PACKET), 0) < 0) {

                                            perror("Send to next client error");
                                            exit(1);
                                    }
                                    break;
                            }

                    }
                    if(i == tracker) {
                            strcpy(packet.buff,"User is offline!");
                            if(send(thread_info.sockfd, (void *)&packet, sizeof(struct PACKET), 0) < 0) {
                                    perror("Send to client error");
                                    exit(1);
                            }
                    }
                }
                else if (strcmp(packet.option, "list") == 0 ) {
                        printf("looking at list");
                        User *temp_user = &local_user;
                        char name_list[1024];
                        int iterator = 0;
                        while(temp_user->next != NULL) {
                            temp_user = temp_user->next;
                            for(int i = 0 ; i < strlen(temp_user->name) ; i++) {
                                name_list[i+iterator] = temp_user->name[i];
                            }
                            name_list[strlen(temp_user->name)+iterator] = '\n';
                            iterator = strlen(temp_user->name) + 1;
                        }
                        strcpy(packet.buff , name_list);
                        if(send(thread_info.sockfd, (void *)&packet, sizeof(struct PACKET), 0) < 0) {
                                perror("Send to client error");
                                exit(1);
                        }
                }
                else {
                    printf("no decent option sent");
                }
        }
}
