#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "definiciones.h"
#include <semaphore.h>

/***
 * Imprime el estado de los procesos en tres categorías:
 *  - El PID de los proceso que tenga acceso a la memoria en ese momento
 *  - El PID de los procesos que estén “ejecutando”
 *  - El PID de los procesos que estén bloqueados
*/

void imprimirEstadoPIDS(struct AtributosHilo *hilos)
{
    int actual = 0;
    printf("\n\n-------------------------------\n");
    printf("Procesos con acceso a memoria: \n");
    while (hilos[actual].id != -1 )
    {
        if (hilos[actual].tipo == activo)
        {
            printf("%c,", hilos[actual].nombre);
        }
        actual++;
    }

    actual = 0;
    printf("\n\n-------------------------------\n");
    printf("Procesos en ejecución: \n");
    while (hilos[actual].id != -1)
    {
        if (hilos[actual].tipo == ejecutando)
        {
            printf("%c,", hilos[actual].nombre);
        }
        actual++;
    }

    actual = 0;
    printf("\n\n-------------------------------\n");
    printf("Procesos bloqueados: \n");
    while (hilos[actual].id != -1)
    {
        if (hilos[actual].tipo == bloqueado)
        {
            printf("%c,", hilos[actual].nombre);
        }
        actual++;
    }

    actual = 0;
    printf("\n\n-------------------------------\n");
    printf("Procesos finalizados: \n");
    while (hilos[actual].id != -1)
    {
        if (hilos[actual].tipo == finalizado)
        {
            printf("%c, ", hilos[actual].nombre);
        }
        actual++;
    }
}

void *menu(char *memoria, struct AtributosHilo *hilos)
{

    printf("\n\n\n-------------------------------------------------------------------------------\n");
    printf("-- Comandos:                                                                 --\n");
    printf("--      1   Estado del archivo                                               --\n");
    printf("--      2   Estado de los procesos                                           --\n");
    printf("--      3   Terminar todos los procesos                                      --\n");
    printf("-------------------------------------------------------------------------------\n");

    char option = '\0';
    while (option != '3')
    {
        option = '\0';
        scanf("%1c", &option);
        if (option != '\n')
        {
            switch (option)
            {
            case '1':
                printf("----------------------------\n");
                printf("Estado del archivo: \n");
                printf("----------------------------\n");
                // Imprime el estado de la memoria
                for (int c = 0; memoria[c] != '\0'; c++)
                {
                    printf("%c", memoria[c]);
                }
                printf("\n");
                break;
            case '2':
                printf("----------------------------\n");
                printf("Estado de los procesos: \n");
                printf("----------------------------\n");
                imprimirEstadoPIDS(hilos);
                break;
            case '3':
                printf("----------------------------\n");
                printf("Cerrando todos los procesos:\n");
                printf("----------------------------\n");
                
                int shm_id;
                key_t key = ftok("/tmp/shmfile", 65);
                shm_id = shmget(key, 0, 0666|IPC_CREAT);
                shmctl(shm_id, IPC_RMID, NULL);

                int shm_id_AH;
                key_t key_AH = ftok("/tmp", 50);
                shm_id_AH = shmget(key_AH, sizeof(struct AtributosHilo) * MAX_HILOS, 0666 | IPC_CREAT);
                shmctl(shm_id_AH, IPC_RMID, NULL);
                return 0;
                break;
            }

            printf("\n\n\n----------------------------\n");
            printf("Nuevo comando => ");
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    /*
     * Se inicializan las variables necesarias para la memoria compartida
     */
    int shm_id_AH;
    char *memoria;

    /**
     * Memoria con el archivo de la memoria
    */
    key_t key = ftok("/shmfile", 65);
    int shm_id = shmget(key, 0, 0666 | IPC_CREAT);
    memoria = shmat(shm_id, NULL, 0);

    /**
     * Memoria compartida con la información de los hilos
    */
    key_t key_AH = ftok("/tmp", 50);
    shm_id_AH = shmget(key_AH, sizeof(struct AtributosHilo) * MAX_HILOS, 0666 | IPC_CREAT);
    struct AtributosHilo *hilos;
    hilos = shmat(shm_id_AH, NULL, 0);

    menu(memoria, hilos);
}
