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
int c=0;

struct timeval t0, t1;
double media = 0.0;

void* hilo(void* arg){
	int data = (int) arg;
	gettimeofday(&t1, NULL);
	unsigned int ut1 = t1.tv_sec*1000000 + t1.tv_usec;
	unsigned int ut0 = t0.tv_sec*1000000 + t0.tv_usec;
	media += (ut1-ut0);

	if ((shmidset[c] = shmget(keyset[c], SHMSZ, 0666)) < 0) {
        perror("shmget");
        return NULL;
    }
    if ((shmset[c] = shmat(shmidset[c], NULL, 0)) == (char*) -1) {
        perror("shmat");
        return NULL;
    }
    strcpy(shmset[c],"--");
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


int main(){
	pthread_t *h;
	char **arr;
    char **old;
    int pid;
    int x=0,y=0, i=0;

	pid=getpid();
    printf("PID: %d\n", pid);
    printf("Cuantos sensores existen: ");
    scanf("%d", &n);
    h=(pthread_t*)malloc(n*sizeof(pthread_t));
    keyset = (key_t*)malloc(n*sizeof(key_t));
    shmidset = (int*)malloc(n*sizeof(int));
    shmset = (char**)malloc(n*sizeof(char*));
    old = (char**) calloc(n, sizeof(char*));
    arr = (char**) calloc(n, sizeof(char*));
	for (i=0; i<n; i++){
		old[i] = (char*) calloc(SHMSZ, sizeof(char));
	    arr[i] = (char*) calloc(SHMSZ, sizeof(char));
	}

	for (i=0; i<n; i++){
		c=i;
		printf("Key id del sensor %d: ", c);
    	scanf("%d", &keyset[c]);
		gettimeofday(&t0, NULL);
		pthread_create(&h[i], NULL, hilo, (void *)i);
	}
	for(i=0;i<n;i++){
	    pthread_join(h[i],NULL);
	    //printf("%d",i);
	}

	//tiempo en microsegundos
	printf("%f \n", (media/100.0));
}