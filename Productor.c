#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "definiciones.h"

char caracterActual = 48;

void *vidaHilo(void *atributosHilo)
{
	struct AtributosHilo *atributos = (struct AtributosHilo *)atributosHilo;
	struct AtributosHilo atributosCopia = {atributos->id, atributos->algoritmo, atributos->lineas, atributos->tiempo, atributos->tipo};	
	printf("Nace hilo %c con lineas %d\n\n", atributosCopia.id, atributosCopia.lineas);	
	sleep(atributosCopia.tiempo);
	printf("Muere hilo %c con lineas %d\n\n", atributosCopia.id, atributosCopia.lineas);
	return NULL;
}

char nuevoChar()
{
	if(caracterActual == 57)
	{
		caracterActual = 65;
	}
	else if (caracterActual == 90)
	{
		caracterActual = 97;
	}
	else
		caracterActual++;
	return caracterActual;
}

int main(int argc, char **argv)
{
	int algoritmo;
	int shm_id_AH;
	int shm_id;

	printf("Ingrese el numero del algoritmo que desea utilizar para la distribucion de memoria\n");
	printf("1. Best-Fit\n");
	printf("2. First-Fit\n");
	printf("3. Worst-Fit\n");
	scanf("%d", &algoritmo);
	while(algoritmo < 1 || algoritmo > 3){
		printf("Ingrese un numero correspondiente a un algoritmo\n");
		scanf("%d", &algoritmo);
	}
	key_t key_AH = ftok("shmfile",50);
	shm_id_AH = shmget(key_AH, sizeof(struct AtributosHilo ) * MAX_HILOS, 0666|IPC_CREAT);
	struct AtributosHilo *hilos;
	hilos = shmat(shm_id_AH, NULL, 0);


	key_t key = ftok("shmfile", 65);
	shm_id = shmget(key, 0, 0666|IPC_CREAT);
	printf("El id generado es %d\n", shm_id);
	srand(time(0));
	int cont_ID = 0;// contador de hilos

	while(1){
		int lineasHilo = (rand() % 10) + 1;
		int tiempoHilo = (rand() % (60 - 20 + 1)) + 20;
		int tiempoEspera = (rand() % (60 - 30 + 1)) + 30;
		printf("Lineas: %d\n", lineasHilo);
		printf("Tiempo: %d\n", tiempoHilo);
		printf("TiempoEspera: %d\n\n", tiempoEspera);	


		char c = nuevoChar();
		hilos[cont_ID].nombre = c;
		hilos[cont_ID].id = cont_ID;
		hilos[cont_ID].algoritmo = algoritmo;
		hilos[cont_ID].lineas = lineasHilo;
		hilos[cont_ID].tiempo = tiempoHilo;
		hilos[cont_ID].tipo = activo;

		pthread_create(&hilos[cont_ID].thread, NULL, vidaHilo, &hilos[cont_ID]);
		//pthread_join(hiloNuevo, NULL);		
		sleep(tiempoEspera);
		cont_ID++;
	}
	return 0;
}

