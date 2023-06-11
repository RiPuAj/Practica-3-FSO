// Código fuente de la librería sala

#include <stdlib.h> 
#include <stdio.h>
#include "./sala.h"
#include <unistd.h>


int* listaAsientos;
int capacidadMax;
int libres;

void retardo()
{
  unsigned long retardo=rand()%1000000;
  usleep(retardo);
}

int reserva_asiento (int id){
	
	if(libres == 0){	// Miramos si el aforo está completo
		return -1;
	}
	
	if (id < 1){					// Miramos que el id sea valido
		return -1;		// Ya que en el estado se devuelve 0 si está libre
	}
	
	int asientoLibre = -1;
	
	for(int i = 0; i < capacidadMax; i++){
		
		if(asientoLibre < 0){			// Buscamos un asiento libre solo 1 vez
			if(*(listaAsientos+i) == -1){
			
				// Para localizar fallos
				retardo();
				asientoLibre = i;
			}
		}
		
		if(*(listaAsientos+i) == id){		// Miramos si la persona ya tiene un
			return -2;			// asiento asignado
		}
	}
	
	*(listaAsientos+asientoLibre) = id;		// Si no tiene asiento asignado se lo 
	libres--;					// asignamos con el guardado antes
	
	return asientoLibre;
}

int libera_asiento (int asiento){
	
	if(asiento >= capacidadMax || asiento < 0){	// Miramos que el asiento sea válido
		return -1;
	}
	
	
	if(*(listaAsientos+asiento) == -1){		// Miramos si el asiento ya está libre
		return -1;
	}

	int idActual = *(listaAsientos+asiento);	// Liberamos el asiento
	*(listaAsientos+asiento) = -1;
	libres++;
	return idActual;				// Devolvemos id de la persona sentada
}

int estado_asiento(int asiento){
	
	if(asiento >= capacidadMax || asiento < 0){	// Miramos que el asiento sea válido
		return -1;
	}
	
	if(*(listaAsientos+asiento) != -1){		// Miramos si el asiento está ocupado
		return *(listaAsientos+asiento);
	}
	
	return 0;					// Devolvemos 0 si está libre
}

int asientos_libres (){
	return libres;					// Devolvemos los asientos libres
}

int asientos_ocupados (){
	return capacidadMax-libres;		// Devolvemos los asientos ocupados
}

int capacidad (){
	return capacidadMax;				// Devolvemos la capacidad de la sala
}

void crea_sala (int capacidad){
	
	capacidadMax = capacidad;			
	libres = capacidadMax;
	listaAsientos = malloc(capacidadMax*sizeof(int));	// Guardamos espacio para la sala
	
	for(int i = 0; i < capacidadMax; i++){		// Inicializamos toda la sala a -1
		*(listaAsientos+i) = -1;
	}
	
	printf("\n\x1b[34mSISTEMA\x1b[0m: Sala con capacidad %d creada correctamente\n", capacidad);
}

void elimina_sala(){
	free(listaAsientos);	// Liberamos el espacio ocupado por la sala
	capacidadMax = 0;
	libres = 0;
	printf("\x1b[34mSISTEMA\x1b[0m: Sala eliminada correctamente\n");
}

