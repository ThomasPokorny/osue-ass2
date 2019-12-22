/********************************************//**
 *  HEADER FOR the intmul application 
 * 
 * @file intmul.h
 * @author Thomas Robert Pokorny 1527212
 * 
 * @date 17 Dec 2019
 * 
 * @brief defines the structs needed for intmul
 * 
 * 
 ***********************************************/
#ifndef INTMUL_H_
#define INTMUL_H_

/**
 * @brief holds both numbers needed for the computation
 * @var hex_1 the first number
 * @var hex_2 the second number
 */ 
typedef struct intmul_confs {
   char *hex_1;
   char *hex_2; 
} intmul_conf;

/**
 * @brief holds the informtion needed for the master process to comunicated with the child processes
 * @var pid1 process id of the first child
 * @var pid2 process id of the second child
 * @var pid3 process id of the third child
 * @var pid4 process id of the fourth child
 * 
 * @var ah first half of the first number
 * @var al second half of the first number
 * @var bh first half of the second number
 * @var bl second half of the second number
 */ 
typedef struct master_confs {
   int pid1;
   int pid2;
   int pid3;
   int pid4;
   char *ah;
   char *al;
   char *bh;
   char *bl; 
   size_t n;
} master_conf;

#endif 
