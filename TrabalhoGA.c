#include <stdio.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>


// variáveis globais compartilhadas pelas threads
int prox_num = 1;
int max_primos = 0;
const int limite = 5000000;
const int chunk = 5000;

//função pra testar primos
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

// função das threads
void* buscar_chunks_primos(void* arg) {
    while (1) {
        int local_inicial;
        int local_final;

        // mutex trava aq

        // A thread separa sua parte de execução
        local_inicial = prox_num;
        prox_num += chunk;

        // mutex abrea aq

        // verifica se os números já foram processados e se sim, para.
        if (local_inicial > limite) {
            break;
        }

        // define onde o pacote termina
        local_final = local_inicial + chunk - 1;

        // força a divisão do pacote final ser equivalente ao limite (no nosso caso 5.000.000)
        if (local_final > limite) {
            local_final = limite;
        }

        // contagem de primos dentro de seu respectivo chunk
        for (int i = local_inicial; i <= local_final; i++) {
            if (is_prime(i)) {
                // outro bloq do mutex aq
                max_primos++;
            }
        }

    }
    return NULL;
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");

    // verifica se o usuário passou o arg certo
    if (argc != 2) {
        printf("Uso correto do argumento: %s <numero_de_threads>\n", argv[0]);
        return 1;
    }

    int k = atoi(argv[1]); // converte texto para int

    // se o arg passado foi o certo, separa os núcleos automaticamente
    if (k == 0) {
        k = sysconf(_SC_NPROCESSORS_ONLN);
        printf("Detectado k = 0. Usando %d núcleos.\n", k);
    } else {
        printf("Inicializando com %d threads\n", k);
    }

    clock_t comeco = clock();

    // vetor de threads
    pthread_t threads[k];

    // cada [k] thread terá sua função
    for (int i = 0; i < k; i++) {
        pthread_create(&threads[i], NULL, buscar_chunks_primos, NULL);
    }

    // block na main para finalizar o processo até todos terminarem
    for (int i = 0; i < k; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    clock_t fim = clock();

    printf("Foram encontrados: %d numeros primos!\n", max_primos);
    double tempo_ms = ((double) (fim - comeco) / CLOCKS_PER_SEC) * 1000;
    printf("Levou aproximadamente: %.2f de MS para a execucao terminar", tempo_ms);

    return 0;
}
