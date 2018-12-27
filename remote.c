#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

// driver code 
void main(){ 
    int pid; 

    printf("Cual es el PID del proceso: ");
	scanf("%d", &pid);
	union sigval value;
	value.sival_int = 0;
	if(sigqueue(pid, SIGUSR2, value) == 0) {
        printf("Conectado satisfactoriamente\n");
    } else {
        perror("SIGSENT-ERROR:");
        exit(1);
    }

    while(1) {
    	value.sival_int = 0;

    	printf("Cual sensor desea manejar: ");
    	scanf("%d",&value.sival_int);

    	if(sigqueue(pid, SIGUSR1, value) == 0) {
	        //printf("Se ha enviado la señal del sensor!!\n");
	    } else {
	        perror("SIGSENT-ERROR:");
	    }

    	printf("Que accion desea realizar: 1. Iniciar\t2.Terminar\t3.Reiniciar\n");
    	scanf("%d",&value.sival_int);

		if(sigqueue(pid, SIGUSR2, value) == 0) {
	        printf("Se ha enviado la señal del sensor!!\n");
	    } else {
	        perror("SIGSENT-ERROR:");
	    }

	    /*printf("\nPARENT: sending SIGQUIT\n\n"); 
	    kill(pid, SIGQUIT);*/ 
	    sleep(3);
	    system("clear");
	}
} 