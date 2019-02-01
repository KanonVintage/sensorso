#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */

int main(){
    int i;
    pid_t pids[3];
    for (i=0;i<3;i++){
        pids[i]=fork();
        if (pids[i]==0) { /* child process */
            static char *argv[]={"1","2",NULL};
            sleep(3);
            execv("compiled/sensorSO",argv);
            exit(127); /* only if execv fails */
        }
        printf("[%d] [%d] i=%d\n", getppid(), getpid(), i);
    }
    printf("[%d] [%d] hi\n", getppid(), getpid());
}