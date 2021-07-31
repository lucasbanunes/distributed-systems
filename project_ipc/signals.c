#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>

int n = 1;

int is_prime(int num){
    // Tests if num is prime
    if (num%2 == 0 || num <= 1){
        return 0;
    }
    for (int i=3;i<n;i=i+2){
        if (n%i == 0){
            return 0;
        }
    }
    return 1;
}

void handlerSIGUSR1(int signum){
    printf("[!] Recebeu SIGUSR1...\n");
    printf("O numero %d é primo? %d\n", n, is_prime(n));
}

void handlerSIGUSR2(int signum){
    //Updates the sequence number with random value
    printf("[!] Recebeu SIGUSR2...\n");
    n += (rand() % 100)+1;
}
void handlerSIGTERM(int signum){
    printf("[X] Finalizando o disparador...\n");
    exit(0);
}

int main(int argc,char **argv,char **envp){

    // Sets signal handlers
    signal(SIGUSR1,handlerSIGUSR1);
    signal(SIGUSR2,handlerSIGUSR2);
    signal(SIGTERM,handlerSIGTERM);
    signal(SIGINT,handlerSIGTERM);

    if (argc != 2){
        printf("A funcao funciona apenas com um argumento\n");
        printf("%s <busy||block>\n", argv[0]);
        return -1;
    }
    printf("Comecando no pid: %d \n",getpid());  

    if (strcmp(argv[1],"block") == 0){

        while(1){
            sigset_t sigset;
            int sig;
            sigemptyset(&sigset);
            sigaddset(&sigset, SIGTERM);
            sigaddset(&sigset, SIGUSR1);
            sigaddset(&sigset, SIGUSR2);
            printf("[?] Aguardando sinal\n");
            printf("O valor de n e: %d\n", n);
            int result = sigwait(&sigset, &sig);
            printf("[!] Recebido sinal: %d\n", sig);
            
            if (sig == SIGTERM){
                handlerSIGTERM(sig);
            }
            else if (sig == SIGUSR1){
                handlerSIGUSR1(sig);
            }
            else if (sig == SIGUSR2){
                handlerSIGUSR2(sig);
            }
            else{
                printf("O sinal %d não possui handler", sig);
            }
        
        }
    }
    else if (strcmp(argv[1],"busy") == 0){
        printf("[!] Computando\n");
        while (1){
            //Updates the sequence number with random value
            n += (rand() % 100)+1;
            sleep(5);   //Avoids overflow
        }
    }
    else{
        printf("Parameter not supported");
    }
    return -1;
}