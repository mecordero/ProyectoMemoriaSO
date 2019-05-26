#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
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

struct Espacio
{
	int inicio;
	int tamano;
	struct Espacio * siguiente;
};

struct ListaEspacios
{
	struct Espacio * primero;
};

void agregarEspacio(int inicio, int tamano, struct ListaEspacios * lista){
	struct Espacio * e = (struct Espacio*)malloc(sizeof(struct Espacio));
	e->inicio = inicio;
	e->tamano = tamano;
	e->siguiente = NULL;
	
	if(lista->primero == NULL)
		lista->primero = e;
	else{
		struct Espacio * actual = lista->primero;
		while(actual->siguiente != NULL){
			actual = actual->siguiente;
		}
		actual->siguiente = e;
	}
}

void imprimirEspacios(struct ListaEspacios * lista){
	struct Espacio * actual = lista->primero;
	
	while(actual != NULL){
		printf("tamano: %d inicio: %d, ", actual->tamano, actual->inicio);
		
		actual = actual->siguiente;
	}
	printf("\n");
}

int getBestFit(struct ListaEspacios * lista, int necesario){
	struct Espacio * actual = lista->primero;
	int inicioEspacio = -1;
	int minSobra = 999999;
	while(actual != NULL){
		if(actual->tamano >= necesario){
			int sobra = actual->tamano - necesario;
			if(sobra < minSobra){
				minSobra = sobra;
				inicioEspacio = actual->inicio;
			}
		}
		actual = actual->siguiente;
	}
	
	return inicioEspacio;
}

void vaciarLista(struct ListaEspacios * lista){
	struct Espacio * actual = lista->primero;
	lista->primero = NULL;
	while(actual != NULL){	
		struct Espacio * tmp = actual;
		actual = actual->siguiente;
		tmp->siguiente = NULL;
	}
}


#endif
