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

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h> 

#define SHMSZ     27
#define MAX_SAMPLES 100
#define MAX_SAMPLES_THETA 50
#define DIST 10
#define PI 3.14159265
#define MAXCHAR 25
        /* ranf() is uniform in 0..1 */

// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
typedef struct {
    int* keyd;
    int* keyt;
} keys;

float box_muller(float m, float s)	/* normal random variate generator */{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ((double)(rand())/RAND_MAX)- 1.0;
			x2 = 2.0 * ((double)(rand())/RAND_MAX) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

void* sensorSO(void *vargp) {
    keys *data = (keys*)vargp;
	char c;
    int shmidd,shmidt;
    key_t keyd,keyt;
    char *shmd, *shmt;
	int i,j;
	float distances[MAX_SAMPLES];
    float angles[MAX_SAMPLES_THETA];
    float anglesD[MAX_SAMPLES];
	float mu,sigma,delta_theta;

    struct timespec tim, tim2;
    tim.tv_sec = 1;
    tim.tv_nsec = 0;

    printf("no pues");
    printf("oye %d",data->keyd);
    keyd = 1234;
    if ((shmidd = shmget(keyd, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return NULL;
    }    
    if ((shmd = (char*)shmat(shmidd, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return NULL;
    }    
    keyt = 5678;
    if ((shmidt = shmget(keyt, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return NULL;
    }    
    if ((shmt = (char*)shmat(shmidt, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return NULL;
    }    

	mu=0;
	sigma=25;    

	sleep(3);

    for(i=0;i<MAX_SAMPLES_THETA;i++)	  
	{
		angles[i]=box_muller(mu,sigma);
	}
	
	j=-1;
    for(i=0;i<MAX_SAMPLES_THETA-1;i++)	  
	{        
		if (j++<MAX_SAMPLES) anglesD[j]=angles[i];
    	if (j++<MAX_SAMPLES){
			delta_theta=abs(angles[i+1]-angles[i])/2;
			if (angles[i+1]>angles[i]) 
				anglesD[j]= angles[i]+delta_theta;
			else
				anglesD[j]= angles[i]-delta_theta;
		}
	}
   
    for(i=0;i<j;i++){	
		distances[i]=DIST/cos(anglesD[i]/180*PI);		
	}

	for(i=0;i<j;i++){	
	  if(nanosleep(&tim , &tim2) < 0 ) {
  			printf("Nano sleep failed \n");
	        return NULL;
		}	
	   sprintf(shmd,"%f",distances[i]);
	   if (i%2==0){
			sprintf(shmt,"%f",anglesD[i]); 
	   }else{
			strcpy(shmt,"--");
	   }
	}
    free(data);
	return 0;
} 
   
int creator(int keyd, int keyt) { 
    pthread_t thread_id;
    int keya=1234;
    int keyb=5678;
    keys *data = (keys*)malloc(sizeof(keys));
    data->keyd=&keya;
    data->keyt=&keyb;
    printf("Before Thread\n"); 
    pthread_create(&thread_id, NULL, sensorSO, data); 
    pthread_join(thread_id, NULL); 
    printf("After Thread\n");
    free(data);
    return thread_id;
}

int main() {
    for (int i=0; i<1; i++)
        creator(1,2);
    exit(0); 
}