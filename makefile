 #*******************************************//**
 # intmul make file 
 # 
 # @file makefile
 # @author Thomas Robert Pokorny 1527212
 # 
 # @date 19 Dec 2019
 # 
 # @brief compiles and builds all nessesary files for intmul
 # 
 # 
 # 
 #***********************************************/
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

.PHONY: all clean

all : intmul

intmul : intmul.o
	$(CC) $(CFLAGS) -o intmul intmul.o

intmul.o : intmul.c intmul.h
	$(CC) $(CFLAGS) -c -o intmul.o intmul.c

clean:
	rm -rf *.o intmul