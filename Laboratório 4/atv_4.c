/*
 * Solução concorrente para a quantidade de números primos em um intervalo 1 a N
 * De forma ingênua e usando a bolsa de tarefas (Alocação Dinâmica de Tarefas)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

long long int N; // Número de elementos
long long int i_global = 0;
pthread_mutex_t mutex;

int is_prime(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long int i = 3; i < sqrt(n) + 1; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void *task(void *arg) {
    long long int i_local;
    long long int local_primes = 0;

    do {
        // Acesso e atualização da variável global i_global de forma segura
        pthread_mutex_lock(&mutex);
        i_local = i_global;
        i_global++;
        pthread_mutex_unlock(&mutex);

        if (i_local < N) { // Verifica se o intervalo foi ultrapassado
            if (is_prime(i_local)) {
                local_primes++;
            }
        }
    } while (i_local < N);

    // Retorna a quantidade de primos encontrados
    int *ret = (int *) malloc(sizeof(int));
    *ret = local_primes;
    pthread_exit((void *) ret);
}

int main(int argc, char *argv[]) {
    double start, finish, elapsed;
    GET_TIME(start);

    // Variáveis locais
    int n_threads;
    long long int local_primes;
    long long int total_primes = 0;
    pthread_t *tid;
    int *ret;

    // Inicialização da variável global
    pthread_mutex_init(&mutex, NULL);

    // Leitura e avaliação dos parâmetros de entrada
    if (argc < 3) {
        fprintf(stderr, "Digite: %s <N> <n_threads>\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]);
    n_threads = atoi(argv[2]);

    // Alocação de memória para as estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * n_threads);
    if (tid == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    // Criação das threads
    for (int i = 0; i < n_threads; i++) {
        if (pthread_create(tid + i, NULL, task, NULL)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }

    // Aguarda o término das threads
    for (int i = 0; i < n_threads; i++) {
        if (pthread_join(*(tid + i), (void **) &ret)) {
            fprintf(stderr, "ERRO--pthread_join\n");
            return 4;
        }
        local_primes = *ret;
        total_primes += local_primes;
        free(ret);
    }

    // Exibição do resultado
    printf("Número totais de Primos na sequência: %lld\n", total_primes);

    // Liberação da memória alocada
    free(tid);

    // Finalização da variável global
    pthread_mutex_destroy(&mutex);

    GET_TIME(finish);
    elapsed = finish - start;
    printf("Tempo de execução: %.4f segundos\n", elapsed);

    return 0;
}
