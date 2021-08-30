all: compile link

compile:
	gcc -g -Wall -c string_compare.c main.c

link:
	gcc string_compare.o main.o -o string_compare

clean:
	rm string_compare.o main.o
