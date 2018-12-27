//The real thing
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

//constants
#define SHMSZ 27 
//variables
key_t* 	keyset;
int* 	shmidset, n;
char**	shmset;
int val[2]={0,0};

void* sensorReader(void* arg){
	char* msg = (char*) arg;
	int i;

	for (i=0; i<strlen(msg); i++){
		printf("%c", msg[i]) ;
		fflush(stdout) ;
		usleep(1000000) ;
	}

	return NULL;
}

void sighand(int signo, siginfo_t *info, void *extra){
	int int_val = info->si_value.sival_int;
    if(int_val==0){
     	//printf("Signal handler installed, waiting for signal\n");
    }else if(int_val<0){
	    printf("Signal: %d, value: [%d]\n", signo, int_val);
	}else{
		if(signo==10) val[0]=int_val;
		else if(signo==12) val[1]=int_val;
		else printf("Ese sensor o accion no existe");
	    //printf("Signal: %d, value: [%d]\n", signo, int_val);
	}
}

void sighandler(int signum) {
   printf("\nCaught signal %d, coming out...\n", signum);
   exit(1);
}

int main(int argc, char* argv[]){
    char **arr;
    char **old;
    int c=0;
    int pid;
    int x=0,y=0, i;

	struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = &sighand;
    if ((sigaction(SIGUSR1, &action, NULL)==-1)||(sigaction(SIGUSR2, &action, NULL)==-1)) {
        perror("sigusr: sigaction");
        return 0;
    }
    signal(SIGINT, sighandler);

    pid=getpid();
    printf("PID: %d\n", pid);
    printf("Cuantos sensores existen: ");
    scanf("%d", &n);
    keyset = (key_t*)malloc(n*sizeof(key_t));
    shmidset = (int*)malloc(n*sizeof(int));
    shmset = (char**)malloc(n*sizeof(char*));
    old = (char**) calloc(n, sizeof(char*));
    arr = (char**) calloc(n, sizeof(char*));
	for (i=0; i<n; i++){
		old[i] = (char*) calloc(SHMSZ, sizeof(char));
	    arr[i] = (char*) calloc(SHMSZ, sizeof(char));
	}

    for(c=0; c<n; c++){
    	printf("Key id del sensor %d: ", c+1);
    	scanf("%d", &keyset[c]);
	    if ((shmidset[c] = shmget(keyset[c], SHMSZ, 0666)) < 0) {
	        perror("shmget");
	        return(1);
	    }
	    if ((shmset[c] = shmat(shmidset[c], NULL, 0)) == (char*) -1) {
	        perror("shmat");
	        return(1);
	    }
    }

    while(1){
    	x=val[0];
    	y=val[1];
    	for(c=0; c<n; c++){
			if (c==0){
				if(strcmp(shmset[c],arr[c])!=0){
					fprintf(stderr,"distancia %s\n",arr[c]);
				}
			}
			if (c==1){
				if ((strcmp(arr[c],"--")!=0)&&(strcmp(old[c],arr[c])!=0)){
					fprintf(stdout,"giroscopio: %s\n",arr[c]);
					strcpy(old[c],arr[c]);
				}
	        }
	        if (c>=2){
	        	printf("\nSensor:%d\tAccion:%d", x, y);
	        }
	        strcpy(arr[c],shmset[c]);
    	}
	}

	free(keyset);
    free(shmidset);
    free(shmset);
    for (i = 0; i < n; i++ ){
	    free(arr[i]);
	    free(old[i]);
	}
	free(old);
	free(arr);

    return(0);
}

/*
//Processes tutorial
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