/********************************************//**
 * @mainpage  INTMUL Implement an algorithm for the efficient multiplication of large integers. 
 * 
 * @file intmul.c
 * @author Thomas Robert Pokorny 1527212
 * 
 * @date 17 Dez 2019
 * 
 * @brief From the assignemnt: "The program takes two hexadecimal integers A and B with an equal number of digits as input, multiplies them and prints the result."
 * 
 * @details From the assignemnt: "The program takes two hexadecimal integers A and B with an equal number of digits as input, multiplies them and prints the result. The input is read from stdin and consists of two lines:
 *      the first line is the integer A and the second line is the integer B.
 *      Your program must accept any number of digits. Terminate the program with exit status EXIT_FAILURE if an invalid input is encountered or if the two integers do not have equal length.""
 * 
 * 
 * Calling synopsis:
 *  intmul
 * 
 ***********************************************/
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
#include "intmul.h"


#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }
const bool DEBUG = false;

/* FUNCTIONS USED BY THE APPLICATION */
static void debugLog(char *m, char* obj);
static void parsrgs(intmul_conf *conf);
static void mult(intmul_conf conf);

int main(int argc, char *argv[]){
    intmul_conf conf;

    /* parsing args from stdin */
    parsrgs(&conf);

    if(DEBUG == true){
        debugLog("1st value", conf.hex_1);
        debugLog("2nd value", conf.hex_2);
    }

    // mult(strtol(conf.hex_1, NULL, 0), strtol(conf.hex_2, NULL, 0));
    mult(conf);

    exit(EXIT_SUCCESS); 
}

/** IMPLEMENTATION OF FUNCTIONS **/

/**
 * @brief sets the two hex numbers from stdin into the conf struct
 * @param conf the config struct 
 */ 
static void parsrgs(intmul_conf *conf){

    char *line = NULL;
    size_t size; 

    for(int i = 0; i < 2 ; i++){
        if (getline(&line, &size, stdin) != -1) {

            if(i == 0){
                conf->hex_1 = malloc(strlen(line));
                strcpy (conf->hex_1, line);
            }else{
                conf->hex_2 = malloc(strlen(line));
                strcpy (conf->hex_2, line);
            }
        }
        else{
            ERROR_EXIT("not enough lines given %s\n", strerror(errno));
        }
    }
    
    free(line);

    //NOTE: check lenght of lines, if the lines do not have equal lenght the program exists with an error
    if(strlen(conf->hex_1) !=  strlen(conf->hex_2))
        ERROR_EXIT("integers do not have equal lenght! %s\n", strerror(errno));
}

/*
* TODO: comment
*/
static void mult(intmul_conf conf){

    // NOTE: we are only checking 
    
    if(strlen(conf.hex_1) == 2 ){
        int d1 = (int) strtol(conf.hex_1, NULL, 16);
        int d2 = (int) strtol(conf.hex_2, NULL, 16);

        int pord = d1 * d2;
        fprintf(stdout, "%d", pord);
        exit(EXIT_SUCCESS); 
    }
    // NOTE: the main fork and recursive logik
    else{

        // fork shit
        pid_t pid = fork();
        int stat; 

        // pipe fd
        int pipefd[2];
        pipe(pipefd);

        switch (pid)
        {
        case -1:
            ERROR_EXIT("Cannot fork process %s\n", strerror(errno));
            break;
        case 0:
            // child process task
            close(pipefd[0]);    // close reading end in the child

            dup2(pipefd[1], 1);  // send stdout to the pipe
            dup2(pipefd[1], 2);  // send stderr to the pipe

            close(pipefd[1]);   
            
            execlp("ls", "ls", (char *)NULL);

            //exit(EXIT_SUCCESS); 
            break;
        default:
            printf("waiting \n");
            // parent task
            wait(&stat);
            printf("waiting finished \n");
            if (1 == 1) {
                 char buffer[1024];

                close(pipefd[1]);  // close the write end of the pipe in the parent

                while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
                {
                    printf("from child: %s", buffer);
                }
            } else
                ERROR_EXIT("error in process %s\n", strerror(errno));

            break;
        }
    }
}

/**
 * @brief logs a debug message to stdout
 * @param m the message titel
 * @param obj the message
 */ 
static void debugLog(char *m, char* obj){
    printf("DEBUG: '%s': ", m);
    printf("%s\n", obj);
}
