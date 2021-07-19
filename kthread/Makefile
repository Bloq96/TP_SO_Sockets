obj-m += socket_thread.o

all: compile

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run:
	sudo insmod socket_thread.ko

clean:
	sudo rmmod socket_thread.ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
