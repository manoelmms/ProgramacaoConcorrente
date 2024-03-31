/*
 * Calcula a soma de um vetor de doubles de tamanho tam de forma concorrente com n threads
 * Manoel Silva
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TESTE
#define MARGEM 0.000000001 // 10^-9
double *vet; // Vetor de reais

typedef struct {
    long int block_size; // Tamanho do bloco
    int id; // Identificador da thread
} t_Args;

typedef struct {
    double sum; // Soma do bloco
} t_Sum;

void *task_sum(void *arg){
    t_Args *args = (t_Args *) arg;
    t_Sum *sum = (t_Sum *) malloc(sizeof(t_Sum));
    sum->sum = 0;
    long int start = args->block_size * args->id; // Início do bloco
    long int end = start + args->block_size; // Fim do bloco

    // Soma os elementos do bloco
    for (long int i = start; i < end; i++){
        sum->sum += vet[i];
        printf("Thread %d: %.18lf\n", args->id, vet[i]);
    }
    printf("Thread %d: %.18lf\n", args->id, sum->sum);
    // Retorna a soma do bloco
    pthread_exit((void *) sum);
}

int main(int argc, char *argv[]){
    long int tam = 0; // Tamanho do vetor
    double resultado_final = 0; // Resultado final
    int n_threads = 0; // Número de threads
    t_Sum *retorno; // Retorno das threads
    long int block_size; // Tamanho do bloco
    pthread_t *tid; // Identificadores das threads
    double resultado_ideal; // Resultado retornado pelo gerador do vetor

    // Verifica se o número de argumentos está correto
    if (argc < 2){
        printf("Uso: %s <número de threads>\n", argv[0]);
        return 1;
    }
    n_threads = atoi(argv[1]);
    if (n_threads < 1){
        printf("Número de threads inválido\n");
        return 2;
    }
    printf("Número de threads: %d\n", n_threads);

    // Lê o tamanho do vetor
    scanf("%ld", &tam);
    printf("Tamanho do vetor: %ld\n", tam);

    // Aloca o vetor
    vet = (double *) malloc(sizeof(double) * tam);
    if (vet == NULL){
        printf("--ERRO: malloc()\n"); exit(-1);
    }

    // Lê o vetor
    for (long int i = 0; i < tam; i++){
        scanf("%lf", &vet[i]);
    }
    
    #ifdef TESTE
        scanf("%lf", &resultado_ideal);
    #endif

    // Criar as threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * n_threads);
    if (tid == NULL){
        printf("--ERRO: malloc()\n"); exit(-1);
    }
    
    block_size = tam / n_threads;
    if(!block_size){
        printf("Aviso! Número de threads maior que o tamanho do vetor\n");
    }

    for (int i = 0; i < n_threads; i++){
        t_Args *arg = malloc(sizeof(t_Args));
        if (arg == NULL){
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        arg->block_size = block_size;
        arg->id = i;
    
        if (pthread_create(tid + i, NULL, task_sum, (void *) arg)){
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    if (tam % n_threads){
        for (long int i = block_size * n_threads; i < tam; i++){
            resultado_final += vet[i];
            printf("Thread principal: %.18lf\n", vet[i]);
        }
    }
    // Aguardar o término das threads
    for (int i = 0; i < n_threads; i++){
        if (pthread_join(tid[i], (void **) &retorno)){
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
        printf("Thread %d retornou %.18lf\n", i, retorno->sum);
        resultado_final += retorno->sum;
    }
    printf("Soma: %.18lf\n", resultado_final);
    printf("Resultado ideal: %.18lf\n", resultado_ideal);

    #ifdef TESTE
        if (resultado_final > resultado_ideal + MARGEM || resultado_final < resultado_ideal - MARGEM){
            printf(">Erro! Resultado incorreto\n");
        } else {
            printf(">Resultado correto\n");
        }
    #endif

    // Libera as áreas de memória alocadas
    free(vet);
    free(tid);
    free(retorno);
    return 0;
}

    