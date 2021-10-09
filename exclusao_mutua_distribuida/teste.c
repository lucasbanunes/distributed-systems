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
#define ZERO_MSG "000000"
#define MSG_SIZE 6
char *padding = "00000000000000000000000";

int main(int argc,char **argv,char **envp){


    char msg_teste[MSG_SIZE+1] = ZERO_MSG;
    char nova_msg[MSG_SIZE];
    printf("%s\n", msg_teste);
    sprintf(msg_teste, "%i|%i|", 1, 2);
    printf("%s\n", msg_teste);
    int padLen = MSG_SIZE - strlen(msg_teste);
    sprintf(nova_msg, "%s%*.*s", msg_teste, padLen, padLen, padding);  // RIGHT Padding 
    printf("%s\n", nova_msg);
    return 0;
}