#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include "definiciones.h"

int main(int argc, char **argv)
{
	int lineas;
	int shm_id;
	char * memoria, *c;
	int shm_id_AH; //AH será el arreglo que contenga los hilos con su información

	key_t key = ftok("/tmp/shmfile", 65);
	key_t key_AH = ftok("/tmp",50);
	shm_id_AH= shmget(key_AH, sizeof(struct AtributosHilo ) * MAX_HILOS, 0666|IPC_CREAT); //crea memoria para guardar cada hilo y sus atributos
	printf("Ingrese la cantidad de lineas que desea para la memoria: ");
	scanf("%d", &lineas);
	shm_id = shmget(key, lineas+1, 0666|IPC_CREAT);
	memoria = shmat(shm_id, NULL, 0);

	/**
	 * Inicializa todos los hilos con valores nulos 
	 */
	struct AtributosHilo *hilos;
	hilos = shmat(shm_id_AH, NULL, 0);
	for (int cont = 0; cont < MAX_HILOS; cont++) {
		hilos[cont].nombre = '\0';
		hilos[cont].id = -1;
		hilos[cont].algoritmo = -1;
		hilos[cont].lineas = -1;
		hilos[cont].tiempo = -1;
		hilos[cont].tipo = -1;
	}


	int i;
	c = memoria;
	
	//Llena la memoria disponible de ceros
	for (i = 0; i < lineas; i++){
		*c++ = '0';
	}
	*c = '\0';

	
	
	printf("\nLa cantidad ingresada de lineas es %d\n", lineas);

	return 0;
}

