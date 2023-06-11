#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "../cabeceras/sala.h"
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

int n, m, ret, asiento_default = 0;
int hiloResTer = 0, hiloLibTer = 0;
FILE* fid;
pthread_mutex_t mutexMulthilos = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reserva_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t libera_cond = PTHREAD_COND_INITIALIZER;

void retardo_aleatorio()
{
  unsigned long retardo=rand()%1000000;
  usleep(retardo);
}

int numero_random(){

	return rand() % 1000;

}

int encuentra_asiento(){

	while(asientos_ocupados() == 0){
		continue;
	}
	
	asiento_default = 0;
	int i = numero_random() % (capacidad());
	
	while(estado_asiento(i) == 0){
	
		i = numero_random() % (capacidad());
		
	}
	
	return i;

}

void estado_sala(){
	
	// Función para printear el estado de la sala (Nº Asientos e IDs)
	
	printf("\n\x1b[32mRESULTADO:\x1b[0m SALA GRÁFICA\n");
	printf("\nNº -> Número de asiento\n");
	printf("ES -> Estado del asiento:\n");
	printf("\t0 -> Asiento libre\n");
	printf("\tID > 0 -> Asiento ocupado por ese ID\n");
	printf("Nº");

	int iter;
	for(int i = 0; i < capacidad(); i++){
		printf("|%5.1d", i);
		if ((i+1) % 10 == 0){
			printf("\nID");
		
			for(int j = i-10+1; j<i+1; j++){
				printf("|%5.1d", estado_asiento(j));
			}
			
			if(i+1 < capacidad()){
				printf("\nNº");
				iter = i+1;
			}else{
				printf("\n");
				continue;
			}
			
		}
		
		if(i == capacidad()-1){
			printf("\nID");
			int n = capacidad();
			for(int j = iter; j<n; j++){
				printf("|%5.1d", estado_asiento(j));
			}
			printf("\n");
		}
	}
}
void lanza_error(int error){

		switch(error){
			
			case -3:
				// Error de inanición
				fprintf(stderr, "\x1b[31mError:\x1b[0m Riesgo de inanición.\n");
				elimina_sala();
				exit(-1);
				break;
		
			case -2:
				// Error de ID ya en la sala
				fprintf(stderr, "\x1b[31mError:\x1b[0m Id en la sala ya.\n");
				exit(-1);
				break;
		
			case -1:
				// Error de sala llena
				fprintf(stderr, "\x1b[31mError:\x1b[0m Sala llena. Operación imposible\n");
				exit(-1);
				break;
			
			case 0:
				// Error de sintaxis
				fprintf(stderr, "\x1b[31mError:\x1b[0m Error de sintaxis\n");
				exit(-1);
				break;
		}
}

void* print_estado(){

	for(;;){
		retardo_aleatorio();
		estado_sala();
	}
}


void* reserva(void* nHilo){

	int num_hilo = (int) nHilo;
	int id1 = numero_random();
	int id2 = numero_random();
	int id3 = numero_random();
	
	// Bloqueamos el cerrojo
	pthread_mutex_lock(&mutexMulthilos);
	
	
	while(asientos_libres() == 0){				
	
		if(hiloLibTer == m){
		
			// Cancelamos el hilo si todos los hilos de liberación
			// han terminado
			pthread_mutex_unlock(&mutexMulthilos);
			pthread_exit(NULL);

		}
		
		// Esperamos a que hayan huecos libres para reservar
		pthread_cond_wait(&reserva_cond, &mutexMulthilos);
		
	}
	
	int as1r = reserva_asiento(id1);
	
	while (as1r == -2){
		
		id1 = numero_random(); 
		as1r = reserva_asiento(id1);
	
	}
	
	// Despertamos a un hilo de liberación
	pthread_cond_broadcast(&libera_cond);
	
	// Desbloqueamos el cerrojo
	pthread_mutex_unlock(&mutexMulthilos);
	
	// Escribimos en el fichero de registros
	fprintf(fid, "Reserva\t|\tHILO R %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as1r, id1);
	fprintf(fid, "-------------------------------------------------------------------------\n");
	retardo_aleatorio();

	// --------------------------------------- Segunda reserva ------------------------------------------
	
	// Bloqueamos el cerrojo
	pthread_mutex_lock(&mutexMulthilos);
	
	while(asientos_libres() == 0){				// Miramos si el aforo está completo
		if(hiloLibTer == m){
		
			// Cancelamos el hilo si todos los hilos de liberación
			// han terminado
			pthread_mutex_unlock(&mutexMulthilos);
			pthread_exit(NULL);

		}
		
		// Esperamos a que hayan huecos libres para reservar
		pthread_cond_wait(&reserva_cond, &mutexMulthilos);
		
	}

	int as2r = reserva_asiento(id2);
	while (as2r == -2){
		id2 = numero_random(); 
		as2r = reserva_asiento(id2);
	}
	
	// Despertamos un hilo de liberación
	pthread_cond_broadcast(&libera_cond);
	
	// Desbloqueamos el cerrojo
	pthread_mutex_unlock(&mutexMulthilos);

	// Escribimos en el fichero de registros
	fprintf(fid, "Reserva\t|\tHILO R %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as2r, id2);
	fprintf(fid, "-------------------------------------------------------------------------\n");
	retardo_aleatorio();

	// --------------------------------------- Tercera reserva -------------------------------------------
	
	// Bloqueamos el cerrojo
	pthread_mutex_lock(&mutexMulthilos);
	
	while(asientos_libres() == 0){				// Miramos si el aforo está completo
	
		if(hiloLibTer == m){
			
			// Cancelamos el hilo si todos los hilos de liberación
			// han terminado
			pthread_mutex_unlock(&mutexMulthilos);
			pthread_exit(NULL);	
			
		}

		// Esperamos a que hayan huecos libres para reservar
		pthread_cond_wait(&reserva_cond, &mutexMulthilos);
		
	}
	
	int as3r = reserva_asiento(id3);
	while (as3r == -2){
		
		id3 = numero_random(); 
		as3r = reserva_asiento(id3);
	
	}
	
	// Variable inanición
	hiloResTer++;	
	
	// Despertamos un hilo de liberación
	pthread_cond_broadcast(&libera_cond);
	
	// Desbloqueamos cerrojo
	pthread_mutex_unlock(&mutexMulthilos);

	// Escribimos en el fichero de registros
	fprintf(fid, "Reserva\t|\tHILO R %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as3r, id3);
	fprintf(fid, "-------------------------------------------------------------------------\n");
	retardo_aleatorio();
	

}


void* libera(void* nHilo){

	int num_hilo = (int) nHilo;
	
	sleep(1);
	
	// ------------------------------------- Primera liberación --------------------------------------	
	// Bloqueamos el cerrojo
	pthread_mutex_lock(&mutexMulthilos);
	
	// Comprobamos que haya al menos 1 reserva
	while(asientos_ocupados() == 0){


		if(hiloResTer == n){
			
			// Cancelamos el hilo si todos los hilos de reserva
			// han terminado
			pthread_mutex_unlock(&mutexMulthilos);
			pthread_exit(NULL);	
			
		}
	
		// Esperamos a que hayan huecos libres para reservar
		pthread_cond_wait(&libera_cond, &mutexMulthilos);
	
	}
	
	int as = encuentra_asiento();
	
	// Buscamos algún asiento ocupado
	while(estado_asiento(as) == 0) as = encuentra_asiento();

	int id = libera_asiento(as);
	
	// Despertamos un hilo de liberación
	pthread_cond_broadcast(&reserva_cond);
	
	// Desbloqueamos cerrojo
	pthread_mutex_unlock(&mutexMulthilos);
	
	// Escribimos en el fichero de registros
	fprintf(fid, "Libera\t|\tHILO L %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as, id);
	fprintf(fid, "-------------------------------------------------------------------------\n");
	retardo_aleatorio();
	

	
	// ----------------------------------------- Segunda liberación -----------------------------
	
	// Bloqueamos el cerrojo
	pthread_mutex_lock(&mutexMulthilos);
	
	// Comprobamos que haya al menos 1 reserva
	while(asientos_ocupados() == 0){


		if(hiloResTer == n){
			
			// Cancelamos el hilo si todos los hilos de reserva
			// han terminado
			pthread_mutex_unlock(&mutexMulthilos);
			pthread_exit(NULL);	
			
		}
	
		// Esperamos a que hayan huecos libres para reservar
		pthread_cond_wait(&libera_cond, &mutexMulthilos);
	
	}
	
	as = encuentra_asiento();
	
	sleep(1);
	while(estado_asiento(as) == 0) as = encuentra_asiento();
	
	id = libera_asiento(as);
	
	// Despertamos un hilo de liberación
	pthread_cond_broadcast(&reserva_cond);
	
	// Desbloqueamos cerrojo
	pthread_mutex_unlock(&mutexMulthilos);
	
	// Escribimos en el fichero de registros
	fprintf(fid, "Libera\t|\tHILO L %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as, id);
	fprintf(fid, "-------------------------------------------------------------------------\n");
	retardo_aleatorio();
	
	// ------------------------------- Tercera liberación ---------------------------------------- 
	
	// Bloqueamos el cerrojo
	pthread_mutex_lock(&mutexMulthilos);
	
	// Comprobamos que haya al menos 1 reserva
	while(asientos_ocupados() == 0){


		if(hiloResTer == n){
			
			// Cancelamos el hilo si todos los hilos de reserva
			// han terminado
			pthread_mutex_unlock(&mutexMulthilos);
			pthread_exit(NULL);	
			
		}
	
		// Esperamos a que hayan huecos libres para reservar
		pthread_cond_wait(&libera_cond, &mutexMulthilos);
	
	}
	
	
	as = encuentra_asiento();
	
	sleep(1);
	
	// Buscamos un asiento ocupado
	while(estado_asiento(as) == 0) as = encuentra_asiento();
	
	// Liberamos
	id = libera_asiento(as);
	
	// Variable de inanición
	hiloLibTer++;
	
	// Despertamos un hilo de liberación
	pthread_cond_broadcast(&reserva_cond);
	
	// Desbloqueamos cerrojo
	pthread_mutex_unlock(&mutexMulthilos);
	
	// Escribimos en el archivo de registros
	fprintf(fid, "Libera\t|\tHILO L %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as, id);
	fprintf(fid, "-------------------------------------------------------------------------\n");
	
	retardo_aleatorio();
}

void main(int argc, char* argv[]){

	// Inicializamos la semilla para los números aleatorios
	srand(getpid());
	
	// Lanzamos error si el número de argumentos es equivocado
	if(argc < 3 || argc > 3){
	
		lanza_error(0);

	}

	// Convertimos n a variable de tipo int
	n = atoi(argv[1]);
	
	// Convertimos m a variable de tipo int
	m = atoi(argv[2]);
	
	// Comprobar si n es un número entero positivo
	if(n < 1) {
		
		fprintf(stderr, "\x1b[31mError:\x1b[0m Número de hilos incorrecto\n");
		exit(-1);
	}
	
	// Comprobar si m es un número entero positivo
	if(m < 1) {
		
		fprintf(stderr, "\x1b[31mError:\x1b[0m Número de hilos incorrecto\n");
		exit(-1);
	}
	
	// Creamos una sala
	crea_sala(10);

	// Inicializamos la el hilo de printeo
	pthread_t estadoSala;
	
	// Reservamos espacio de memoria para los hilos
	pthread_t* hilos_reservas = malloc(sizeof(pthread_t)*n);
	pthread_t* hilos_liberacion = malloc(sizeof(pthread_t)*m);
		
	fid = fopen("registro_OP.txt", "w");
	fprintf(fid, "ACCION\t|\tHILO\t\t|\tASIENTO\t\t|\tID\t|\n");
	fprintf(fid, "-------------------------------------------------------------------------\n");

	pthread_create(&estadoSala, NULL, print_estado, NULL);
	
	// Variables que nos indicas cuantos hilos se han lanzado
	int resLanzadas = 0, libLanzadas = 0;
	
	// Lanzar hilos aleatoriamente
	
	for (int i = 0; i < (m+n); i++){
		
		// Si randNum = 0 -> Lanzamos hilo de reserva
		// Si randNum != 0 -> Lanzamos hilo de liberación
		int randNum = rand()%2;

		
		if(randNum % 2 == 0 && resLanzadas < n){
			
			// Lanzamos los hilos de reserva de reserva
			
			ret = pthread_create(hilos_reservas+resLanzadas, NULL, reserva, (void*)(i+1));
			
			if (ret) {
				errno=ret;
				fprintf(stderr,"Error %d al crear el hilo %s\n",errno,strerror(errno));
				exit(-1);
  			}
			
			// Incrementamos resLanzadas
  			resLanzadas++;
		
		} else if(libLanzadas < m){
			
			// Lanzamos de liberación
			ret = pthread_create(hilos_liberacion+libLanzadas, NULL, libera, (void*)(i+1));
			if (ret) {
				errno=ret;
				fprintf(stderr,"Error %d al crear el hilo %s\n",errno,strerror(errno));
				exit(-1);
	  		}

			// Incrementamos libLanzadas
			libLanzadas++;

		} else {
			
			// Lanzamos de reserva, ya que libLanzadas > m
			ret = pthread_create(hilos_reservas+resLanzadas, NULL, reserva, (void*)(i+1));
			
			if (ret) {
				errno=ret;
				fprintf(stderr,"Error %d al crear el hilo %s\n",errno,strerror(errno));
				exit(-1);
  			}

			// Incrementamos resLanzadas
  			resLanzadas++;			
		}
	}
	
	
	for (int i = 0; i < (m+n); i++){
	
		if(i < n){
			
			// Join para los hilos de reserva cuando -> i < n
			ret = pthread_join(*(hilos_reservas+i), NULL);
			
			if (ret) {
				errno=ret;
				fprintf(stderr,"Error %d al hacer join %s\n",errno,strerror(errno));
				exit(-1);
  			}
		
		} else {
		
			// Join para los hilos de liberación cuando -> n <= i
			ret = pthread_join(*(hilos_liberacion+(i-n)), NULL);
			
			if (ret) {
				errno=ret;
				fprintf(stderr,"Error %d al hacer join %s\n",errno,strerror(errno));
				exit(-1);
  			}
		
		}
	}
	
	sleep(1);
	
	// Cancelamos el hilo del estado de la sala
	pthread_cancel(estadoSala);
	
	// Cerramos el fichero de registro
	fclose(fid);

	// Eliminamos la sala creada
	elimina_sala();
	
	// Liberamos los hilos de liberación
	free(hilos_reservas);
	
	// Liberamos el espacio de los hilos de liberación
	free(hilos_liberacion);
	
	// Destruimos el cerrojo
	pthread_mutex_destroy(&mutexMulthilos);
	
	// Destruimos las variables condiciones
	pthread_cond_destroy(&reserva_cond);
	pthread_cond_destroy(&libera_cond);
	
}




