/********************************************//**
 *  HEADER FOR the intmul application 
 * 
 * @file intmul.h
 * @author Thomas Robert Pokorny 1527212
 * 
 * @date 17 Dec 2019
 * 
 * @brief defines the funktions
 * 
 * 
 ***********************************************/
#ifndef INTMUL_H_
#define INTMUL_H_

/**
 * @brief holds all server infomations
 * @var port the server  port 
 * @var indexFile the index file
 * @var documentRoot the document root for the server
 */ 
typedef struct intmul_confs {
   char *hex_1;
   char *hex_2; 
} intmul_conf;

/**
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
