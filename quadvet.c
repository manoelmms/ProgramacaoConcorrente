#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  int idThread, nThreads;
  int *vetor;
  int pos;
} t_Args;

int* cria_vetor_rand(int tam){
    int *vetor = (int *) malloc(sizeof(int) * tam);
    for (int i = 0; i < tam; i++){
        vetor[i] = rand() % 100;
    }
    return vetor;
}

int* cria_vetor(int tam){
    int *vetor = (int *) malloc(sizeof(int) * tam);
    for (int i = 0; i < tam; i++){
        vetor[i] = i + 1;
    }
    return vetor;
}

void *quad_vetor (void *arg){
    t_Args args = *(t_Args*) arg;
    args.vetor[args.pos] = args.vetor[args.pos] * args.vetor[args.pos];
    printf("Sou a thread %d de %d threads e meu resultado é %d\n", args.idThread + 1, args.nThreads, args.vetor[args.pos]);
    free(arg);
    pthread_exit(NULL);

}

int main(){

    int TAM = 10;
    int *vetor = cria_vetor(TAM);
    for (int i = 0; i<10; i++){
        printf("%d ", vetor[i]);
    }
    printf("\n");

    pthread_t tid[TAM];
    for (int i = 0; i < TAM; i++){
        t_Args *arg = malloc(sizeof(t_Args));
        if (arg == NULL){
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        arg->idThread = i;
        arg->nThreads = TAM;
        arg->vetor = vetor;
        arg->pos = i;
        if (pthread_create(&tid[i], NULL, quad_vetor, (void *) arg)){
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    
    for (int i = 0; i<TAM; i++){
        printf("%d ", vetor[i]);
    }
    printf("\n");
    printf("--Thread principal terminou\n");
    pthread_exit(NULL);
    return 0;
}
