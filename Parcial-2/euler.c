#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_HILOS 4
#define ITERACIONES 20000

double e_global = 0.0;

pthread_mutex_t mutex;

// funcion para calcular factorial
double factorial(int n)
{
    double fact = 1.0;

    for(int i = 1; i <= n; i++)
    {
        fact *= i;
    }

    return fact;
}

// funcion que ejecutara cada hilo
void *calcular_e(void *arg)
{
    int id = *(int *)arg;

    // divide el trabajo entre los hilos
    int inicio = (ITERACIONES / NUM_HILOS) * id;
    int fin = (ITERACIONES / NUM_HILOS) * (id + 1);

    // aqui el ultimo hilo va tomar las restantes
    if(id == NUM_HILOS - 1)
    {
        fin = ITERACIONES;
    }

    double suma_local = 0.0; //cada hilo guarda su suma aqui primero para evitar una concurrencia

    //se calculan los terminos
    for(int i = inicio; i < fin; i++)
    {
        suma_local += 1.0 / factorial(i);
    }

    // seccion critica
    pthread_mutex_lock(&mutex);

    e_global += suma_local; //aqui se agregan los resultados de todos los hilos

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main()
{
    pthread_t hilos[NUM_HILOS];
    int ids[NUM_HILOS];

    pthread_mutex_init(&mutex, NULL);

    //creo los hilos
    for(int i = 0; i < NUM_HILOS; i++)
    {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, calcular_e, &ids[i]);
    }

    // espero a que cada hilo termine su trabajo
    for(int i = 0; i < NUM_HILOS; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("Valor aproximado de e: %.9f\n", e_global);

    return 0;
}