#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "../cabeceras/sala.h"
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

int n;
time_t time_actual;
FILE* fid;
int ret;

void retardo_aleatorio()
{
  unsigned long retardo=rand()%1000000;
  usleep(retardo);
}

int numero_random(){

	srand(clock());
	return rand() % 1000;

}

void estado_sala(){
	
	// Función para printear el estado de la sala (Nº Asientos e IDs)
	
	printf("\n\x1b[32mRESULTADO:\x1b[0m SALA GRÁFICA\n");
	printf("\nNº -> Número de asiento\n");
	printf("ES -> Estado del asiento:\n");
	printf("\t0 -> Asiento libre\n");
	printf("\tID > 0 -> Asiento ocupado por ese ID\n");
	printf("Nº");

	int iter ;
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
void lanza_error(int error, int tipoError){

		// Si tipoError == 0 -> Error de reserva
		// Si tipoError == 1 -> Error de reserva
		// Si tipoError == -1 -> Otro error

		switch(error){
		
			case -2:
			
				if(tipoError == 0){
					
					fprintf(stderr, "\x1b[31mError:\x1b[0m ID ya se encuentra en la sala\n");
				} else if (tipoError == 1){
					
					fprintf(stderr, "\x1b[31mError:\x1b[0m Asiento inválido\n");				
					
				}
				
				break;
		
			case -1:
			
				if(tipoError == 0){
			
					fprintf(stderr, "\x1b[31mError:\x1b[0m Sala llena. Operación imposible hilo\n");
				pthread_exit(NULL);
					
				} else if(tipoError == 1){
					
					fprintf(stderr, "\x1b[31mError:\x1b[0m Asiento ya está libre\n");
				}
				
				break;
				
			case 0:
			
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

void* reservaYAnula(void* nHilo){
	
	int num_hilo = (int) nHilo;
	int id1 = numero_random();
	int id2 = numero_random();
	int id3 = numero_random();

	int as1 = reserva_asiento(id1);
	fprintf(fid, "Reserva\t|\tHILO %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as1, id1);
	fprintf(fid, "-----------------------------------------------------------------\n");
	retardo_aleatorio();
	
	if(as1 < 0){
	
		lanza_error(as1, 0);
	}

	int as2 = reserva_asiento(id2);
	fprintf(fid, "Reserva\t|\tHILO %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as2, id2);
	fprintf(fid, "-----------------------------------------------------------------\n");
	retardo_aleatorio();

	if(as2 < 0){
	
		lanza_error(as2, 0);

	}
	
	int as3 = reserva_asiento(id3);
	fprintf(fid, "Reserva\t|\tHILO %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as3, id3);
	fprintf(fid, "-----------------------------------------------------------------\n");
	retardo_aleatorio();
	
	if(as3 < 0){
	
		lanza_error(as3, 0);
	
	}

	id1 = libera_asiento(as1);
	fprintf(fid, "Libera\t|\tHILO %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as1, id1);
	fprintf(fid, "-----------------------------------------------------------------\n");
	retardo_aleatorio();
	
	id2 = libera_asiento(as2);
	fprintf(fid, "Libera\t|\tHILO %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as2, id2);
	fprintf(fid, "-----------------------------------------------------------------\n");
	retardo_aleatorio();
	
	id3 = libera_asiento(as3);
	fprintf(fid, "Libera\t|\tHILO %d\t|\tAsiento %d\t|\tID %d\t|\n", num_hilo, as3, id3);
	fprintf(fid, "-----------------------------------------------------------------\n");
	retardo_aleatorio();
	
}

void main(int argc, char* argv[]){

	// Comprobamos que se pasen los argumentos correctos
	if(argc < 2 || argc > 2){lanza_error(0, -1);}
	
	n = atoi(argv[1]);
	
	// Ver si es un número entero positivo
	if(n < 1) {
		
		fprintf(stderr, "Número de hilos incorrecto\n");
		exit(-1);
	}
	
	crea_sala(10);
	pthread_t estadoSala;
	pthread_t* hilos = malloc(sizeof(pthread_t)*n);
	
		
	fid = fopen("registro_OP.txt", "w");
	fprintf(fid, "ACCION\t|\tHILO\t|\tASIENTO\t\t|\tID\t|\n");
	fprintf(fid, "-----------------------------------------------------------------\n");

	for(int i = 0; i < n; i++){

		ret = pthread_create(hilos+i, NULL, reservaYAnula, (void*)(i+1));
		if (ret) {
			errno=ret;
			fprintf(stderr,"Error %d al crear el hilo %s\n",errno,strerror(errno));
			exit(-1);
  		}

	}
	
	pthread_create(&estadoSala, NULL, print_estado, NULL);
	
	for(int i = 0; i < n; i++){
		
		ret = pthread_join(hilos[i], NULL);
		if (ret) {
			errno=ret;
			fprintf(stderr,"Error %d al hacer join %s\n",errno,strerror(errno));
			exit(-1);
  		}	
	}

	pthread_cancel(estadoSala);

	fclose(fid);
	
	elimina_sala();
	
	free(hilos);
	
}




