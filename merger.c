#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */

#define MAXCHAR 25

int main()
{
    FILE *fp;
    char str[MAXCHAR];
    char filename[MAXCHAR];
    char controllers[MAXCHAR*2];
    char name[10];
    int keyd, keyt;
    char keycd[5], keyct[5];
    char* ptr;
    int cont=0;

    strcpy(filename,"../startup.x");

    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }

    pid_t pids[3];

    while (fgets(str, MAXCHAR, fp) != NULL){
        pid_t pid=fork();
        pids[cont]=pid;
        //printf("%s", str);
        sscanf(str, "%[^:]:%d,%d", name, &keyd, &keyt);
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
        printf("[%d] [%d] i=%d\n", getppid(), getpid(), cont);
        cont++;
    }

    rewind(fp);
    cont=0;

    while (fgets(str, MAXCHAR, fp) != NULL){
        pid_t pid=fork();
        pids[cont]=pid;
        //printf("%s", str);
        sscanf(str, "%[^:]:%d,%d", name, &keyd, &keyt);
        sprintf(keycd,"%d",keyd);
        sprintf(keyct,"%d",keyt);
        //printf("name:%s  keyd:%s  keyt:%s\n", name,keycd,keyct);
        if (pid==0) { /* child process */
            //char *argv[]={"1", name,"3"};
            execl("compiled/sensorSO","argv",name,keycd,keyct,(char *)NULL);
            //execl("/usr/bin/terminator", "terminator", "-e", controllers, NULL);

            exit(127); /* only if execv fails */
        }
        printf("[%d] [%d] i=%d\n", getppid(), getpid(), cont);
        cont++;
    }

    fclose(fp);

    return 0;
}