#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "definiciones.h"
#include <semaphore.h> 

char caracterActual = 48;

void *vidaHilo(void *atributosHilo)
{
	struct ListaEspacios * l, lis;
	l = &lis;
	l->primero = NULL;
	char * memoria, *c;
	key_t key = ftok("shmfile", 65);
	int shm_id = shmget(key, 0, 0666|IPC_CREAT);
	//printf("El id generado es %d\n", shm_id);
	memoria = shmat(shm_id, NULL, 0);
	
	struct AtributosHilo *atributos = (struct AtributosHilo *)atributosHilo;
	struct AtributosHilo atributosCopia = {atributos->nombre, atributos->id, atributos->algoritmo, atributos->lineas, atributos->tiempo, atributos->tipo, atributos->mutex};	
	printf("\nNace hilo %c, necesita %d lineas y va a durar %d segundos\n\n", atributosCopia.nombre, atributosCopia.lineas, atributosCopia.tiempo);	
	//busca si existe la memoria suficiente
	int inicio = 0;
	int contadorLinea = 0;
	int contadorEspacio = 0;
	
	switch(atributosCopia.algoritmo){
		case 1: //First-Fit
			printf("El hilo %c esta bloqueado esperando utilizar la memoria\n", atributosCopia.nombre);
			sem_wait(&atributosCopia.mutex);
			printf("El hilo %c entra a la region critica\n", atributosCopia.nombre);
			for (c = memoria; *c != '\0'; c++){
				if(*c == '0'){
					if(contadorEspacio == 0)
						inicio = contadorLinea;
					contadorEspacio ++;
					if(contadorEspacio >= atributosCopia.lineas){
						//ya encontro suficiente espacio
						c = memoria;
						c+= inicio;
						int inscritos;
						for (inscritos = 0; inscritos < atributosCopia.lineas; inscritos++){
							*c++ = atributosCopia.nombre;
						}
						
						for (c = memoria; *c != '\0'; c++){
							printf("%c", *c);
						}
						printf("\n");
						sem_post(&atributosCopia.mutex);
						printf("El hilo %c se encuentra en ejecucion\n", atributosCopia.nombre);
						goto final;
					}
				}else{
					contadorEspacio = 0;
				}
				contadorLinea++;
			}
			
			//Nunca encontro espacio
			printf("El hilo %c, no encuentra memoria y muere\n", atributosCopia.nombre);
			return NULL;
		case 2: //Best-Fit;
			vaciarLista(l);
			printf("El hilo %c esta bloqueado esperando utilizar la memoria\n", atributosCopia.nombre);
			sem_wait(&atributosCopia.mutex);
			printf("El hilo %c entra a la region critica\n", atributosCopia.nombre);
			for (c = memoria; *c != '\0'; c++){
				if(*c == '0'){
					if(contadorEspacio == 0)
						inicio = contadorLinea;
					contadorEspacio ++;
				}else{
					if(contadorEspacio > 0){
						agregarEspacio(inicio, contadorEspacio, l);
						imprimirEspacios(l);
						contadorEspacio = 0;
					}
				}
				contadorLinea++;
			}
			if(contadorEspacio > 0){
				agregarEspacio(inicio, contadorEspacio, l);
				imprimirEspacios(l);
			}			
			//termina de buscar todos los espacios disponibles
			int bestInicio = getBestFit(l, atributosCopia.lineas);
			if(bestInicio != -1){
				inicio = bestInicio;
				c = memoria;
				c+= bestInicio;
				int inscritos;
				for (inscritos = 0; inscritos < atributosCopia.lineas; inscritos++){
					*c++ = atributosCopia.nombre;
				}
						
				for (c = memoria; *c != '\0'; c++){
					printf("%c", *c);
				}
				printf("\n");
				sem_post(&atributosCopia.mutex);
				printf("El hilo %c se encuentra en ejecucion\n", atributosCopia.nombre);
			}else{
				printf("El hilo %c, no encuentra memoria y muere\n", atributosCopia.nombre);
				return NULL;
			}
			
			break;
		//case 3: //Worst-Fit
		
	}
	
final:
	//ejecuta	
	sleep(atributosCopia.tiempo);
	
	//se desinscribe de la memoria
	int desinscritos;
	c = memoria;
	c+= inicio;
	sem_wait(&atributosCopia.mutex);	
	for (desinscritos = 0; desinscritos < atributosCopia.lineas; desinscritos++){
		*c++ = '0';
	}
	for (c = memoria; *c != '\0'; c++){
		printf("%c", *c);
	}
	sem_post(&atributosCopia.mutex);
	printf("\n");
	
	//muere
	printf("Muere hilo %c\n", atributosCopia.nombre);
	
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
	//char * memoria, *c;
	//key_t key = ftok("shmfile", 65);
	//int shm_id = shmget(key, 0, 0666|IPC_CREAT);
	//printf("El id generado es %d\n", shm_id);
	//memoria = shmat(shm_id, NULL, 0);
	
	int algoritmo;
	int shm_id_AH;
	sem_t mutex;
	
	printf("Ingrese el numero del algoritmo que desea utilizar para la distribucion de memoria\n");
	printf("1. First-Fit\n");
	printf("2. Best-Fit\n");
	printf("3. Worst-Fit\n");
	scanf("%d", &algoritmo);
	
	
	while(algoritmo < 1 || algoritmo > 3){
		printf("Ingrese un numero correspondiente a un algoritmo\n");
		scanf("%d", &algoritmo);
	}
	
	key_t key_AH = ftok("/tmp",'S');
	shm_id_AH = shmget(key_AH, sizeof(struct AtributosHilo ) * MAX_HILOS, 0666|IPC_CREAT);
	//printf("El SEGUNDO id generado es %d\n", shm_id_AH);
	struct AtributosHilo *hilos;
	hilos = shmat(shm_id_AH, NULL, 0);
	
	srand(time(0));
	int cont_ID = 0;// contador de hilos

	while(1){
	
		int lineasHilo = (rand() % 10) + 1;
		int tiempoHilo = (rand() % (60 - 20 + 1)) + 20;
		int tiempoEspera = (rand() % (60 - 30 + 1)) + 30;
		//printf("Lineas: %d\n", lineasHilo);
		//printf("Tiempo: %d\n", tiempoHilo);
		printf("TiempoEspera: %d\n\n", tiempoEspera);	

		char ch = nuevoChar();
		sem_init(&mutex, 0, 1); 
		
		hilos[cont_ID].nombre = ch;
		hilos[cont_ID].id = cont_ID;
		hilos[cont_ID].algoritmo = algoritmo;
		hilos[cont_ID].lineas = lineasHilo;
		hilos[cont_ID].tiempo = tiempoHilo;
		hilos[cont_ID].tipo = activo;
		hilos[cont_ID].mutex = mutex;
	
		pthread_create(&hilos[cont_ID].thread, NULL, vidaHilo, &hilos[cont_ID]);
		/*
		for (c = memoria; *c != '\0'; c++){
			printf("%c", *c);
		}
		printf("\n");*/
		
		//pthread_join(hiloNuevo, NULL);		
		sleep(tiempoEspera);
		cont_ID++;
	}
	
	return 0;
}

