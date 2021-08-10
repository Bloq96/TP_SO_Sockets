obj-m += client_socket.o

all: compile

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run:
	sudo insmod client_socket.ko

clean:
	sudo rmmod client_socket.ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
