all: compile link

compile:
	gcc -g -Wall -c client.c -o client.o
	gcc -g -Wall -c server.c -o server.o

link:
	gcc -o client client.o
	gcc -o server server.o
	
run_client:
	./client

run_server:
	./server

clean:
	rm client.o server.o
