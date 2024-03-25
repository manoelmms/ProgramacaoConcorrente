#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 100
#define TAM 100000
#define NTHREADS 93
#define MARGEM 0.00000000001

typedef struct {
    int idThread, nThreads;
    float *start_vet;
    int limit;
} t_Args;

float* init_vet(int tam){
    float *vetor = (float *) malloc(sizeof(float) * tam);
    if (vetor == NULL){
        printf("--ERRO: malloc()\n"); exit(-1);
    }
    return vetor;
    
}

// void rand_vet(long int *vetor, int tam){
//     srand(time(NULL));
//     for (int i = 0; i < tam; i++){
//         long int number = rand() % (2*MAX) - MAX; // Random number between -MAX and MAX
//         vetor[i] = number;
//     }
// }

void rand_float_vet(float *vetor, int tam){
    srand(time(NULL));
    for (int i = 0; i < tam; i++){
        float number = ((2*MAX) * ((float)rand() / RAND_MAX)) - MAX; // Random float between -MAX and MAX
        vetor[i] = number;
    }
}

// void seq_vet(long int *vetor, int tam){
//     for (int i = 0; i < tam; i++){
//         vetor[i] = i + 1;
//     }
// }

void* duplicate_vet(float *vetor, int tam){
    float *new_vet = (float *) malloc(sizeof(float) * tam);
    if (new_vet == NULL){
        printf("--ERRO: malloc()\n"); exit(-1);
    }
    for (int i = 0; i < tam; i++){
        new_vet[i] = vetor[i];
    }
    return (void *) new_vet;
    
}

void print_vet(float *vet, int tam){
    for (int i = 0; i < tam; i++){
        printf("%.1f ", vet[i]);
    }
    printf("\n");
}

int testa_result(float *original, float *result, int tam){
    for (int i = 0; i < tam; i++){
        if (result[i] > original[i] * original[i] + MARGEM || result[i] < original[i] * original[i] - MARGEM){
            printf("Erro na posição %d\n", i);
            printf("> Original: %.1f\n ", original[i]);
            printf("> Resultado: %.1f\n ", result[i]);
            return 0;
        }
    }
    return 1;
}


void* quad_vet(void *arg){
    t_Args args = *(t_Args*) arg;
    for (int i = 0; i < args.limit; i++){
        args.start_vet[i] = args.start_vet[i] * args.start_vet[i];
        printf("Sou a thread %d de %d threads e meu resultado é %.1f\n", args.idThread + 1, args.nThreads, args.start_vet[i]);
    }
    free(arg);
    pthread_exit(NULL);
}

int main(){

    float *vetor = init_vet(TAM);
    //rand_vet(vetor, TAM);
    //seq_vet(vetor, TAM);
    rand_float_vet(vetor, TAM);
    float *test = duplicate_vet(vetor, TAM);

    printf("Vetor original:\n ");
    print_vet(test, TAM);

    pthread_t tid[NTHREADS];
    t_Args *arg;

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

        arg->start_vet = vetor + i * (TAM / NTHREADS);


        if (pthread_create(&tid[i], NULL, quad_vet, (void *) arg)){
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    for (int i = 0; i < NTHREADS; i++){
        if (pthread_join(tid[i], NULL)){
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
    }

    // printf("\nResultado: ");
    // print_vet(vetor, TAM);

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
