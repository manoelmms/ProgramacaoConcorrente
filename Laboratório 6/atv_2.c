/*
 * Solução concorrente para a quantidade de números primos em um arquivo de entrada binario
 * De forma ingênua e usando consumidores e produtores em um buffer pequeno
 * com apenas semáforos
 * Aviso: NÃO funciona no MacOS, pois não existe implementacao de semáforos POSIX!
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <math.h>

// Variáveis globais
sem_t empty, full; // Semáforos para controle de buffer
sem_t mutex; // Semáforo para exclusão mútua
long long int *buffer; // Buffer para armazenar os números
int tam_buffer; // Tamanho do buffer

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
    if (buffer[out] == -1) { // Verifica se é o fim do arquivo
        sem_post(&mutex);
        sem_post(&empty); 
	    sem_post(&full); 
        return -1;
    }
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
    insert_buffer(-1); // Insere um número inválido para indicar o fim do arquivo

    if(fclose(args->file)) {
        fprintf(stderr, "Erro ao fechar o arquivo\n");
        exit(1);
    }
    free(args);
    // printf("Produtor finalizado\n");
    pthread_exit(NULL);
}

void *consumer(void *arg) { 
    int *local_primes = (int *) malloc(sizeof(int));
    if (!local_primes) {
        fprintf(stderr, "Erro de alocação\n");
        pthread_exit(NULL);
    } 
    *local_primes = 0;

    long long int number;
    while (1) {
        number = remove_buffer();
	// printf("%lld\n", number);
        if (number == -1) break; // Verifica se é o fim do arquivo
        if (is_prime(number)) (*local_primes)++;
    }
    // printf("\nConsumidor finalizado\n");
    // printf("Número de primos encontrados: %d\n", *local_primes);
    // printf("Endereço do local_primes: %p\n", local_primes);

    pthread_exit((void*) local_primes);
}


int main (int argc, char *argv[]) {
    // Variáveis locais
    int total_primes = 0;
    int max_primes = 0;
    int thread_max_primes = 0;
    pthread_t *tid;
    int n_consumers;
    int *return_primes = NULL;

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
    // inicializa o buffer
    buffer = (long long int *) malloc(sizeof(long long int) * tam_buffer);
    if (!buffer) {
        fprintf(stderr, "Erro de alocação do Buffer\n");
        return 3;
    }

    // Abre o arquivo de entrada
    FILE *file = fopen(argv[3], "rb");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada\n");
        return 4;
    }

    // Inicializa as variáveis de controle
    sem_init(&empty, 0, tam_buffer); // Buffer vazio inicialmente com tamanho tam_buffer 
    sem_init(&full, 0, 0); // Buffer cheio inicialmente com 0 elementos 
    sem_init(&mutex, 0, 1); // Inicializa o semáforo de exclusão mútua

    // Aloca espaço para as threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * n_consumers+1); // n_consumers consumidores + 1 produtor
    if (!tid) {
        fprintf(stderr, "Erro de alocação\n");
        return 5;
    }

    // Cria as threads consumidoras e produtora
    t_Args_prod *arg = (t_Args_prod *) malloc(sizeof(t_Args_prod));
    arg->file = file;
    if (pthread_create(tid, NULL, producer, (void *) arg)) {
        fprintf(stderr, "Erro na criação da thread produtora\n");
        return 6;
    }
    
    for (int i = 1; i <= n_consumers; i++) {
        // printf("Criando thread consumidora %d\n", i);
        if (pthread_create(tid+i, NULL, consumer, NULL)) {
            fprintf(stderr, "Erro na criação da thread consumidora %d\n", i);
            return 7;
        }
    }

    // Aguarda o término das threads consumidoras e produtora
    for (int i = 1; i <= n_consumers; i++) {
        if (pthread_join(*(tid+i), (void**) &return_primes)) {
            fprintf(stderr, "Erro no join das threads\n");
            return 8;
        }
        if (return_primes) {
            total_primes += *return_primes;
            if (*return_primes > max_primes) {
                max_primes = *return_primes;
                thread_max_primes = i;
            }
            free(return_primes);
        }
    }

    // Exibe o resultado
    printf("Número totais de Primos na sequência: %d\n", total_primes);
    printf("Thread %d com mais números primos encontrados - %d\n", thread_max_primes, max_primes);

    // Libera a memória alocada
    free(tid);
    free(buffer);

    // Finaliza os semáforos
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}
