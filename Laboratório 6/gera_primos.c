/* Programa para gerar um arquivo binário com números primos para testes */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long int i = 3; i < sqrt(n) + 1; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int miller_rabin(long long int n, int k) {
    /* Teste de primalidade de Miller-Rabin
     * n: número a ser testado
     * k: número de iterações
     * Retorna 1 se n é primo e 0 caso contrário
     */

    if (n == 2 || n == 3) return 1;
    if (n <= 1 || n % 2 == 0) return 0;

    long long int s = 0, d = n - 1;
    while (d % 2 == 0) {
        s++;
        d /= 2;
    }

    for (int i = 0; i < k; i++) {
        long long int a = 2 + rand() % (n - 3);
        long long int x = (long long int)pow(a, d) % n;

        if (x == 1 || x == n - 1) continue;

        for (int j = 0; j < s - 1; j++) {
            x = (x * x) % n;
            if (x == 1) return 0;
            if (x == n - 1) break;
        }

        if (x != n - 1) return 0;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <número de primos> <arquivo de saída>\n", argv[0]);
        return 1;
    }

    long long int n_primes = atoll(argv[1]);
    FILE *file = fopen(argv[2], "wb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    long long int i = 1;
    while (n_primes > 0) {
        fwrite(&i, sizeof(long long int), 1, file);
        if (is_prime(i)) {
            printf("%lld\n", i);
            n_primes--;
        }
        i++;
    }
    fclose(file);
    return 0;
}