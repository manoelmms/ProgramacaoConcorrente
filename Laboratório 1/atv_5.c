#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 100
#define TAM 10000
#define NTHREADS 23

typedef struct {
    int idThread, nThreads;
    int *start_vet;
    long int limit;
} t_Args;

int* init_vet(long int tam){
    int *vetor = (int *) malloc(sizeof(int) * tam);
    if (vetor == NULL){
        printf("--ERRO: malloc()\n"); exit(-1);
    }
    return vetor;
    
}

void rand_vet(int *vetor, long int tam){
    srand(time(NULL));
    for (long int i = 0; i < tam; i++){
        long int number = rand() % (2*MAX) - MAX; // Random number between -MAX and MAX
        vetor[i] = number;
    }
}

void seq_vet(int *vetor, long int tam){
    for (long int i = 0; i < tam; i++){
        vetor[i] = i + 1;
    }
}

void* duplicate_vet(int *vetor, long int tam){
    int *new_vet = (int *) malloc(sizeof(int) * tam);
    if (new_vet == NULL){
        printf("--ERRO: malloc()\n"); exit(-1);
    }
    for (long int i = 0; i < tam; i++){
        new_vet[i] = vetor[i];
    }
    return (void *) new_vet;
    
}

void print_vet(int *vet, long int tam){
    for (long int i = 0; i < tam; i++){
        printf("%d ", vet[i]);
    }
    printf("\n");
}

int testa_result(int *original, int *result, long int tam){
    // Sequential algorithm to compare the results
    for (long int i = 0; i < tam; i++){
        if (result[i] != original[i] * original[i]){
            printf("Erro na posição %ld\n", i);
            printf("> Original: %d\n ", original[i]);
            printf("> Resultado: %d\n ", result[i]);
            return 0;
        }
    }
    return 1;
}

void* quad_vet(void *arg){
    t_Args args = *(t_Args*) arg;
    for (long int i = 0; i < args.limit; i++){
        args.start_vet[i] = args.start_vet[i] * args.start_vet[i];
        printf("Sou a thread %d de %d threads e meu resultado é %d\n", args.idThread + 1, args.nThreads, args.start_vet[i]);
    }
    free(arg);
    pthread_exit(NULL);
}

int main(){
    // Initializing data structures
    int *vetor = init_vet(TAM);
    rand_vet(vetor, TAM);
    //seq_vet(vetor, TAM);
    int *test = duplicate_vet(vetor, TAM);

    printf("Vetor original:\n ");
    print_vet(test, TAM);

    pthread_t tid[NTHREADS];
    t_Args *arg;

    //Warning in case of NTHREADS > TAM
    if (NTHREADS > TAM){
        printf(">Warning!!!: NTHREADS > TAM\n"); // Still runs without problem, but the threads will be idle
    }
    
    // Creating threads
    for (int i = 0; i < NTHREADS; i++){
        arg = malloc(sizeof(t_Args));
        if (arg == NULL){
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        arg->idThread = i;
        arg->nThreads = NTHREADS;

        // Divide vector in NTHREADS parts, if it's the last thread, it will take the rest of the division
        // This way, we guarantee that all threads will have the same amount of work 
        
        if (i == NTHREADS - 1){
            arg->limit = TAM / NTHREADS + TAM % NTHREADS;
        } else {
            arg->limit = TAM / NTHREADS;
        }

        arg->start_vet = vetor + i * (TAM / NTHREADS); // Pointer to the beginning of the part of the vector that the thread will work on


        if (pthread_create(&tid[i], NULL, quad_vet, (void *) arg)){
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    for (int i = 0; i < NTHREADS; i++){
        if (pthread_join(tid[i], NULL)){
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
    }

    printf("\nResultado: ");
    print_vet(vetor, TAM);

    if (testa_result(test, vetor, TAM)){
        printf("\nVetor original foi alterado corretamente\n");
    } else {
        printf("\nVetor original não foi alterado corretamente\n");
        exit(-1);
    }

    free(vetor);
    free(test);
    pthread_exit(NULL);
    return 0;
}
