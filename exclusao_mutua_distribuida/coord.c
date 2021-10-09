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

#define MAX_PROCESSES 256
#define MSG_SIZE 6
#define REGISTER_ID 0
#define REQUEST_ID 1
#define GRANT_ID 2
#define RELEASE_ID 3
#define DENY_ID 4
#define PADDING_MSG "000000"

int __rodando = 1;
sem_t mem_mutex;
sem_t full;
sem_t empty;
sem_t processed_mutex;
int queue[MAX_PROCESSES];
int proccess_sockets[MAX_PROCESSES];

int enviar_id(int process_id, int socket_fd){
    char *msg;
    msg = build_msg(REGISTER_ID, process_id);
    sendto(socket_fd, (const char *)msg, strlen(msg),
    MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
    sizeof(servaddr));   
}

char *build_msg(int msg_id, int process_id){
    char aux_msg[MSG_SIZE+1]; // One more for the null pointer
    char msg[MSG_SIZE+1];
    sprintf(aux_msg, "%i|%i|", msg_id, process_id);
    int pad_len = MSG_SIZE - strlen(aux_msg);
    sprintf(msg, "%s%*.*s", aux_msg, pad_len, pad_len, PADDING_MSG);
    return msg;
}

int algoritmo_exclusao_mutua(){
    /*
    while(__rodando){
        wait(free_rc)
        proccess_id = obter_id()
        log_acesso()
        atualizar_ocupante()
    }
    */

}

/*
int obter_id(){
    wait(q_full);
    wait(q_mutex);
    proccess_id = queue[q_idx];
    signal(q_mutex);
    signal(q_empty);
    return proccess_id;
}
*/

/*
int cadastrar_processo(process_addr){
    int new_id = -1;
    for (i = 0; i<MAX_PROCESSSES, i++){
        if (proccess_sockets[i] != 0):
            new_id = i;
            proccess_sockets[i] = process_addr;
    return new_id;
    }
}
*/

int comunicador(){
    /*
    int ultimo_ocupante = -1;
    int ocupante_atual = -1;
    socket_fd;
    char buffer[MSG_SIZE;]
    int proccess_addr;
    while(__rodando){
        bufer = VAZIO;
        while(buffer == VAZIO){
            listen(socket, buffer, process_addr)
        }
        parsed = parse_msg(buffer)
        switch (parsed.msg_type){
            case 0:
            new_id = cadastrar_processo(process_addr)
            if (new_id == -1){
                nao_autorizado(process_addr)
            }
            else{
                enviar_id(new_id)
            }

            case 1:
            botar_na_fila()

            case 3:
            liberar()
            signal(free_rc)

        }
        ocupante_atual = obter_ocupante()
        if (ultimo_ocupante != ocupante_atual) & (ocupante_atual != vazio){
            enviar_grant(ocupante_atual);
            utlitmo_ocupante = ocupante_atual;
        }
    }
    */
}

int imprimir_fila(){

}

int imprimir_numero_chamadas_processo(){

}

int encerrar(){
    __encerrar = 1;

}

int main(int argc,char **argv,char **envp){
    // criar threads
    int opcao;
    while(1){
        system("clear");
        printf("Digite uma das funcoes:\n");
        printf("1 - Imprimir fila\n");
        printf("2 - Imprimir quantidade de vezes que cada processo foi atendido\n");
        printf("3 - Encerrar execucao\n");
        scanf("%i", &opcao);
        switch (opcao){
            case 1:
                printf("Opcao 1 selecionada\n");
                break;
            case 2:
                printf("Opcao 2 selecionada\n");
                break;
            case 3:
                printf("Opcao 3 selecionada\n");
                printf("Encerrando")
                encerrar();
                break;
            default:
                printf("Opcao invalida\n");
                break;
        }
    }
}