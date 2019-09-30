#!/bin/bash

CC=g++ -std=c++11
CFLAGS= -O2 
LDFLAGS=
EXEC= gscarf convert
OBJ1= graph_binary.o community.o
OBJ2= graph.o
LIB= # add your path of libboost_serialization.a

all: $(EXEC)

gscarf : $(OBJ1) main.o $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

convert : $(OBJ2) convert.o $(LIB) 
	$(CC) -o $@ $^ $(LDFLAGS)

##########################################
# Generic rules
##########################################

%.o: %.cpp %.h
	$(CC)  -o $@ -c $< $(CFLAGS)

%.o: %.cpp
	$(CC)  -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o *~ $(EXEC)
