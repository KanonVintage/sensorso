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

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>

void* hola(void* arg){
	char* msg = "Hola";
	int i;
	for (i=0; i<strlen(msg); i++){
		printf("%c", msg[i]) ;
		fflush(stdout) ;
		usleep(1000000) ;
	}
	return NULL;
}

void* mundo(void* arg){
	char* msg = " mundo";
	int i;
	for (i=0; i<strlen(msg); i ++){
		printf("%c", msg[i]);
		fflush(stdout);
		usleep(1000000);
	}
	return NULL;
}

int main(int argc, char* argv[]){
	pthread_t h1;
	pthread_t h2;
	pthread_create(&h1, NULL, hola, NULL);
	pthread_create(&h2, NULL, mundo, NULL);
	pthread_join(h1, NULL);
	pthread_join(h2, NULL);
	printf("Fin\n");
}