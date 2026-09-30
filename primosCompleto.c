#include <stdio.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// Variáveis globais compartilhadas pelas threads
int prox_num = 1;
int max_primos = 0;
int chunks_processados = 0;
int mostrar_progresso = 0;

const int limite = 5000000;
const int chunk = 5000;


// Mutex utilizado para proteger as variáveis compartilhadas
pthread_mutex_t mutex;


// Função para verificar se um número é primo
int is_prime(int n)
{
if (n < 2)
{
return 0;
}

if (n == 2)
{
return 1;
}

if (n % 2 == 0)
{
return 0;
}

int maximo = sqrt(n);

for (int i = 3; i <= maximo; i += 2)
{
if (n % i == 0)
{
return 0;
}
}

return 1;
}


// Função executada pelas threads
void *buscar_chunks_primos(void *arg)
{
while (1)
{
int local_inicial;
int local_final;
int meus_primos = 0;


/*
* Região crítica:
* apenas uma thread por vez pode retirar
* um novo chunk de prox_num.
*/
pthread_mutex_lock(&mutex);

if (prox_num > limite)
{
pthread_mutex_unlock(&mutex);
break;
}

local_inicial = prox_num;
prox_num += chunk;

pthread_mutex_unlock(&mutex);


/*
* Define o final do chunk.
*/
local_final = local_inicial + chunk - 1;

if (local_final > limite)
{
local_final = limite;
}


/*
* O cálculo dos primos acontece FORA do mutex.
* Assim, várias threads podem calcular
* simultaneamente.
*/
for (int i = local_inicial; i <= local_final; i++)
{
if (is_prime(i))
{
meus_primos++;
}
}


/*
* Região crítica:
* apenas uma thread por vez atualiza
* o total global.
*/
pthread_mutex_lock(&mutex);

max_primos += meus_primos;
chunks_processados++;

if (mostrar_progresso)
{
    int porcentagem =
        (chunks_processados * 100) / (limite / chunk);

    printf("\rProgresso: %d%%", porcentagem);
    fflush(stdout);
}

pthread_mutex_unlock(&mutex);
}

return NULL;
}


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    struct timespec comeco, fim;

    /*
    * Verifica se o número de threads ou "benchmark"
    * foi informado.
    */
    if (argc != 2)
    {
        printf("Uso correto: %s <numero_de_threads | benchmark>\n", argv[0]);
        return 1;
    }


    /*
    * Inicializa o mutex.
    */
    pthread_mutex_init(&mutex, NULL);


    /*
    * Verifica se foi solicitado o benchmark.
    */
    if (strcmp(argv[1], "benchmark") == 0)
    {
        int quantidades_threads[] = {1, 2, 4, 6, 8};
        mostrar_progresso = 0;

        printf("\n===== BENCHMARK =====\n");

        for (int x = 0; x < 5; x++)
        {
            int k = quantidades_threads[x];

            /*
            * Reinicia as variáveis compartilhadas
            * para uma nova execução.
            */
            prox_num = 1;
            max_primos = 0;

            printf("\n------------------------------\n");
            printf("Executando com %d threads\n", k);
            printf("------------------------------\n");


            /*
            * Cria o vetor de threads.
            */
            pthread_t threads[k];


            /*
            * Inicia a medição do tempo.
            */
            clock_gettime(CLOCK_MONOTONIC, &comeco);


            /*
            * Cria as threads.
            */
            for (int i = 0; i < k; i++)
            {
                pthread_create(
                    &threads[i],
                    NULL,
                    buscar_chunks_primos,
                    NULL
                );
            }


            /*
            * Espera todas as threads terminarem.
            */
            for (int i = 0; i < k; i++)
            {
                pthread_join(
                    threads[i],
                    NULL
                );
            }


            /*
            * Finaliza a medição do tempo.
            */
            clock_gettime(CLOCK_MONOTONIC, &fim);


            /*
            * Calcula o tempo em milissegundos.
            */
            double tempo_ms =
            (fim.tv_sec - comeco.tv_sec) * 1000.0 +
            (fim.tv_nsec - comeco.tv_nsec) / 1000000.0;


            /*
            * Mostra os resultados.
            */
            printf(
                "Foram encontrados: %d numeros primos\n",
                max_primos
            );

            printf(
                "Tempo: %.2f ms\n",
                tempo_ms
            );
        }
    }
    else
    {
        /*
        * Modo normal:
        * converte o argumento para inteiro.
        */
        int k = atoi(argv[1]);
        mostrar_progresso = 1; 


        /*
        * Se k = 0, detecta automaticamente
        * o número de CPUs disponíveis.
        */
        if (k == 0)
        {
            k = sysconf(_SC_NPROCESSORS_ONLN);

            printf(
                "Detectado k = 0. Usando %d CPUs.\n",
                k
            );
        }
        else
        {
            printf(
                "Inicializando com %d threads.\n",
                k
            );
        }


        /*
        * Cria o vetor de threads.
        */
        pthread_t threads[k];


        /*
        * Inicia a medição do tempo.
        */
        clock_gettime(CLOCK_MONOTONIC, &comeco);


        /*
        * Cria as threads.
        */
        for (int i = 0; i < k; i++)
        {
            pthread_create(
                &threads[i],
                NULL,
                buscar_chunks_primos,
                NULL
            );
        }


        /*
        * Espera todas as threads terminarem.
        */
        for (int i = 0; i < k; i++)
        {
            pthread_join(
                threads[i],
                NULL
            );
        }


        /*
        * Finaliza a medição do tempo.
        */
        clock_gettime(CLOCK_MONOTONIC, &fim);


        /*
        * Calcula o tempo em milissegundos.
        */
        double tempo_ms =
            (fim.tv_sec - comeco.tv_sec) * 1000.0 +
            (fim.tv_nsec - comeco.tv_nsec) / 1000000.0;


        /*
        * Mostra os resultados.
        */
        printf("\n");

        printf(
            "Foram encontrados: %d numeros primos!\n",
            max_primos
        );

        printf(
            "Levou aproximadamente: %.2f ms\n",
            tempo_ms
        );
    }


    /*
    * Libera o mutex.
    */
    pthread_mutex_destroy(&mutex);


    return 0;
}
