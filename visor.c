#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#define MAXCHAR 115
#define SHMSZ   27

float calculateSD(float data[], int cont){
    float sum = 0.0, mean, standardDeviation = 0.0;
    int i;

    for(i=0; i<cont; ++i){
        sum += data[i];
    }
    mean = sum/cont;
    for(i=0; i<cont; ++i) standardDeviation += pow(data[i] - mean, 2);

    return sqrt(standardDeviation/cont);
}

int main(int argc, char **argv){ 
    int cont;
    sscanf(argv[1], "%d", &cont);
    int keys[cont];
    float datos[cont], sd;
    int shmidX[cont];
    char *shmX[cont];
    int flags[cont], flag;
    int i=0;
    int keyW, keyT;
    int shmidI, shmidQ;
    char *shmI, *shmQ;

    int W = 1;
    float T = 1;

    for(int i=0;i<cont;i++){
        sscanf(argv[i+2], "%d", &keys[i]);
    }

    sscanf(argv[cont+2], "%d", &keyW);
    sscanf(argv[cont+3], "%d", &keyT);

    if ((shmidI = shmget(keyW, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return(1);
    }    
    if ((shmI = (char *)shmat(shmidI, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }sprintf(shmI,"%d",W);

    if ((shmidQ = shmget(keyT, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return(1);
    }    
    if ((shmQ = (char *)shmat(shmidQ, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }sprintf(shmQ,"%f",T);

    for(int i=0;i<cont;i++){
        if ((shmidX[i] = shmget(keys[i], SHMSZ, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            return(1);
        }
        if ((shmX[i] = (char *)shmat(shmidX[i], NULL, 0)) == (char *) -1) {
            perror("shmat");
            return(1);
        }
    }

    while(1) {
        for(i=0;i<cont;i++){
            sscanf(shmX[i], "%f", &datos[i]);
            if(datos[i]!=0){
                flags[i]=1;
            }if(datos[i]==0){
                flags[i]=0;
            }
            flag=flag+flags[i];
        }

        if(flag==cont){
            sleep(1);
            system("clear");
            sscanf(shmI, "%d", &W);
            sscanf(shmQ, "%f", &T);
            printf("T:(%f)\tW:(%d)\n",T,W);
            sd=calculateSD(datos, cont);
            for(i=0;i<cont;i++){
                if(i==0) printf("Sensor-cen:\t");
                if(i==1) printf("Sensor-izq:\t");
                if(i==2) printf("Sensor-der:\t");
                if(i>2)  printf("Sensor %d:\t", i+1);
                printf("%f \n", datos[i]);
            }
            printf("\tdesviacion estandar: %f\n",sd);
            if((fabs(datos[0]-datos[1])<=T*sd)&&(fabs(datos[0]-datos[2])<=T*sd)){
                printf("Es carro\n");
            }
            if((fabs(datos[0]-datos[1])==W*sd)||(fabs(datos[0]-datos[2])==W*sd)||(fabs(datos[1]-datos[2])==W*sd)){
                printf("Es obstaculo\n");
            }
            printf("\n");
        }else{
            sleep(1);
            system("clear");
            printf("Los datos estan siendo cargados\n");
            for(i=0;i<cont;i++){
                flags[i]=0;
            }
        }
        i=0;
        flag=0;
	}
    return 0;
}