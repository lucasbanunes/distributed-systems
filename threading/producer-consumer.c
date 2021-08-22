#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_PROCESSED 100000

int number_processed = 0;
sem_t mem_mutex;
sem_t full;
sem_t empty;
sem_t processed_mutex;


typedef struct arg_data {
    int *memory;
} arg_data;

bool is_prime(int num){
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

int generate_number(){
    // Gera um inteiro aleatório entre 1 e 10^7
    return (rand()%10000000) + 1;
}

void *producer_work(void* arg){
    arg_data* producer_arg = (arg_data*)arg;
    int memory_pointer=0;
    int new_num;

    while (1){
        new_num = generate_number();    // Produz
        
        sem_wait(&empty);   // Espera espaço vazio
        sem_wait(&mem_mutex);   // Acesso a memória

        while (producer_arg->memory[memory_pointer] != 0){
            memory_pointer++;
        }
        producer_arg->memory[memory_pointer] = new_num;
        sem_post(&mem_mutex);
        sem_post(&full);
        memory_pointer=0;
    }
}

void *consumer_work(void *arg){
    arg_data* consumer_arg = (arg_data*)arg;
    int memory_pointer = 0;
    int read_num;
    int free = 0;

    while (1){

        sem_wait(&full);    // Espera valor
        sem_wait(&mem_mutex);   // Acesso a memória
        while (consumer_arg->memory[memory_pointer] == 0) {
            memory_pointer++;       
        }
        read_num = consumer_arg->memory[memory_pointer];
        consumer_arg->memory[memory_pointer] = free;
        sem_post(&mem_mutex);
        sem_post(&empty);
        memory_pointer=0;

        //Checa se já foram processados números o suficente
        sem_wait(&processed_mutex);
        if (number_processed >= MAX_PROCESSED){
            sem_post(&processed_mutex);
            return NULL;
        }
        number_processed++;
        sem_post(&processed_mutex);

        if (is_prime(read_num)){
            printf("%i e primo.\n", read_num);
        }
        else {
            printf("%i nao e primo.\n", read_num);
        }
    }
}

int main(int argc,char **argv,char **envp){

    if (argc != 4){
        printf("A funcao funciona apenas com 3 argumento\n");
        printf("%s <tamanho da memoria (N)> <numero de produtores (Np)> <numero de consumidores (Nc)>\n", argv[0]);
        return 1;
    }
    srand(time(NULL));

    // Obtem os parametros de entrada
    int memory_size = atoi(argv[1]);
    int n_producers = atoi(argv[2]);
    int n_consumers = atoi(argv[3]);

    //Inicia a memoria
    int memory[memory_size];
    for (int memory_pointer=0; memory_pointer<memory_size; memory_pointer++){
        memory[memory_pointer] = 0;
    }

    //Inicia os semáforos
    sem_init(&mem_mutex, 0, 1);
    sem_init(&processed_mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, memory_size);

    //Inicia as threads
    pthread_t consumers[n_consumers];
    arg_data consumers_args[n_consumers];

    pthread_t producers[n_producers];
    arg_data producers_args[n_producers];

    clock_t start = clock();

    for (int current_consumer= 0; current_consumer < n_consumers; current_consumer++){
        // Inicializando argumentos da struct
        consumers_args[current_consumer].memory = memory;
        pthread_create(&consumers[current_consumer], NULL, consumer_work, &consumers_args[current_consumer]);
    }
    for (int current_prod= 0; current_prod < n_producers; current_prod++){
        // Inicializando argumentos da struct
        producers_args[current_prod].memory = memory;
        pthread_create(&producers[current_prod], NULL, producer_work, &producers_args[current_prod]);
    }

    // Espera os consumidores finalizarem
    for (int current_consumer= 0; current_consumer < n_consumers; current_consumer++){
        pthread_join(consumers[current_consumer], NULL);
    }
    clock_t finish = clock();
    double time_elapsed = ((double)(finish - start)) / CLOCKS_PER_SEC;
    printf("final: %i;%i;%i;%.10f\n",memory_size,n_producers,n_consumers,time_elapsed);
    return 0;   //Quando o main retorna o processo inteiro é finalizado
}