#include <stdio.h>
#include <math.h>
#include <time.h>
#include <locale.h>


int is_prime(int n) {
    if (n < 2) {
        return 0;
    } else if (n == 2) {
        return 1;
    } else if (n % 2 == 0) {
        return 0;
    } else {
        int maximo = sqrt(n);
        for (int i = 3; i <= maximo; i += 2) {
            if (n % i == 0) {
                return 0;
            }
        }
        return 1;
    }
}

int main() {
    int primos = 0;
    const int limite = 5000000;
    setlocale(LC_ALL, "");

    clock_t comeco = clock();

    for (int i = 1; i <= limite; i++) {
        if (is_prime(i)) {
            primos++;
        }
        if (i % 250000 == 0) {
            printf("#");
            fflush(stdout);
        }
    }
    printf("\n");
    clock_t fim = clock();

    printf("Foram encontrados: %d numeros primos!\n", primos);
    double tempo_ms = ((double) (fim - comeco) / CLOCKS_PER_SEC) * 1000;
    printf("Levou aproximadamente: %.2f de MS para a execucao terminar", tempo_ms);
}
