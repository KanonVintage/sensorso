/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Daniel Ochoa Donoso 2010 <dochoa@fiec.espol.edu.ec>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#define SHMSZ     27
#define PI 3.14159265
int shmidd,shmidt,shmidQ,shmidX;
key_t keyd,keyt,keyQ,keyX;
char *shmd, *shmt, *shmQ, *shmX;


int main(int argc, char **argv){
	char tmpd[SHMSZ];
    char oldt[SHMSZ];
    char oldQ[SHMSZ];
    char tmpt[SHMSZ];
    int keya, keyb, keyq, keyx, Q=1, X=0;
    float ang,dis,fin,*dat;
    int flaga,flagb, flagc, cont=0, i;

    printf("Currently: %s\n keyd:%s\t| keyt:%s\n\n",argv[1],argv[2],argv[3]);
    sscanf(argv[2], "%d", &keya);
	sscanf(argv[3], "%d", &keyb);
    sscanf(argv[4], "%d", &keyq);
    sscanf(argv[5], "%d", &Q);
    sscanf(argv[6], "%d", &keyx);

    //printf("keyd: %d \nkeyt: %d \nkeyQ: %d\nQ:%d\nX:%d\n",keya,keyb,keyq,Q,keyx);
    keyd = keya;
    if ((shmidd = shmget(keyd, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return(1);
    }
    if ((shmd = (char *)shmat(shmidd, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }
    //printf("1: %d %d %s\n", keyd, shmidd, shmd);
    keyt = keyb;
    if ((shmidt = shmget(keyt, SHMSZ,  0666)) < 0) {
        perror("shmget");
        return(1);
    }
    if ((shmt = (char *)shmat(shmidt, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }
    //Shared memory for sampling Q
	keyQ = keyq;
    if ((shmidQ = shmget(keyQ, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return(1);
    }    
    if ((shmQ = (char *)shmat(shmidQ, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }
    sprintf(shmQ,"%d",Q);

    //Shared memory for sharing final result
	keyX = keyx;
    if ((shmidX = shmget(keyX, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return(1);
    }    
    if ((shmX = (char *)shmat(shmidX, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }
    sprintf(shmX,"%d",X);
    //printf("%d",keyx);
	//sscanf(shmI, "%d", &I);

    //keys = (key_p*)malloc(cont*sizeof(key_p));
    //dat = (float*)malloc(Q*sizeof(float));
    while(1){
        flaga=0;
        flagb=0;
		strcpy(tmpt,shmt);
        sscanf(shmQ, "%d", &Q);

        //Verificar si Q cambia en el tiempo
        if(strcmp(oldQ,shmQ)!=0){
            //printf("Q changed :(%d)\n", Q);
            free(dat);
            dat = (float*)malloc(Q*sizeof(float));
            cont=0;
            flagc=0;
            strcpy(oldQ,shmQ);
        }

        //Cambiar las banderas para cuando cambian los valores de giroscopio y distancia
		if ((strcmp(tmpt,"--")!=0)&&(strcmp(oldt,tmpt)!=0)){
			//fprintf(stdout,"giroscopio: %s\n",tmpt);
            sscanf(tmpt, "%f", &ang);
			strcpy(oldt,tmpt);
            flaga=1;
            if (strcmp(shmd,tmpd)!=0){
                strcpy(tmpd,shmd);
			    //fprintf(stderr,"distancia %s\n",shmd);
                sscanf(shmd, "%f", &dis);
                flagb=1;
            }
        }
		//strcpy(tmpd,shmd);
        //Si cont = Q-1 entonces termino el arreglo
        if(cont==Q){
            cont=0;
            flagc=1;
        }

        //Si ambas banderas estan activas calcular la distancia real
        if(flaga=1&&flagb==1){
            ang=ang/180*PI;
            fin=dis*cos(ang);
            dat[cont]=fin;
            //printf("\t%d;distancia final: %f\n",cont,dat[cont]);
            cont++;
            if(flagc==1){
                fin=0;
                for(i=0;i<Q;i++){
                    fin=fin+dat[i];
                }
                fin=fin/(Q);
                //printf("\tdistancia final %f\n", fin);
                sprintf(shmX, "%f", fin);
            }
            if(flagc==0){
                fin=0;
                sprintf(shmX, "%f", fin);
            }
        }
	}

    free(dat);    
    return(0);

}
