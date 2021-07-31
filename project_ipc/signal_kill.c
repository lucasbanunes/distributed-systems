#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 3){
        printf("The function works with only two arguments\n");
        printf("%s <pid> <signal>\n", argv[0]);
        return -1;
    }
    
    int SIGNAL; 
    int PID = atoi(argv[1]);
    
    if (kill(PID, 0) == -1){
        printf("The PID %d doesn't exist.\n", PID);
        return -1;
    }

    if (strcmp(argv[2], "SIGUSR1") == 0){
        SIGNAL=SIGUSR1;
    }
    else if (strcmp(argv[2], "SIGUSR2") == 0){
        SIGNAL=SIGUSR2;
    }
    else if (strcmp(argv[2], "SIGTERM") == 0){
        SIGNAL=SIGTERM;
    }
    else if (strcmp(argv[2], "SIGINT") == 0){
        SIGNAL=SIGINT;
    }
    else{
        printf(" %s unsupported signal.\n", argv[2]);
        return -1;
    }
    if (kill(PID, SIGNAL) == -1){
        printf("Can't send signal %s to PID %d.\n", argv[2], PID);
        return -1;
    }
    return 0; 
}