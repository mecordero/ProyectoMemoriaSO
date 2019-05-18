#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int lineas;
	int shm_id;
	key_t key = ftok("shmfile", 65);
	printf("Ingrese la cantidad de lineas que desea para la memoria: ");
	scanf("%d", &lineas);
	shm_id = shmget(key, lineas, 0666|IPC_CREAT);
	printf("La cantidad ingresada de lineas es %d\n", lineas);
	printf("El id generado es %d\n", shm_id);
	return 0;
}

