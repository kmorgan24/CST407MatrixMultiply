#**************************************************
# Makefile for CST407 Matrix Multiply
#
# Author: Philip Howard
#
CC = mpiCC
CFLAGS = -g -O4 -Wall -pthread -std=c11
CPPFLAGS = -g -O4 -Wall -std=c++11
LFLAGS = -pthread

OBJS = matrix.o

all: mmult

clean:
	rm -f mmult
	rm -f *.o
	rm -f out

.c.o:
	gcc $(CFLAGS) -c $? -o $@

.cpp.o:
	g++ $(CPPFLAGS) -c $? -o $@

mmult: mmult.o $(OBJS)
	gcc mmult.o -o mmult $(OBJS) $(LFLAGS)

