#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXCHAR 25
#define SHMSZ   27

int main()
{
    FILE *fp;
    char str[MAXCHAR];
    char filename[MAXCHAR];
    char controllers[MAXCHAR*2];
    char name[10];
    int keyd, keyt, keyI, I;
    char keycd[5], keyct[5], keycI[5];
    char* ptr;
    int cont=0;
    key_t keysI;
    int shmidI;
    char *shmI;


    strcpy(filename,"compiled/startup.x");

    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }

    //pid_t pids[3];

    while (fgets(str, MAXCHAR, fp) != NULL){
        pid_t pid=fork();
        //pids[cont]=pid;
        //printf("%s", str);
        sscanf(str, "%[^:]:%d,%d|%d", name, &keyd, &keyt, &keyI);
        sprintf(keycd,"%d",keyd);
        sprintf(keyct,"%d",keyt);
        sprintf(keycI,"%d",keyI);
        //printf("name:%s  keyd:%s  keyt:%s\n", name,keycd,keyct);
        if (pid==0) { /* child process */
            //char *argv[]={"1", name,"3"};
            execl("compiled/sensorSO","argv",name,keycd,keyct,keycI,(char *)NULL);
            //execl("/usr/bin/terminator", "terminator", "-e", controllers, NULL);
            exit(127); /* only if execv fails */
        }
        printf("[%d] [%d] i=%d\n", getppid(), pid, cont);
        cont++;
    }

    sleep(3);
    rewind(fp);
    cont=0;

    while (fgets(str, MAXCHAR, fp) != NULL){
        pid_t pid=fork();
        //pids[cont]=pid;
        //printf("%s", str);
        sscanf(str, "%[^:]:%d,%d|%d", name, &keyd, &keyt, &keyI);
        sprintf(keycd,"%d",keyd);
        sprintf(keyct,"%d",keyt);
        strcpy(controllers,"./compiled/lector ");
        strcat(controllers,name);
        strcat(controllers," ");
        strcat(controllers,keycd);
        strcat(controllers," ");
        strcat(controllers,keyct);
        //sleep(3);
        //printf("%s",controllers);
        //printf("name:%s  keyd:%s  keyt:%s\n", name,keycd,keyct);
        if (pid==0) { /* child process */
            //char *argv[]={"1", name,"3"};
            //execl("compiled/sensorSO","argv",name,keycd,keyct,(char *)NULL);
            execl("/usr/bin/terminator", "terminal", "-e", controllers, NULL);
            exit(127); /* only if execv fails */
        }
        //printf("[%d] [%d] i=%d\n", getppid(), pid, cont);
        cont++;
    }

    fclose(fp);

    //Shared memory for time frecuency
    keysI = keyI;
    if ((shmidI = shmget(keysI, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return(1);
    }    
    if ((shmI = (char *)shmat(shmidI, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }

    while(1) {
        sleep(1);
	    system("clear");

        fprintf(stdout,"Intervalo actual I:(%s)\n",shmI);
    	printf("Cual intervalo desea manejar\n(Elegir un numero entre 1-10): ");
    	scanf("%d",&I);

        if(I<1 || I>10) printf("\nError %d: OPCION INVALIDA\n",I);
        else{ 
            printf("\nNUEVO INTERVALO I:(%d)\n",I);
            sprintf(shmI,"%d",I);
        }
	}

    return 0;
}