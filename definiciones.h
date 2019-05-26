#include <stdio.h>
#include <pthread.h>
#include <semaphore.h> 
#ifndef definiciones_h
#define definiciones_h

#define MAX_HILOS 10000

enum tipoEstado {activo = 1, ejecutando = 2, bloqueado = 3};

struct AtributosHilo
{
    char nombre;
	int id;
	int algoritmo;
	int lineas;
	int tiempo;
	int tipo;
	sem_t mutex;
    pthread_t thread;
};


#endif
