obj-m += socket_thread.o

all: compile

clear: remove clean

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run:
	sudo insmod socket_thread.ko

remove:
	sudo rmmod socket_thread.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
