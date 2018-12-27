/*
//Process tutorial
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main (int argc, char* argv[]) {
	pid_t pid;
	pid = fork();

	if (pid == -1) {
		printf (" Fallo en fork \n ");
		return -1;
	} else if (!pid) {
		printf ("Proceso hijo: PID %d \n", getpid());
	} else {
		printf ("Proceso padre: PID %d \n", getpid());
	}
	return 0;
}*/

/*
//Threads tutorial
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void* slowprintf(void* arg){
	char* msg = (char*) arg;
	int i;

	for (i=0; i<strlen(msg); i++){
		printf("%c", msg[i]) ;
		fflush(stdout) ;
		usleep(1000000) ;
	}

	return NULL;
}

int main(int argc, char* argv[]){
	pthread_t h1;
	pthread_t h2;

	char* hola = "Hola";
	char* mundo = " mundo";

	pthread_create(&h1, NULL, slowprintf, (void*)hola);
	pthread_create(&h2, NULL, slowprintf, (void*)mundo);

	pthread_join(h1, NULL);
	pthread_join(h2, NULL);

	printf("\nFin\n");
}*/

/*
//Threads with structs tutorial
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

struct parametros{
	int id;
	float escalar;
	float matriz[3][3];
};

void init(float m[3][3]){
	int i;
	int j;

	for (i=0; i<3; i++){
		for (j=0; j<3; j++){
			m[i][j] = random()*100;
		}
	}
}

void* matrizporescalar(void* arg){
	struct parametros* p = (struct parametros*)arg;
	int i;
	int j;

	for (i=0; i<3; i++){
		printf("Hilo %d multiplicando fila %d \n", p->id, i);
		for (j=0; j<3; j++){
			p->matriz[i][j] = p->matriz[i][j] * p->escalar;
			usleep(100000);
		}
	}
}

int main (int argc, char* argv[]){
	pthread_t h1;
	pthread_t h2;

	struct parametros p1;
	struct parametros p2;

	p1.id = 1;
	p1.escalar = 5.0;
	init(p1.matriz);

	p2.id = 2;
	p2.escalar = 10.0;
	init(p2.matriz);

	pthread_create(&h1, NULL, matrizporescalar, (void*)&p1);
	pthread_create(&h2, NULL, matrizporescalar, (void*)&p2);

	pthread_join(h1, NULL);
	pthread_join(h2, NULL);

	printf("\nFin\n");
}*/

/*
//Forking Mean Time measurement
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
	struct timeval t0,t1;
	int i = 0;
	int id = -1;

	gettimeofday(&t0, NULL);
	
	for (i=0; i<100; i++){
		id = fork();
		if (id==0) 
			return 0;
	}

	if (id!=0){
		gettimeofday(&t1, NULL);
		unsigned int ut1 = t1.tv_sec*1000000 + t1.tv_usec;
		unsigned int ut0 = t0.tv_sec*1000000 + t0.tv_usec;

		//tiempo medio en microsegundos
		printf("%f \n", (ut1-ut0)/100.0);
	}
}*/

/*
//Threading Mean Time measurement
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

struct timeval t0, t1;
double media = 0.0;

void* hilo(void* arg){
	gettimeofday(&t1, NULL);
	unsigned int ut1 = t1.tv_sec*1000000 + t1.tv_usec;
	unsigned int ut0 = t0.tv_sec*1000000 + t0.tv_usec;
	media += (ut1 - ut0);
}

int main(){
	int i = 0;
	pthread_t h;

	for (i=0; i<100; i++){
		gettimeofday(&t0, NULL);
		pthread_create(&h, NULL, hilo, NULL);
		pthread_join(h ,NULL);
	}

	//tiempo en microsegundos
	printf("%f \n", (media/100.0));
}*/