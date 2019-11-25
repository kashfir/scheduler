CC=g++
CFLAGS= -c -Wall -std=c++11
POSIXFLAG=-pthread -std=c++11

all: main

main: main.o
	$(CC) $(POSIXFLAG) main.cpp  -o main

main.o: main.cpp
	$(CC) $(CFLAGS) $(POSIXFLAG) main.cpp

clean:
	rm -rf *o main
