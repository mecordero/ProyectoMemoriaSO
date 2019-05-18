#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int shm_id;
	key_t key = ftok("shmfile", 65);
	shm_id = shmget(key, 0, 0666|IPC_CREAT);
	shmctl(shm_id, IPC_RMID, NULL);
	return 0;
}

