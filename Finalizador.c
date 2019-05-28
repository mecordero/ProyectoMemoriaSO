#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "definiciones.h"
#include <semaphore.h> 

int main(int argc, char **argv)
{
	int shm_id;
	key_t key = ftok("/tmp/shmfile", 65);
	shm_id = shmget(key, 0, 0666|IPC_CREAT);
	shmctl(shm_id, IPC_RMID, NULL);

	int shm_id_AH;
    key_t key_AH = ftok("/tmp", 50);
    shm_id_AH = shmget(key_AH, sizeof(struct AtributosHilo) * MAX_HILOS, 0666 | IPC_CREAT);
	shmctl(shm_id_AH, IPC_RMID, NULL);
	return 0;
}

