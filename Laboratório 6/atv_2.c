/*
 * Solução concorrente para a quantidade de números primos em um arquivo de entrada binario
 * De forma ingênua e usando consumidores e produtores em um buffer pequeno
 * com apenas semáforos
 */

#include <stdio.h>
#include <pthreads.h>
#include <semaphore.h>
#include <stdlib.h>
#include <math.h>

// Variáveis globais
buffer[tam_buffer];
sem_t empty, full; // Semáforos para controle de buffer
sem_t mutex; // Semáforo para exclusão mútua

typedef struct {
    FILE *file;
} t_Args_prod;

int is_prime(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long int i = 3; i < sqrt(n) + 1; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void insert_buffer(long long int number) {
    static int in = 0;  // Posição de inserção
    sem_wait(&empty); // Espera o slot vazio no buffer 
    sem_wait(&mutex);
    buffer[in] = number; // Insere o número no buffer
    in = (in + 1) % tam_buffer; // Atualiza a posição de inserção
    sem_post(&mutex);
    sem_post(&full);
}

long long int remove_buffer() {
    static int out = 0; // Posição de remoção
    long long int number;
    sem_wait(&full); // espera o slot cheio no buffer
    sem_wait(&mutex); 
    number = buffer[out]; // Pega o número do buffer
    buffer[out] = 0; // Apaga o número do buffer (opcional)
    out = (out + 1) % tam_buffer; // Atualiza a posição de remoção 
    sem_post(&mutex);
    sem_post(&empty);
    return number;
}

void *producer(void *arg){
    t_Args_prod *args = (t_Args_prod *) arg;
    long long int number_read;
    while (fread(&number_read, sizeof(long long int), 1, args->file)) {
        insert_buffer(number_read);
    }
    insert_buffer(-1); // Insere um número inválido para finalizar a execução
    
    if(fclose(args->file)) {
        fprintf(stderr, "Erro ao fechar o arquivo\n");
        return 2;
    }
    free(args);
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    long long int number;
    int local_primes = 0;
    while (1) {
        number = remove_buffer();
        if (number == -1) break; // Se for um número inválido, finaliza a execução
        if (is_prime(number)) local_primes++; // Verifica se o número é primo
    }
    pthread_exit((void *) local_primes);
}

int main (int argc, char *argv[]) {
    // Variáveis locais
    int n_threads;
    int total_primes = 0;
    int max_primes = 0;
    int thread_max_primes = 0;
    pthread_t *tid;
    int tam_buffer;
    int n_consumers;

    // Recebe e valida os parâmetros de entrada
    if (argc < 4) {
        fprintf(stderr, "Digite: %s <num_thread_consumer> <tam_buffer> [arquivo_entrada]\n", argv[0]);
        return 1;
    }
    
    n_consumers = atoi(argv[1]);
    tam_buffer = atoi(argv[2]);
    if (n_consumers < 1 || tam_buffer < 1) {
        fprintf(stderr, "Digite: %s <num_thread_consumer> <tam_buffer> [arquivo_entrada]\n", argv[0]);
        fprintf(stderr, "O número de threads consumidoras e o tamanho do buffer devem ser maiores que 0\n");
        return 2;
    }

    // Abre o arquivo de entrada
    FILE *file = fopen(argv[3], "rb");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada\n");
        return 3;
    }

    // Inicializa as variáveis de controle
    sem_init(&empty, 0, tam_buffer); // Buffer vazio inicialmente com tamanho tam_buffer 
    sem_init(&full, 0, 0); // Buffer cheio inicialmente com 0 elementos 
    sem_init(&mutex, 0, 1); // Inicializa o semáforo de exclusão mútua

    // Aloca espaço para as threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * n_consumers+1); // n_consumers consumidores + 1 produtor
    if (!tid) {
        fprintf(stderr, "Erro de alocação\n");
        return 4;
    }

    // Cria as threads consumidoras e produtora
    t_Args_prod *arg = (t_Args_prod *) malloc(sizeof(t_Args_prod));
    arg->file = file;
    if (pthread_create(tid, NULL, producer, (void *) arg)) {
        fprintf(stderr, "Erro na criação da thread produtora\n");
        return 5;
    }
    
    for (int i = 1; i <= n_consumers; i++) {
        if (pthread_create(tid+i, NULL, consumer, NULL)) {
            fprintf(stderr, "Erro na criação da thread consumidora %d\n", i);
            return 6;
        }
    }

    // Aguarda o término das threads consumidoras e produtora
    for (int i = 0; i <= n_consumers; i++) {
        int local_primes;
        if (pthread_join(*(tid+i), (void **) &local_primes)) {
            fprintf(stderr, "Erro na junção da thread %d\n", i);
            return 7;
        }
        if (i == 0) continue; // Pula a thread produtora
        
        printf("Thread %d encontrou %d números primos\n", i, local_primes);
        total_primes += local_primes;
        if (local_primes > max_primes) {
            max_primes = local_primes;
            thread_max_primes = i;
        }
    }

    // Exibe o resultado
    printf("Número totais de Primos na sequência: %d\n", total_primes);
    printf("Thread %d com mais números primos encontrados - %d\n", thread_max_primes, max_primes);

    // Libera a memória alocada
    free(tid);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}