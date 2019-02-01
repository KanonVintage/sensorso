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
    	printf("Key id del sensor %d: ", c);
    	scanf("%d", &keyset[c]);
	    if ((shmidset[c] = shmget(keyset[c], SHMSZ, 0666)) < 0) {
	        perror("shmget");
	        return(1);
	    }
	    if ((shmset[c] = shmat(shmidset[c], NULL, 0)) == (char*) -1) {
	        perror("shmat");
	        return(1);
	    }
	    strcpy(shmset[c],"--");
    }

    while(1){
    	x=val[0];
    	y=val[1];
    	int flag=1;

    	while(flag){
    		for(c=0; c<n; c++){
    			strcpy(arr[c],shmset[c]);
				if(c>0){
					
					if ((strcmp(arr[c],"--")!=0)&&(strcmp(old[c],arr[c])!=0)){
						//fprintf(stdout,"giroscopio: %s\n",arr[c]);
						flag=0;
						strcpy(old[c],arr[c]);
		        	}
		        }
				if(c==0){
					if (strcmp(shmset[c],arr[c])!=0){
						//fprintf(stderr,"distancia %s\n",shmset[c]);
					}
					//strcpy(arr[c],shmset[c]);
				}
				if(x==c){
					if(y==0){
						val[1]=0;
					}
		    		if(y==1){
		    			strcpy(arr[c],"off");
		    		}else if(y>1){
		    			val[1]=0;
		    			strcpy(arr[c],"reboot");
		    		}
		    	}
    		}
    	}

    	if(flag==0){
	    	for(c=0; c<n; c++){
	    		if(c>1) printf("\nSensor:%d\tAccion:%d\t%s", c, y, arr[c]);
	    		if(c==0) printf("\nDistancia:%d\tAccion:%d\t%s", c, y, arr[c]);
	    		if(c==1) printf("\nGiroscopio:%d\tAccion:%d\t%s", c, y, arr[c]);
	    	}
	    	//sleep(3);
    	}
    	printf("\nAHHHHH\n");
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