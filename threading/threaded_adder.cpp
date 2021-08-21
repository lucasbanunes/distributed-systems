#include <cstdint>
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <atomic>

using namespace std;

// Acumulador das threads
int acc = 0;

// Definindo lock e suas funcionalidades
typedef struct spinlock {
    std::atomic<bool> held = {false};
} spinlock;

void acquire(spinlock *lock){
    while(lock->held.exchange(true, std::memory_order_acquire));
}

void release(spinlock *lock) {
    lock->held.store(false, std::memory_order_release);
}

//Lock das threads
spinlock thread_lock;

// Estrutura de dados dos argumentos da thread
typedef struct arg_data {
    int8_t *all_numbers;
    int start;
    int end;
} arg_data;

// Gera os arrays aleatórios
int8_t *generateArray(int n_numbers ){
    int8_t* numbers= (int8_t*) malloc((n_numbers)*sizeof(int8_t));
    for (int i=0; i<n_numbers; ++i){
        numbers[i] = rand()%101;
    }
    return numbers;
}

// Adiciona os numeros por thread
void *addNumbers(void* arg){
    arg_data* thread_args = (arg_data*)arg;
    for(int i = thread_args->start; i < thread_args->end; ++i){
        acquire(&thread_lock);
        acc += thread_args->all_numbers[i];
        release(&thread_lock);
    }
    return NULL;    
}

int main(int argc,char **argv,char **envp){

    if (argc != 3){
        printf("A funcao funciona apenas com 2 argumento\n");
        printf("%s <quantidade de numeros (N)> <numero de threads (K)>\n", argv[0]);
        return 1;
    }
    // Obtem os parametros de entrada
    int n_numbers = atoi(argv[1]);
    int n_threads = atoi(argv[2]);

    // Gera Array com numeros aleatorios
    int8_t *all_numbers;
    srand(time(NULL));
    all_numbers = generateArray(n_numbers);
    
    // Gera parametros para threads
    int split_size = n_numbers/n_threads;
    int start = 0;
    int end;
    pthread_t thr[n_threads];
    arg_data thread_args[n_threads];

    // Inicia threads
    clock_t thread_start = clock();
    for(int current_thread=0; current_thread<n_threads; ++current_thread){
        
        // Adiciona resto dos dados para ultima thread para casos onde o numero de valores
        // nao e divisivel pelo numero de threads
        if (current_thread == n_threads-1){
            end =start + split_size + n_numbers%n_threads;
        }
        else{
            end = start + split_size;
        }
        
        // Inicializando argumentos da struct
        thread_args[current_thread].all_numbers = all_numbers;
        thread_args[current_thread].start = start;
        thread_args[current_thread].end = end;

        pthread_create(&thr[current_thread], NULL, addNumbers, &thread_args[current_thread]); // Instancia thread
        //printf("Thread: %i, start: %i, end: %i\n", current_thread, start, end);
        
        start += split_size;
    }

    // Join nas threads
    for(int current_thread=0; current_thread<n_threads; ++current_thread){
        pthread_join(thr[current_thread], NULL);
    }
    clock_t thread_finish = clock();
    double time_elapsed = ((double)(thread_finish - thread_start)) / CLOCKS_PER_SEC;
    // printf("A soma total é %i \n", acc);
    printf("%.10f;%i;%i\n", time_elapsed, n_threads, n_numbers);

    // Debug
    /*clock_t no_thread_start = clock();
    int no_thread_acc = 0;
    for(int i=0; i<n_numbers; i++){
        no_thread_acc += all_numbers[i];
    }
    clock_t no_thread_finish = clock();
    printf("A soma total sem threads é %i \n", no_thread_acc);
    printf("%.10f\n",((double)(no_thread_finish - no_thread_start)) / CLOCKS_PER_SEC);
    */
    return 0;
}