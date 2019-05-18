#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int algoritmo;
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
	key_t key = ftok("shmfile", 65);
	shm_id = shmget(key, 0, 0666|IPC_CREAT);
	printf("El id generado es %d\n", shm_id);
	srand(time(0));
	while(1){
		int lineasHilo = (rand() % 10) + 1;
		int tiempoHilo = (rand() % (60 - 20 + 1)) + 20;
		int tiempoEspera = (rand() % (60 - 30 + 1)) + 30;
		printf("Lineas: %d\n", lineasHilo);
		printf("Tiempo: %d\n", tiempoHilo);
		printf("TiempoEspera: %d\n\n", tiempoEspera);
		sleep(tiempoEspera);
	}
	return 0;
}

