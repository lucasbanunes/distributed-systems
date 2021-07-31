#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

int is_prime(int num){
    // Tests if num is prime
    if (num%2 == 0 || num <= 1){
        return 0;
    }
    for (int i=3;i<num;i=i+2){
        if (num%i == 0){
            return 0;
        }
    }
    return 1;
}

int main(int argc,char **argv,char **envp){

    if (argc != 2){
        printf("A funcao funciona apenas com um argumento\n");
        printf("%s <quantitade>\n", argv[0]);
        return -1;
    }
    int quantity = atoi(argv[1]);

    int p[2];
    pid_t childpid;

    if (pipe(p) < 0){
        printf("Erro ao construir o pipe\n");
        return 1;
    }

    childpid = fork();
    if (childpid < 0){
        printf("Erro de fork\n");
        close(p[0]);
        close(p[1]);
        return 1;
    }
    else if (childpid == 0){    //Processo filho, consumidor
        int n, res;
        n = 1;
        close(p[1]);
        while (n!=0){
            read(p[0], &n, sizeof(n));
            res = is_prime(n);
            printf("O numero %d e primo? %d\n", n, res);
        }
        printf("Consumidor finalizado\n");
        close(p[0]);
        return 0;
    }
    else{   //Processo pai, produtorwrite(p[1], &n, sizeof(n));
        close(p[0]);
        int n = 1;
        write(p[1], &n, sizeof(n));
        for (int i = 1; i<quantity; i++){
            n += (rand() % 100)+1;
            write(p[1], &n, sizeof(n));
        }
        printf("Produtor finalizado\n");
        n = 0;
        write(p[1], &n, sizeof(n));
        close(p[1]);
    }
    return 0;
}