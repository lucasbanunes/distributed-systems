#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <atomic>
#include <semaphore>
#include <vector>
#include <thread>

using namespace std;

/* Produtor
i = 0
while true {
    
}
*/

/* Consumidor
i = 0
while true {
    
}
[[prod1], [prod2], ...]
[[1123123,1],[12312313,0],[0,1],[0,0]]
[1123123,12312313,0,0,1,0,0]
*/

typedef struct arg_data {
    int *memory;
    counting_semaphore *mutex;
    counting_semaphore *full;
    counting_semaphore *empty;
} arg_data;



bool isPrime(int num){
    // Testa se num é primo
    if (num%2 == 0 || num <= 1){
        return false;
    }
    for (int i=3;i<num;i=i+2){
        if (num%i == 0){
            return false;
        }
    }
    return true;
}

int generateNumber(){
    // Gerea um inteiro aleatório entre 1 e 10^7
    return (rand()%10000000) + 1;
}

void *producerWork(void* arg){
    i=0;
    while true{
        mutex.acquire();
        while memory[i] != 0{
            i++
            if i >= n_numbers{
                mutex.release();
                break
                
            }
        }
        mutex.acquire();
        memory[i] = generateNumber();
        mutex.release();
    }
}

void consumerWork(){
    i=0;
    while true{
        mutex.acquire();
        while memory[i] != 0{
            i++
            if i >= n_numbers{
                mutex.release();
                break
                
            }
        }
        mutex.acquire();
        isPrime(memory[i]); 
        mutex.release();
    }
}



int main(int argc,char **argv,char **envp){

    if (argc != 4){
        printf("A funcao funciona apenas com 3 argumento\n");
        printf("%s <tamanho da memoria (N)> <numero de produtores (Np)> <numero de consumidores (Nc)>\n", argv[0]);
        return 1;
    }
    // Obtem os parametros de entrada
    int n_numbers = atoi(argv[1]);
    int n_produtores = atoi(argv[2]);
    int n_consumidores = atoi(argv[3]);


    counting_semaphore<1> mutex(1);
    counting_semaphore<n_numbers> empty(n_numbers);
    counting_semaphore<n_numbers> full(0);

    // //Instancia threads
    // vector<thread> consumers;
    // vector<thread> producers;

    // for (int i=0; i<n_consumers; i++){
    //     thread t(consume)
    // }

    
    srand(time(NULL));
    int* memory = (int*) malloc((n_numbers)*sizeof(int));
}



