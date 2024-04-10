/*
 * Multiplicação de matrizes MxN e NxP de forma sequencial
 */

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

typedef struct {
    int linhas;
    int colunas;
    float *elementos;
} Matriz;

Matriz *le_matriz_bin(const char *arquivo) {
    Matriz *matriz = (Matriz *) malloc(sizeof(Matriz));
    FILE * descritor_arquivo; //descritor do arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada

    if (!matriz) {
        fprintf(stderr, "Erro de alocação da matriz\n");
        return NULL;
    }
    
    descritor_arquivo = fopen(arquivo, "rb");
    if (!descritor_arquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return NULL;
    }

    ret = fread(&matriz->linhas, sizeof(int), 1, descritor_arquivo);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões da matriz\n");
        return NULL;
    }

    ret = fread(&matriz->colunas, sizeof(int), 1, descritor_arquivo);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões da matriz\n");
        return NULL;
    }

    matriz->elementos = (float *) malloc(sizeof(float) * matriz->linhas * matriz->colunas);
    if (!matriz->elementos) {
        fprintf(stderr, "Erro de alocação da matriz\n");
        return NULL;
    }

    ret = fread(matriz->elementos, sizeof(float), matriz->linhas * matriz->colunas, descritor_arquivo);
    if (ret < matriz->linhas * matriz->colunas) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return NULL;
    }

    return matriz;
}

void escreve_matriz_bin(const char *arquivo, Matriz *matriz) {
    FILE * descritor_arquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida

    descritor_arquivo = fopen(arquivo, "wb");
    if (!descritor_arquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return;
    }

    ret = fwrite(&matriz->linhas, sizeof(int), 1, descritor_arquivo);
    if (!ret) {
        fprintf(stderr, "Erro de escrita das dimensões da matriz\n");
        return;
    }

    ret = fwrite(&matriz->colunas, sizeof(int), 1, descritor_arquivo);
    if (!ret) {
        fprintf(stderr, "Erro de escrita das dimensões da matriz\n");
        return;
    }

    ret = fwrite(matriz->elementos, sizeof(float), matriz->linhas * matriz->colunas, descritor_arquivo);
    if (ret < matriz->linhas * matriz->colunas) {
        fprintf(stderr, "Erro de escrita dos elementos da matriz\n");
        return;
    }
}
 
void imprime_matriz(Matriz *matriz) {
    for (int i = 0; i < matriz->linhas; i++) {
        for (int j = 0; j < matriz->colunas; j++)
            fprintf(stdout, "%.6f ", matriz->elementos[i * matriz->colunas + j]);
        fprintf(stdout, "\n");
    }
}

Matriz *multiplica_matrizes(Matriz *A, Matriz *B) {
    if (A->colunas != B->linhas) {
        fprintf(stderr, "Erro: Matrizes incompatíveis para multiplicação\n");
        return NULL;
    }

    Matriz *C = (Matriz *) malloc(sizeof(Matriz));
    if (!C) {
        fprintf(stderr, "Erro de alocação da matriz\n");
        return NULL;
    }

    C->linhas = A->linhas;
    C->colunas = B->colunas;
    C->elementos = (float *) malloc(sizeof(float) * C->linhas * C->colunas);
    if (!C->elementos) {
        fprintf(stderr, "Erro de alocação da matriz\n");
        return NULL;
    }

    for (int i = 0; i < C->linhas; i++) {
        for (int j = 0; j < C->colunas; j++) {
            C->elementos[i * C->colunas + j] = 0; // C[i][j] = 0 (inicializa com zero)
            for (int k = 0; k < A->colunas; k++)
                C->elementos[i * C->colunas + j] += A->elementos[i * A->colunas + k] * B->elementos[k * B->colunas + j]; // C[i][j] += A[i][k] * B[k][j]
        }
    }
    return C;
}

int main(int argc, char *argv[]) {
    Matriz *A, *B, *C;
    double inicio, fim;

    if (argc < 4) {
        fprintf(stderr, "Digite: %s <matriz1> <matriz2> <matriz_saida>\n", argv[0]);
        return 1;
    }

    A = le_matriz_bin(argv[1]);
    if (!A) return 2;

    B = le_matriz_bin(argv[2]);
    if (!B) return 3;

    GET_TIME(inicio);
    C = multiplica_matrizes(A, B);
    GET_TIME(fim);

    if (!C) return 4;

    printf("Tempo de multiplicação: %f\n", fim - inicio);

    // imprime_matriz(C);
    escreve_matriz_bin(argv[3], C);

    free(A->elementos);
    free(A);
    free(B->elementos);
    free(B);
    free(C->elementos);
    free(C);

    return 0;
}
    