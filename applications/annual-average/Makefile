all: compile link

compile:
	gcc -Ilib -Iobj -g -Wall -c annual_average.c -o obj/annual_average.o

link:
	gcc obj/annual_average.o obj/delay_milliseconds.o obj/string_compare.o -o annual_average

clean:
	rm obj/annual_average.o
