/*# include <sys/types.h>
# include <unistd.h>
# include <stdio.h>

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
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>

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


# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>

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
}