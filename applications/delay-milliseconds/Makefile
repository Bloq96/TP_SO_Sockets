all: compile link

compile:
	gcc -g -Wall -c delay_milliseconds.c main.c

link:
	gcc delay_milliseconds.o main.o -o delay_milliseconds

clean:
	rm delay_milliseconds.o main.o
