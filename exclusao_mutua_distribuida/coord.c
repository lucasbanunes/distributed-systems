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
#include <netdb.h>
#include <poll.h>

//Sockets
#define PORT_NO 8080
#define MAX_PROCESSES 256
// MessagesIDS
#define MSG_SIZE 6
#define REGISTER_ID 0
#define REQUEST_ID 1
#define GRANT_ID 2
#define RELEASE_ID 3
#define DENY_ID 4
#define QUIT_ID 5
#define UNREGISTER_ID 6
// MSG UTILS
#define PADDING_MSG "000000"
#define INIT_MSG "000000"
#define MSG_SEP "|"
// QUEUE UTILS
#define CLOSE_QUEUE -1337
// Files
#define LOG_FILE "resultados.log"
#define RESULTS_FILE "resultado.txt"
#define TIMEOUT 1000
#define LINE_SEP "----------------------------------------------------------------\n"

typedef struct parsed_msg {
    int msg_id;
    int process_id;
} parsed_msg;

int __running = 1;
int queue[MAX_PROCESSES];
int n_calls[MAX_PROCESSES];
int cr_process_id = -1;
// Socket Variables
int sockfd;
struct sockaddr_in clientsaddrs[MAX_PROCESSES];
struct sockaddr_in serveraddr;
// Semaforos Process Queue
sem_t qfull;
sem_t qempty;
sem_t qmutex;
sem_t free_critical_region;
sem_t wr_cr_process;
sem_t cr_mutex;
sem_t calls_mutex;


// Utils Msg
char *build_msg(int msg_id, int process_id, char *msg){
    char aux_msg[MSG_SIZE+1]; // One more for the null pointer
    sprintf(aux_msg, "%i|%i|", msg_id, process_id);
    int pad_len = MSG_SIZE - strlen(aux_msg);
    sprintf(msg, "%s%*.*s", aux_msg, pad_len, pad_len, PADDING_MSG);
    return msg;
}

parsed_msg parse_msg(char* msg){
    parsed_msg parsed;
    printf("Mensagem parseada: %s\n", msg);
    if(strcmp(msg, INIT_MSG)){
        printf("Init msg");
        parsed.msg_id = 0;
        parsed.process_id =-1;
    }
    char *action_ptr = strtok(msg, MSG_SEP);
    char *id_ptr = strtok(NULL, MSG_SEP);
    parsed.msg_id = atoi(action_ptr);
    parsed.process_id = atoi(id_ptr);
    return parsed;
}

// Comunicador Functions
int register_id(struct sockaddr_in clientaddr){
    printf("Entered register\n");
    int new_id=-1;
    for (int i=0; i<MAX_PROCESSES; i++){
        if (clientsaddrs[i].sin_port  == 0){
            new_id = i;
            clientsaddrs[i] = clientaddr;
            break;
        }
    }
    return new_id;
}

int send_id(int process_id){
    char msg[MSG_SIZE+1];
    build_msg(REGISTER_ID, process_id, msg);
    sendto(sockfd, (const char *)msg, strlen(msg),
    MSG_CONFIRM, (const struct sockaddr *) &clientsaddrs[process_id], 
    sizeof(clientsaddrs[process_id]));
}

int send_grant(int process_id){    
    char msg[MSG_SIZE+1];
    build_msg(GRANT_ID, process_id, msg);
    sendto(sockfd, (const char *)msg, strlen(msg),
    MSG_CONFIRM, (const struct sockaddr *) &clientsaddrs[process_id], 
    sizeof(clientsaddrs[process_id]));

}

int send_deny(struct sockaddr_in clientaddr){
    char msg[MSG_SIZE+1];
    build_msg(DENY_ID, -1, msg);
    sendto(sockfd, (const char *)msg, strlen(msg),
    MSG_CONFIRM, (const struct sockaddr *) &clientaddr, 
    sizeof(clientaddr));
    return 0;   
}

int send_quit(){
    char msg[MSG_SIZE+1];
    for(int i=0; i<MAX_PROCESSES; i++){ 
        if (clientsaddrs[i].sin_port  != 0){ /* Not empty register */
            build_msg(QUIT_ID, i, msg);
            sendto(sockfd, (const char *)msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *) &clientsaddrs[i], sizeof(clientsaddrs[i]));
        }
    }
}

int unregister_process(int process_id){
    bzero((char *) &clientsaddrs[process_id], sizeof(clientsaddrs[process_id]));
    n_calls[process_id] = 0;
}

// Queue Functions
int put_queue(int process_id){
    sem_wait(&qempty);
    sem_wait(&qmutex);
    for(int i=0; i<MAX_PROCESSES; i++){
        if (queue[i] == -1){
            queue[i] = process_id;
            break;
        }
    }
    sem_post(&qmutex);
    sem_post(&qfull);
    return 0;
}

int pop_queue(){
    int pop_id;
    sem_wait(&qfull);
    sem_wait(&free_critical_region);
    sem_wait(&qmutex);
    pop_id = queue[0];
    for(int i=0; i<MAX_PROCESSES-1; i++){
        queue[i] = queue[i+1];
        if (queue[i] == 0){
            break;
        }
    }
    queue[MAX_PROCESSES-1] = -1;
    sem_post(&qmutex);
    sem_post(&qempty);
    return pop_id;
}
// Logging Functions
int log_aquire(FILE *log_file, int process_id){
    // date;process_id;aquired
    fprintf(log_file, "%lu;%i;aquired\n", (unsigned long)time(NULL), process_id);
}
int log_release(FILE *log_file, int process_id){
    // date;process_id;released
    fprintf(log_file, "%lu;%i;released\n", (unsigned long)time(NULL), process_id); 
}
// Algorithm Functions
int write_cr_process(int process_id){
    sem_wait(&wr_cr_process);
    cr_process_id = process_id;
    sem_post(&wr_cr_process);
}

int read_cr_process(){
    int process_id;
    sem_wait(&wr_cr_process);
    process_id = cr_process_id;
    sem_post(&wr_cr_process);
    return process_id;
}

// Algorithm
void *algoritmo_exclusao_mutua(){
    int current_process_id;
    while(__running){
        current_process_id = pop_queue();
        if (current_process_id == CLOSE_QUEUE){
            break;
        }
        write_cr_process(current_process_id);
    }
    return 0;
}
// Comunicador
void *comunicador(){
    FILE *log_file = fopen(LOG_FILE, "a");
    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    int current_cr_process;
    char buffer[MSG_SIZE];
    int res;
    int new_id;
    struct sockaddr_in clientaddr;
    int clientlen  = sizeof(clientaddr);
    parsed_msg parsed;
    printf("Comunicador inicializado\n");   

    while(__running){
        bzero((char *) &clientaddr, sizeof(clientaddr));
        current_cr_process = read_cr_process();
        if (current_cr_process != -1){
            write_cr_process(-1);
            log_aquire(log_file, current_cr_process);
            send_grant(current_cr_process);
        }

        poll(fds, 1, TIMEOUT);
        if (fds[0].revents & POLLIN){
            // printf("Entered\n");
            res = recvfrom(sockfd, buffer, MSG_SIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
            if (res < 0){
                error("ERROR in recvfrom");
            }
            parsed = parse_msg(buffer);
            printf("Parsed msg_id %i\n", parsed.msg_id);

            switch (parsed.msg_id){
                case REGISTER_ID :
                    printf("Registering\n");
                    
                    new_id = register_id(clientaddr);
                    printf("New_id %i\n", new_id);
                    if (new_id == -1){
                        send_deny(clientaddr);
                    }
                    else{
                        send_id(new_id);
                    }
                    break;

                case REQUEST_ID:
                    printf("Request %i\n",parsed.process_id);
                    put_queue(parsed.process_id);
                    break;

                case RELEASE_ID:
                    printf("Released\n");
                    log_release(log_file, parsed.process_id);
                    sem_wait(&calls_mutex);
                    n_calls[parsed.process_id] += 1;
                    sem_post(&calls_mutex);
                    sem_post(&free_critical_region);
                    break;

                case UNREGISTER_ID:
                    printf("Unregister\n");
                    unregister_process(parsed.process_id);
                    break;

                default:
                    printf("Default\n");
                    send_deny(clientaddr);
                    break;
            }
        }
    }
    put_queue(CLOSE_QUEUE);
    printf("Sending Quit\n");
    send_quit();
    return 0;   
}
// Interface Functions
void print_queue(){
    char aux;
    printf(LINE_SEP);
    sem_wait(&qmutex);
    printf("[");
    for (int i=0; i<MAX_PROCESSES; i++){
        printf(" %i,", queue[i]);
    }
    printf("]\n");
    sem_post(&qmutex);
    printf(LINE_SEP);
}

int print_n_calls(){
    char aux;
    printf(LINE_SEP);
    sem_wait(&calls_mutex);
    printf("[");
    for (int i=0; i<MAX_PROCESSES; i++){
        printf(" %i. %i, ", i, n_calls[i]);
    }
    printf("]\n");
    sem_post(&calls_mutex);
    printf(LINE_SEP);
    scanf("%c", &aux);
}

int main(int argc,char **argv,char **envp){
    // Inicializing global variables
    bzero((char *) &serveraddr, sizeof(serveraddr));
    for (int i=0; i<MAX_PROCESSES; i++){
        bzero((char *) &clientsaddrs[i], sizeof(clientsaddrs[i]));
        queue[i] = -1;
        n_calls[i] = 0;
    }
    sem_init(&qfull, 0, 0);
    sem_init(&qempty, 0, MAX_PROCESSES);
    sem_init(&qmutex, 0, 1);
    sem_init(&free_critical_region, 0, 1);
    sem_init(&wr_cr_process, 0, 1);
    sem_init(&cr_mutex, 0, 1);
    sem_init(&calls_mutex, 0, 1);

    // Sockets
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)PORT_NO);    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /* setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0){
        error("ERROR on binding");
    }    
    // criar threads
    pthread_t comunicador_thread_id;
    pthread_t exclusao_mutua_thread_id;
    pthread_create(&comunicador_thread_id, NULL, comunicador, NULL);
    pthread_create(&exclusao_mutua_thread_id, NULL, algoritmo_exclusao_mutua, NULL);

    // Interface
    int opcao;
    system("clear");
    while(1){
        printf("Digite uma das funcoes:\n");
        printf("1 - Imprimir fila\n");
        printf("2 - Imprimir quantidade de vezes que cada processo foi atendido\n");
        printf("3 - Encerrar execucao\n");
        scanf("%i", &opcao);
        switch (opcao){
            case 1:
                printf("Opcao 1 selecionada\n");
                print_queue();
                break;
            case 2:
                printf("Opcao 2 selecionada\n");
                print_n_calls();
                break;
            case 3:
                printf("Opcao 3 selecionada\n");
                printf("Encerrando\n");
                __running = 0;
                pthread_join(comunicador_thread_id, NULL);
                pthread_join(exclusao_mutua_thread_id, NULL);
                return 0;
            default:
                printf("Opcao invalida\n");
                break;
        }
    }
}