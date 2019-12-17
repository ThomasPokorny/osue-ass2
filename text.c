#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h> 


int main(int argc, char *argv[]){
    
    int pipefd[2];
    pipe(pipefd);

    int stat; 

    if (fork() == 0)
    {
        // close(pipefd[0]);    // close reading end in the child

        dup2(pipefd[0], STDIN_FILENO);

        dup2(pipefd[1], 1);  // send stdout to the pipe
        dup2(pipefd[1], 2);  // send stderr to the pipe

        close(pipefd[0]);
        close(pipefd[1]);    // this descriptor is no longer needed
        execlp("./intmul", "intmul", (char *)NULL);
        perror("execlp() failed");
    }
    else
    {
        char *data1 = "A\n";
        write(pipefd[1], data1, strlen(data1));
        write(pipefd[1], data1, strlen(data1));
        close(pipefd[1]);

        wait(&stat);
        // parent

        char buffer[1024];

        // close(pipefd[1]);  // close the write end of the pipe in the parent

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
        {
            fputs(buffer, stdout);
        }
    }


    return EXIT_SUCCESS; 
}
