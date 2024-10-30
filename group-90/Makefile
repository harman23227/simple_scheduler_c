# Makefile for compiling scheduler, shell, and other programs

all: scheduler fib helloworld helloworld_1 helloworld_2 sum 

scheduler: scheduler.c
	gcc -o scheduler scheduler.c -lpthread

fib: fib.c
	gcc -o fib fib.c

helloworld: helloworld.c
	gcc -o helloworld helloworld.c

helloworld_1: helloworld_1.c
	gcc -o helloworld_1 helloworld_1.c

helloworld_2: helloworld_2.c
	gcc -o helloworld_2 helloworld_2.c

sum: sum.c
	gcc -o sum sum.c


clean:
	rm -f scheduler  fib helloworld helloworld_1 helloworld_2 sum 
