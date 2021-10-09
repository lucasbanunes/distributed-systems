#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_ARGS 3
#define COORD_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_SIZE 6

int acquire()

int main(int argc,char **argv,char **envp){


    if (argc != MAX_ARGS+1){
        printf("A funcao funciona apenas com %i argumentos\n", MAX_ARGS);
        printf("%s <numero de requisicoes (r)> <segundos de espera (k) <id do processo (process_id)>\n", argv[0]);
        return 1;
    }

    // Obtem os parametros de entrada
    int r = atoi(argv[1]);
    int k = atoi(argv[2]);
    int process_id = atoi(argv[3]);

    int sockfd;
    char buffer[MAX_MSG_SIZE];
    char *hello = "Hello from client";
    struct sockaddr_in servaddr;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    
    
    for (int i = 0; i<r; i++){
        int n, len;
        
        sendto(sockfd, (const char *)hello, strlen(hello),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
                sizeof(servaddr));            
        n = recvfrom(sockfd, (char *)buffer, MAX_MSG_SIZE, 
                    MSG_WAITALL, (struct sockaddr *) &servaddr,
                    &len);
        buffer[n] = '\0';
        printf("Server : %s\n", buffer);
    }
    
}