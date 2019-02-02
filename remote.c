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

typedef struct {
    pid_t pid;
    pid_t ppid;
    char name[10];
    char keycd[5], keyct[5], keycI[5], keycQ[5],keycX[5];
    int I, Q, status;
} key_p;

int createSensor(key_p key){
    pid_t pid = key.pid;
    char *name = key.name;
    char *keycd = key.keycd;
    char *keyct = key.keyct;
    char *keycI = key.keycI;
    char *keycQ = key.keycQ;
    char tmp1[5];
    char tmp2[5];
    sprintf(tmp1,"%d",key.I);
    sprintf(tmp2,"%d",key.Q);

    //printf("name:%s  keyd:%s  keyt:%s\n", name,keycd,keyct);
    if (pid==0) { /* child process */
        //char *argv[]={"1", name,"3"};
        execl("run/sensorSO","argv",name,keycd,keyct,keycI,tmp1,(char *)NULL);
        //execl("/usr/bin/terminator", "terminator", "-e", controllers, NULL);
        exit(127); /* only if execv fails */
    }
    printf("ParentId:[%d] \tProcessId:[%d] \n", getppid(), pid);
    return 0;
}

int killSensor(key_p key){
    key.pid=-1;
    key.status=0;
    return 0;
}

int reviveSensor(key_p key){
    pid_t pid2=fork();
    key.pid=pid2;
    key.status=1;
    createSensor(key);
    return 0;
}

int createVisor(key_p *key, int cont, int W, int T){
    pid_t pid=fork();
    char controllers[MAXCHAR*2];
    char tmp[5], tmp2[5], tmp3[5];

    sprintf(tmp,"%d",cont);
    sprintf(tmp2,"%d",W);
    sprintf(tmp3,"%d",T);
    strcpy(controllers,"./run/visor ");
    strcat(controllers, tmp);

    for(int i=0; i<cont; i++){
        strcat(controllers," ");
        strcat(controllers, key[i].keycX);
    }
    strcat(controllers," ");
    strcat(controllers,tmp2);
    strcat(controllers," ");
    strcat(controllers,tmp3);

    if (pid==0) { /* child process */
        //char *argv[]={"1", name,"3"};
        //execl("run/sensorSO","argv",name,keycd,keyct,(char *)NULL);
        execl("/usr/bin/terminator", "terminal", "-e", controllers, NULL);
        exit(127); /* only if execv fails */
    }
    return pid;
}

int main()
{
    FILE *fp;
    char str[MAXCHAR];
    char filename[MAXCHAR];
    char controllers[MAXCHAR*2];
    char name[10];
    int keyd, keyt, keyI, I, keyQ, Q, keyX;
    char keycd[5], keyct[5], keycI[5], keycQ[5], keycX[5];
    char* ptr;
    int cont=0;
    key_t keysI;
    int shmidI, shmidQ, i;
    char *shmI, *shmQ;
    float T=1;
    int W=1;
    key_p *keys;
    pid_t visor;
    int keyW=rand() % 9000 + 1000; //Se lo puede inicializar con un valor fijo ej. 1000
    int keyT=rand() % 9000 + 1000; //Se lo puede inicializar con un valor fijo ej. 9999

    strcpy(filename,"run/startup.x");
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }

    //pid_t pids[3];
    //Contar la cantidad de sensores existentes y crear el data set
    while (fgets(str, MAXCHAR, fp) != NULL){cont++;}
    
    rewind(fp);
    keys = (key_p*)malloc(cont*sizeof(key_p));
    cont=0;

    //Guardar los datos en el data set
    while (fgets(str, MAXCHAR, fp) != NULL){
        pid_t pid=fork();
        //pids[cont]=pid;
        //printf("%s", str);
        sscanf(str, "%[^:]:%d,%d;%d,%d;%d,%d;%d", name, &keyd, &keyt, &keyI, &I, &keyQ, &Q, &keyX);
        sprintf(keycd,"%d",keyd);
        sprintf(keyct,"%d",keyt);
        sprintf(keycI,"%d",keyI);
        sprintf(keycQ,"%d",keyQ);
        sprintf(keycX,"%d",keyX);
        //
        keys[cont].pid=pid;
        strcpy(keys[cont].name,name);
        strcpy(keys[cont].keycd,keycd);
        strcpy(keys[cont].keyct,keyct);
        strcpy(keys[cont].keycI,keycI);
        keys[cont].I=I;
        strcpy(keys[cont].keycQ,keycQ);
        keys[cont].Q=Q;
        strcpy(keys[cont].keycX,keycX);
        //printf("THIS IS KEY X: %s", keycX);
        keys[cont].status=1;
        //printf("name: %s \nkeyd: %s \nkeyt: %s \n keyI:%s \nkeyQ: %s\nI:%d\tQ:%d\n",keys[cont].name,keys[cont].keycd,keys[cont].keyct,keys[cont].keycI,keys[cont].keycQ,I,Q);
        createSensor(keys[cont]);
        cont++;
    }

    fclose(fp);
    sleep(3);

    //Abrir 3 lectores en 3 terminales distintas
    for(i=0; i<cont; i++){
        pid_t pid=fork();
        char tmp1[5];
        sprintf(tmp1,"%d",keys[i].Q);
        strcpy(controllers,"./run/lector ");
        strcat(controllers,keys[i].name);
        strcat(controllers," ");
        strcat(controllers,keys[i].keycd);
        strcat(controllers," ");
        strcat(controllers,keys[i].keyct);
        strcat(controllers," ");
        strcat(controllers,keys[i].keycQ);
        strcat(controllers," ");
        strcat(controllers,tmp1);
        strcat(controllers," ");
        strcat(controllers,keys[i].keycX);
        //printf("THIS IS KEY X: %s", keycX);
        keys[i].ppid=pid;
        if (pid==0) {
            execl("run/lector","argv",keys[i].name,keys[i].keycd,keys[i].keyct,keys[i].keycQ,tmp1,keys[i].keycX,(char *)NULL);
            exit(127);
        }/*
        if (pid==0) {
            execl("/usr/bin/terminator", "terminal", "-e", controllers, NULL);
            exit(127);
        }*/
    }

    visor = createVisor(keys,cont, keyW, keyT);

    while(1) {
        sleep(1);
	    system("clear");
        int n=0, m=0;

        printf("Cual sensor desea manejar \tT:(%f)\t| W:(%d)\n", T, W);
        printf("(Elegir un numero entre 1-%d): ", cont);
        for(i=0; i<cont; i++){
            printf("\n\t%d. %s\t\tI:(%d)\t| Q:(%d)",i+1,keys[i].name,keys[i].I,keys[i].Q);
        }
        printf("\n\t0. Salir");
        printf("\nOpcion: ");
    	scanf("%d",&n);
        n=n-1;

        if(n>=0){
            printf("\n\t1. Cambiar intervalo I");
            printf("\n\t2. Cambiar muestras  Q");
            printf("\n\t3. Cambiar valor de  W");
            printf("\n\t4. Cambiar valor de  T");
            printf("\n\t5. Kill it   ");
            printf("\n\t6. Revive it ");
            printf("\n\t7. Salir ");
            printf("\n\nOpcion: ");
            scanf("%d",&m);

            switch (m){
                case 1:
                    //printf("Sensor %s, keyI: %s\n", keys[0].name, keys[0].keycI);
                    sscanf(keys[n].keycI, "%d", &keyI);
                    if ((shmidI = shmget(keyI, SHMSZ, IPC_CREAT | 0666)) < 0) {
                        perror("shmget");
                        return(1);
                    }
                    if ((shmI = (char *)shmat(shmidI, NULL, 0)) == (char *) -1) {
                        perror("shmat");
                        return(1);
                    }
                    printf("\n(Elegir un numero entre 1-10): ");
                    scanf("%d",&I);
                    if(I<1 || I>10) printf("\nError %d: OPCION INVALIDA\n",I);
                    else{ 
                        keys[n].I=I;
                        printf("\nNUEVO INTERVALO I:(%d)\n",I);
                        sprintf(shmI,"%d",I);
                    }
                    break;
                case 2:
                    sscanf(keys[n].keycQ, "%d", &keyQ);
                    if ((shmidQ = shmget(keyQ, SHMSZ, IPC_CREAT | 0666)) < 0) {
                        perror("shmget");
                        return(1);
                    }    
                    if ((shmQ = (char *)shmat(shmidQ, NULL, 0)) == (char *) -1) {
                        perror("shmat");
                        return(1);
                    }
                    printf("\n(Elegir un numero entre 1-10): ");
                    scanf("%d",&I);
                    if(I<1 || I>10) printf("\nError %d: OPCION INVALIDA\n",I);
                    else{ 
                        keys[n].Q=I;
                        printf("\nNUEVO MUESTREO Q:(%d)\n",I);
                        sprintf(shmQ,"%d",I);
                    }
                    break;
                case 3:
                    if ((shmidQ = shmget(keyW, SHMSZ, IPC_CREAT | 0666)) < 0) {
                        perror("shmget");
                        return(1);
                    }    
                    if ((shmQ = (char *)shmat(shmidQ, NULL, 0)) == (char *) -1) {
                        perror("shmat");
                        return(1);
                    }
                    printf("\n(Elegir un numero entre 1-10): ");
                    scanf("%d",&I);
                    if(I<1 || I>10) printf("\nError %d: OPCION INVALIDA\n",I);
                    else{ 
                        W=I;
                        printf("\nNUEVO VALOR W:(%d)\n",I);
                        sprintf(shmQ,"%d",I);
                    }
                    break;
                case 4:
                    if ((shmidQ = shmget(keyT, SHMSZ, IPC_CREAT | 0666)) < 0) {
                        perror("shmget");
                        return(1);
                    }    
                    if ((shmQ = (char *)shmat(shmidQ, NULL, 0)) == (char *) -1) {
                        perror("shmat");
                        return(1);
                    }
                    printf("\n(Elegir un numero entre 1-10): ");
                    scanf("%d",&I);
                    if(I<1 || I>10) printf("\nError %d: OPCION INVALIDA\n",I);
                    else{ 
                        T=I*0.1;
                        printf("\nNUEVO VALOR T:(%f)\n",T);
                        sprintf(shmQ,"%f",T);
                    }
                    break;
                case 5:
                    killSensor(keys[n]);
                    kill(keys[n].pid, SIGKILL);
                    break;
                case 6:
                    reviveSensor(keys[n]);
                    break;
                case 7:
                    // code to be executed if n is equal to constant1;
                    break;
            }
        }else{
            for(i=0;i<cont;i++){
                kill(keys[n].ppid, SIGKILL);
            }
            kill(visor, SIGKILL);
            kill(getpid(),SIGKILL);
            return 0;
        }
	}
    return 0;
}