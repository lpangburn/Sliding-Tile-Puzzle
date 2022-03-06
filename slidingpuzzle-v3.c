#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sp-pipe-client.h"
#include "sp-pipe-server.h"
#include "globals.h"

int main(){

    if(pipe(client) == -1 || pipe(server) == -1){//pipe failed
        fprintf(stderr, "Pipe creation failed. Exiting.\n");
        exit(0);
    }

    clientID = fork();

    if(clientID < 0){ //fork failed
        fprintf(stderr, "Fork failed.\n");
        return 1;
    }


    if (clientID == 0){//child process

       clientProcess();

    }else{

        serverProcess();
        wait(NULL);

    }
}