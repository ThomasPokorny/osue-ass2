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
 * 
 * @todo FREE ALL String !!
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
#include <math.h>
#include "intmul.h"


#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }
const bool DEBUG = false;

/* FUNCTIONS USED BY THE APPLICATION */
static void debugLog(char *m, char* obj);
static void parsrgs(intmul_conf *conf);
static void mult(intmul_conf conf);
static void multRec(intmul_conf conf);

static char* strsplit_one(char *str);
static char* strsplit_two(char *str);

// 
static void slave(int pipe[]);
static void master(master_conf salves, int pipe1[2], int pipe2[2], int pipe3[2],int pipe4[2]);

//
static void longmulti(const char *a, const char *b, char *c);
static char* add(const char *a, const char *b, char *ans);
static char int2hex(int i);
static int hex2int(char ch);
static char* powerh16(int n);
static char *strrev(char *str);

int main(int argc, char *argv[]){
    intmul_conf conf;

    /* parsing args from stdin */
    parsrgs(&conf);

    if(DEBUG == true){
        debugLog("1st value", conf.hex_2);
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
        fprintf(stdout, "%x\n", pord);
        exit(EXIT_SUCCESS); 
    }
    // NOTE: the main fork and recursive logik
    else{
        multRec(conf);
    }
}

/**
 * @brief ecursively execute this program in four child processes one for each computation
 * @param conf contains the integers
 */ 
static void multRec(intmul_conf conf){
    // the two halfs
    char *ah = strsplit_one(conf.hex_1);
    char *al = strsplit_two(conf.hex_1);
    char *bh = strsplit_one(conf.hex_2);
    char *bl = strsplit_two(conf.hex_2);

    //ah = "asd";
    if(DEBUG == true){
        debugLog("ah", ah);
        debugLog("al", al);
        debugLog("bh", bh);
        debugLog("bl", bl);
    }

    int pid1, pid2, pid3, pid4;
    // the needed pipes
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2];
    pipe(pipe1);
    pipe(pipe2);
    pipe(pipe3);
    pipe(pipe4);

    pid1 = fork();
    if(pid1 == -1)
        ERROR_EXIT("could not fork  %s\n", strerror(errno));
    if(pid1 == 0){
        slave(pipe1);
    }
    else{
        pid2 = fork();
        if(pid2 == -1)
            ERROR_EXIT("could not fork  %s\n", strerror(errno));
        if(pid2 == 0){
            slave(pipe2);
        }
        else{
            pid3 = fork();
            if(pid3 == -1)
                ERROR_EXIT("could not fork  %s\n", strerror(errno));
            if(pid3 == 0){
                slave(pipe3);
            }
            else{
                pid4 = fork();
                if(pid4 == -1)
                    ERROR_EXIT("could not fork  %s\n", strerror(errno));
                if(pid4 == 0){
                    slave(pipe4);
                }
                else{
                    master_conf slaves;
                    slaves.pid1 = pid1;
                    slaves.pid2 = pid2;
                    slaves.pid3 = pid3;
                    slaves.pid4 = pid4;

                    slaves.ah = ah;
                    slaves.al = al;
                    slaves.bh = bh;
                    slaves.bl = bl;

                    slaves.n = (strlen(conf.hex_1) -1) ;

                    master(slaves, pipe1, pipe2, pipe3, pipe4);
                }
            }
        }
    }

    //
    free(ah);
    free(al);
    free(bh);
    free(bl);
}

/**
 * @brief executes the children
 */ 
static void slave(int pipe[]){

    dup2(pipe[0], STDIN_FILENO);
    dup2(pipe[1], 1);  // send stdout to the pipe

    close(pipe[0]);
    close(pipe[1]);    // this descriptor is no longer needed
    execlp("./intmul", "intmul", (char *)NULL);
    perror("execlp() failed");
}

/**
 * @brief executes the children
 */ 
static void master(master_conf salves, int pipe1[2], int pipe2[2], int pipe3[2],int pipe4[2]){
    pid_t w;
    
    // child 1
    write(pipe1[1], salves.ah, strlen(salves.ah));
    write(pipe1[1], salves.bh, strlen(salves.bh));
    
    // child 2
    write(pipe2[1], salves.ah, strlen(salves.ah));
    write(pipe2[1], salves.bl, strlen(salves.bl));
    
    // child 3
    write(pipe3[1], salves.al, strlen(salves.al));
    write(pipe3[1], salves.bh, strlen(salves.bh));

    // child 4
    write(pipe4[1], salves.al, strlen(salves.al));
    write(pipe4[1], salves.bl, strlen(salves.bl));
   
    close(pipe1[1]);
    close(pipe2[1]);
    close(pipe3[1]);
    close(pipe4[1]);

    //rec data
    /*char *ret1 = calloc((2024), sizeof(char));
    char *ret2 = calloc((2024), sizeof(char));
    char *ret3 = calloc((2024), sizeof(char));
    char *ret4 = calloc((2024), sizeof(char));*/
    char ret1[5012];
    char ret2[5012];
    char ret3[5012];
    char ret4[5012];

    int status;

    // child 1
    w = waitpid(salves.pid1, &status, WUNTRACED | WCONTINUED);
    if (w == -1) {
        perror("waitpid");
        ERROR_EXIT("process finished with wrong exit code %s\n", strerror(errno));
    }

    if(read(pipe1[0], ret1, sizeof(ret1)) == 0){
        ERROR_EXIT("no received data %s\n", strerror(errno));
    }
    close(pipe1[0]);
    for(int i = 0; i < strlen(ret1); i++) {
        if(ret1[i] == '\n'){
            ret1[i] = 0;
            break;
        }
    }

    // child 2
    w = waitpid(salves.pid2, &status, WUNTRACED | WCONTINUED);
    if (w == -1) {
        perror("waitpid");
        ERROR_EXIT("process finished with wrong exit code %s\n", strerror(errno));
    }

    if(read(pipe2[0], ret2, sizeof(ret2)) == 0){
        ERROR_EXIT("no received data %s\n", strerror(errno));
    }
    close(pipe2[0]);
    for(int i = 0; i < strlen(ret2); i++) {
        if(ret2[i] == '\n'){
            ret2[i] = 0;
            break;
        }
    }

    // child 3
    w = waitpid(salves.pid3, &status, WUNTRACED | WCONTINUED);
    if (w == -1) {
        perror("waitpid");
        ERROR_EXIT("process finished with wrong exit code %s\n", strerror(errno));
    }

    if(read(pipe3[0], ret3, sizeof(ret3)) == 0){
        ERROR_EXIT("no received data %s\n", strerror(errno));
    }
    close(pipe3[0]);
    for(int i = 0; i < strlen(ret3); i++) {
        if(ret3[i] == '\n'){
            ret3[i] = 0;
            break;
        }
    }

    // child 4
    w = waitpid(salves.pid4, &status, WUNTRACED | WCONTINUED);
    if (w == -1) {
        perror("waitpid");
        ERROR_EXIT("process finished with wrong exit code %s\n", strerror(errno));
    }

    if(read(pipe4[0], ret4, sizeof(ret3)) == 0){
        ERROR_EXIT("no received data %s\n", strerror(errno));
    }
    close(pipe4[0]);
    for(int i = 0; i < strlen(ret4); i++) {
        if(ret4[i] == '\n'){
            ret4[i] = 0;
            break;
        }
    }

    uint64_t l1,l2,l3,l4;

    l1 = strtol(ret1, NULL, 16);
    l2 = strtol(ret2, NULL, 16);
    l3 = strtol(ret3, NULL, 16);
    l4 = strtol(ret4, NULL, 16);

    // NOTE: DEPRECATED A · B = Ah · Bh · 16n + Ah · Bl · 16n/2 + Al · Bh · 16n/2 + Al · Bl , this is how the result is computated
    // uint64_t result = l1 * powl(16, salves.n) + l2 * powl(16, (salves.n /2)) + l3 * powl(16, (salves.n /2)) + l4;
    // fprintf(stdout, "%llx\n", result);

    // NOTE: the new computation logik without the usage of long multiplikations
    char *multiplication1 = malloc(1024);
    char *multiplication2 = malloc(1024);
    char *multiplication3 = malloc(1024);
    char *zw = malloc(1024);
    char *ans = malloc(1024);

    longmulti(ret1, powerh16(salves.n), multiplication1);
    longmulti(ret2, powerh16((salves.n /2)), multiplication2);
    longmulti(ret3, powerh16((salves.n /2)), multiplication3);

    ans = add(multiplication1, multiplication2, zw);
    ans = add(ans, multiplication3, zw);
    ans = add(ans, ret4, zw); 
    fprintf(stdout, "%s\n", ans);
}

/**
 * @brief splits a string into two halfs and sets the first half into h1 and the second half into h2, , terminate the program with exit status EXIT_FAILURE if the number of digits is not even.
 * @param h1 the first half of the string
 * @param h2 the second half of the string
 * @param str the given string that is going to be split 
 */
static char* strsplit_one(char *str){
    int len = (strlen(str) -1);
    if((len) % 2 != 0)
        ERROR_EXIT("the given string is not even in lenght %s\n", strerror(errno));

    int len1 = len/2;

    char *ptr;
    ptr = calloc((len1 +1), sizeof(char));
    for(int i = 0; i < len1; i++){ ptr[i] = str[i]; }
    ptr[len1] = '\n';
    return ptr;
}

/**
 * @todo write comments
 * 
 */ 
static char* strsplit_two(char *str){
    int len = (strlen(str) -1);
    if((len) % 2 != 0)
        ERROR_EXIT("the given string is not even in lenght %s\n", strerror(errno));

    int len1 = len/2;

    char *ptr;
    ptr = calloc((len1 +1), sizeof(char));

    int running = len1;
    for(int i = 0; running < len; i++)
    { 
        ptr[i] = str[running];
        running ++; 
    }
    ptr[len1] = '\n';
    return ptr;
}

/**
 */ 
static void longmulti(const char *a, const char *b, char *c)
{
	int i = 0, j = 0, k = 0, n, carry;
	int la, lb;
 
	/* either is zero, return "0" */
	if (!strcmp(a, "0") || !strcmp(b, "0")) {
		c[0] = '0', c[1] = '\0';
		return;
	}

	la = strlen(a);
	lb = strlen(b);
	memset(c, '0', la + lb);
	c[la + lb] = '\0';
 
	for (i = la - 1; i >= 0; i--) {
		for (j = lb - 1, k = i + j + 1, carry = 0; j >= 0; j--, k--) {
			n = hex2int(a[i]) * hex2int(b[j]) + hex2int(c[k]) + carry; //'3';   //I(a[i]) * I(b[j]) + I(c[k]) + carry;
			carry = n / 16;
			c[k] = int2hex((n % 16)); //n + '0';// (n % 16) + '0';
		}
		c[k] += carry;
	}

	if (c[0] == '0') memmove(c, c + 1, la + lb);
 
	return;
}

/**
 */
static char* add(const char *a, const char *b, char *ans){
    int alen, blen;
    int i, carry=0;
    char *wk;
    char *awk=strdup(a);
    char *bwk=strdup(b);
    int n;

    alen=strlen(strrev(awk));
    blen=strlen(strrev(bwk));
    if(alen<blen){
        alen ^= blen;blen ^= alen;alen ^= blen;//swap
        wk = awk ; awk = bwk ; bwk = wk;
    }
    ans[alen+1]=ans[alen]='\0';
    for(i=0;i<alen;++i){
        n = hex2int(awk[i]) +(i<blen ? hex2int(bwk[i]): 0)+carry;
        carry = n/ 16;
        ans[i] = int2hex(n % 16);
    }
    if(carry){
        ans[i++]='1';
    }
    free(awk);
    free(bwk);
    return strrev(ans);
} 

/**
 */
static char* powerh16(int n)
{
    char *c = calloc((n+1), sizeof(char));
    c[0] = '1';
    for(int i = 1; i <= n; i++){
        c[i] = '0';
    }
    return c;
} 

/**
 */
static int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

/**
 */ 
static char int2hex(int i)
{
    if(i < 10)
        return i + '0';
    else if(i == 10)
        return 'a';
    else if(i == 11)
        return 'b';
    else if(i == 12)
        return 'c';
    else if(i == 13)
        return 'd';
    else if(i == 14)
        return 'e';
    else if(i == 15)
        return 'f';
    
    return 'a';
} 

/**
 */
static char *strrev(char *str){
    char c, *front, *back;

    if(!str || !*str)
        return str;
    for(front=str,back=str+strlen(str)-1;front < back;front++,back--){
        c=*front;*front=*back;*back=c;
    }
    return str;
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
