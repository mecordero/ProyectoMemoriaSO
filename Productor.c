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

sem_t sem_log;
 

void *vidaHilo(void *atributosHilo)
{
	struct ListaEspacios * l, lis;
	l = &lis;
	l->primero = NULL;
	char * memoria, *c;
	key_t key = ftok("/shmfile", 65);
	int shm_id = shmget(key, 0, 0666|IPC_CREAT);
	memoria = shmat(shm_id, NULL, 0);

	char bitacora[500];
	
	struct AtributosHilo *atributos = (struct AtributosHilo *)atributosHilo;
	printf("Nace hilo %c, necesita %d lineas y va a durar %d segundos\n", atributos->nombre, atributos->lineas, atributos->tiempo);	
	sprintf(bitacora, "Nace hilo %c, necesita %d lineas y va a durar %d segundos\n", atributos->nombre, atributos->lineas, atributos->tiempo);	
	fileLog(bitacora, &sem_log);
	//busca si existe la memoria suficiente
	int inicio = 0;
	int contadorLinea = 0;
	int contadorEspacio = 0;
	FILE *fptr;
	char chEjecucion;
	
	switch(atributos->algoritmo){
		case 1: //First-Fit
			printf("El hilo %c esta bloqueado esperando utilizar la memoria\n", atributos->nombre);
			sprintf(bitacora,"El hilo %c esta bloqueado esperando utilizar la memoria\n", atributos->nombre);
			fileLog(bitacora, &sem_log);

			atributos->tipo = bloqueado;
			sem_wait(atributos->mutex);
			//Antes de pedir la memoria se fija si tiene que seguir
			fptr = fopen("ejecucion.txt","r");
			if(fptr == NULL)
			{
			  printf("Error leyendo archivo!");
			  return 0;
			}
			chEjecucion = getc(fptr);
			if(chEjecucion == '0'){
				printf("Muere el hilo %c debido a finalizacion\n", atributos->nombre);
				sleep(30);
				fclose(fptr);
				return 0;
			}
			fclose(fptr);
			//Puede continuar
			atributos->tipo = activo;
			printf("El hilo %c entra a la region critica\n", atributos->nombre);
			sprintf(bitacora,"El hilo %c entra a la region critica\n", atributos->nombre);
			fileLog(bitacora, &sem_log);

			for (c = memoria; *c != '\0'; c++){
				if(*c == '0'){
					if(contadorEspacio == 0)
						inicio = contadorLinea;
					contadorEspacio ++;
					if(contadorEspacio >= atributos->lineas){
						//ya encontro suficiente espacio
						c = memoria;
						c+= inicio;
						int inscritos;
						for (inscritos = 0; inscritos < atributos->lineas; inscritos++){
							*c++ = atributos->nombre;
						}
						
						
						sem_post(atributos->mutex);
						atributos->tipo = ejecutando;
						printf("El hilo %c se encuentra en ejecucion\n", atributos->nombre);
						sprintf(bitacora,"El hilo %c se encuentra en ejecucion\n", atributos->nombre);
						fileLog(bitacora, &sem_log);
						goto final;
					}
				}else{
					contadorEspacio = 0;
				}
				contadorLinea++;
			}
			
			//Nunca encontro espacio
			atributos->tipo = finalizado;
			sem_post(atributos->mutex);
			printf("El hilo %c, no encuentra memoria y muere\n", atributos->nombre);
			sprintf(bitacora,"El hilo %c, no encuentra memoria y muere\n", atributos->nombre);
			fileLog(bitacora, &sem_log);
			return NULL;
		case 2: //Best-Fit;
		case 3: //Worst-Fit
			vaciarLista(l);
			printf("El hilo %c esta bloqueado esperando utilizar la memoria\n", atributos->nombre);
			sprintf(bitacora,"El hilo %c esta bloqueado esperando utilizar la memoria\n", atributos->nombre);
			fileLog(bitacora, &sem_log);
			atributos->tipo = bloqueado;
			sem_wait(atributos->mutex);
			//Antes de pedir la memoria se fija si tiene que seguir
			fptr = fopen("ejecucion.txt","r");
			if(fptr == NULL)
			{
			  printf("Error leyendo archivo!");
			  return 0;
			}
			chEjecucion = getc(fptr);
			if(chEjecucion == '0'){
				printf("Muere el hilo %c debido a finalizacion\n", atributos->nombre);
				sleep(30);
				fclose(fptr);
				return 0;
			}
			fclose(fptr);
			//Puede continuar
			atributos->tipo = activo;
			printf("El hilo %c entra a la region critica\n", atributos->nombre);
			sprintf(bitacora,"El hilo %c entra a la region critica\n", atributos->nombre);
			fileLog(bitacora, &sem_log);
			for (c = memoria; *c != '\0'; c++){
				if(*c == '0'){
					if(contadorEspacio == 0)
						inicio = contadorLinea;
					contadorEspacio ++;
				}else{
					if(contadorEspacio > 0){
						agregarEspacio(inicio, contadorEspacio, l);
						contadorEspacio = 0;
					}
				}
				contadorLinea++;
			}
			if(contadorEspacio > 0){
				agregarEspacio(inicio, contadorEspacio, l);
			}			
			//termina de buscar todos los espacios disponibles
			if(atributos->algoritmo == 2)
				inicio = getBestFit(l, atributos->lineas);
			else
				inicio = getWorstFit(l, atributos->lineas);
			if(inicio != -1){
				c = memoria;
				c+= inicio;
				int inscritos;
				for (inscritos = 0; inscritos < atributos->lineas; inscritos++){
					*c++ = atributos->nombre;
				}
						
				sem_post(atributos->mutex);
				atributos->tipo = ejecutando;
				printf("El hilo %c se encuentra en ejecucion\n", atributos->nombre);
				sprintf(bitacora,"El hilo %c se encuentra en ejecucion\n", atributos->nombre);
			    fileLog(bitacora, &sem_log);
			}else{
				printf("El hilo %c, no encuentra memoria y muere\n", atributos->nombre);
				sprintf(bitacora,"El hilo %c, no encuentra memoria y muere\n", atributos->nombre);
				fileLog(bitacora, &sem_log);
				atributos->tipo = finalizado;
				sem_post(atributos->mutex);
				return NULL;
			}
					
	}
	
final:
	//ejecuta	
	sleep(atributos->tiempo);
	
	//se desinscribe de la memoria
	int desinscritos;
	c = memoria;
	c+= inicio;
	atributos->tipo = bloqueado;
	sem_wait(atributos->mutex);	
	//Antes de pedir la memoria se fija si tiene que seguir
	fptr = fopen("ejecucion.txt","r");
	if(fptr == NULL)
	{
	  printf("Error leyendo archivo!");
	  return 0;
	}
	chEjecucion = getc(fptr);
	if(chEjecucion == '0'){
		printf("Muere el hilo %c debido a finalizacion\n", atributos->nombre);
		sleep(30);
		fclose(fptr);
		return 0;
	}
	fclose(fptr);
	//Puede continuar
	atributos->tipo = activo;
	for (desinscritos = 0; desinscritos < atributos->lineas; desinscritos++){
		*c++ = '0';
	}
	sem_post(atributos->mutex);
	printf("\n");
	
	//muere
	atributos->tipo = finalizado;
	printf("Muere hilo %c\n", atributos->nombre);
	sprintf(bitacora,"Muere hilo %c\n", atributos->nombre);
	fileLog(bitacora, &sem_log);
	
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
	
	printf("Ingrese el numero del algoritmo que desea utilizar para la distribucion de memoria\n");
	printf("1. First-Fit\n");
	printf("2. Best-Fit\n");
	printf("3. Worst-Fit\n");
	scanf("%d", &algoritmo);
	
	
	while(algoritmo < 1 || algoritmo > 3){
		printf("Ingrese un numero correspondiente a un algoritmo\n");
		scanf("%d", &algoritmo);
	}
	
	key_t key_AH = ftok("/tmp",50);
	shm_id_AH = shmget(key_AH, sizeof(struct AtributosHilo ) * MAX_HILOS, 0666|IPC_CREAT);
	//printf("El SEGUNDO id generado es %d\n", shm_id_AH);
	struct AtributosHilo *hilos;
	hilos = shmat(shm_id_AH, NULL, 0);
	
	srand(time(0));
	int cont_ID = 0;// contador de hilos

	sem_t mutex;
	sem_init(&mutex, 0, 1); 
	sem_init(&sem_log, 0, 1);
	cleanLog(&sem_log);

	while(1){
		//Antes de crear un hilo se fija si tiene que seguir
		FILE *fptr;
		fptr = fopen("ejecucion.txt","r");
		if(fptr == NULL)
		{
		  printf("Error leyendo archivo!");
		  return 1;
		}
		char chEjecucion = getc(fptr);
		if(chEjecucion == '0'){
			printf("Ya no se van a producir mas hilos\n");
			fclose(fptr);
			return 0;
		}
		fclose(fptr);
		
	
		int lineasHilo = (rand() % 10) + 1;
		int tiempoHilo = (rand() % (60 - 20 + 1)) + 20;
		int tiempoEspera = (rand() % (60 - 30 + 1)) + 30;
		printf("Tiempo de Espera para crear otro hilo: %d\n", tiempoEspera);	

		char ch = nuevoChar();
		
		hilos[cont_ID].nombre = ch;
		hilos[cont_ID].id = cont_ID;
		hilos[cont_ID].algoritmo = algoritmo;
		hilos[cont_ID].lineas = lineasHilo;
		hilos[cont_ID].tiempo = tiempoHilo;
		hilos[cont_ID].tipo = activo;
		hilos[cont_ID].mutex = &mutex;  
	
		pthread_create(&hilos[cont_ID].thread, NULL, vidaHilo, &hilos[cont_ID]);
		
		//pthread_join(hiloNuevo, NULL);		
		sleep(tiempoEspera);
		cont_ID++;
	}
	
	return 0;
}

