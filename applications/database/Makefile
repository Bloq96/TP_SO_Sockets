all: compile link

compile:
	gcc -Ilib -Iobj -g -Wall -c database.c -o obj/database.o

link:
	gcc obj/database.o obj/string_compare.o obj/delay_milliseconds.o -o database

clean:
	rm obj/database.o
