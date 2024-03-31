/*
 * Programa auxiliar para gerar números reais aleatórios, séries de euler, pi, harmônica e harmônica alternada 
 * Usa long double para ter precisão de 18 casas decimais
 * Manoel Silva
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100 // Máximo valor para os números aleatórios

int main(int argc, char *argv[]){
    long int tam = 0; // Tamanho do vetor
    int modo = 0; // Modo de geração do vetor (0: aleatório, 1: série de euler, 2: série pi)
    long double euler = 1; // Valor inicial da série de euler
    long double pi = 0; // Valor inicial da série de pi
    long double number; // Número aleatório
    long double soma = 0; // Soma dos números aleatórios

    if (argc < 2){
        printf("Uso: %s <tamanho do vetor> [-e | -p | -h | -ha]\n", argv[0]);
        printf("  -e: Gera uma série de euler\n");
        printf("  -p: Gera uma série de pi\n");
        printf("  -h: Gera uma série harmônica\n");
        printf("  -ha: Gera uma série harmônica alternada\n");
        return 1;
    }

    // Analisando argumentos
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-'){
            switch (argv[i][1]){
                case 'e':
                    modo = 1;
                    break;
                case 'p':
                    modo = 2;
                    break;
                case 'h':
                    modo = 3;
                    if (argv[i][2] == 'a'){
                        modo = 4;
                    }
                    break;
            }
        } else {
            tam = atol(argv[i]);
        }
    }
    // Caso não tenha sido passado o tamanho correto do vetor, encerra o programa
    if(tam < 1){
        printf("Tamanho do vetor inválido: %ld\n", tam);
        return -1;
    }
    
    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios
    printf("%ld\n", tam); // Imprime o tamanho do vetor
    //printf("%d\n", modo); // Imprime o modo de geração do vetor


    // Gera o vetor de acordo com o modo escolhido
    switch (modo){
        case 0: // Aleatório
            for (int i = 0; i < tam; i++){
                number = ((2*MAX) * ((float)rand() / RAND_MAX)) - MAX; // Número aleatório entre -MAX e MAX
                printf("%.18Lf\n", number);
                soma += number;
            }
            break;
        case 1: // Série de euler
            /*
             * Série de euler com tam termos usando o método recursivo
             * e = 1 + 1 + 1/2 + 1/6 + 1/24 + 1/120 + ...
             */
            printf("1\n");
            soma += 1;
            for (int i = 1; i < tam; i++){
                euler *= 1.0 / i * euler;
                printf("%.18Lf\n", euler);
                soma += euler;
            }
            break;
        case 2: // Série de pi
            /*
             * Série de pi com tam termos usando o método de Leibniz para pi (série alternada)
             * pi = 4 * (1 - 1/3 + 1/5 - 1/7 + 1/9 - 1/11 + ...)
             */
            
            for (int i = 1; i <= tam; i++){
                pi = 4.0 / (2*i - 1) * (i % 2 == 0 ? -1 : 1);
                printf("%.18Lf\n", pi);
                soma += pi;
            }
            break;
        case 3: // Série harmonica
            for (int i = 1; i <= tam; i++){
                number = 1.0 / i;
                printf("%.18Lf\n", number);
                soma += number;
            }
            break;
        case 4: // Série Harmônica Alternada
            for (int i = 1; i <= tam; i++){
                number = 1.0 / i * (i % 2 == 0 ? -1 : 1);
                printf("%.18Lf\n", number);
                soma += number;
            }
            break;
    }

    // Long double tem precisão de 18 casas decimais (80 bits)
    printf("%.18Lf\n", soma); // Imprime a soma dos números gerados para verificação
    return 0;
}