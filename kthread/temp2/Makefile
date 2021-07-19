obj-m += temp2.o

all: compile

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run:
	sudo insmod temp2.ko

clean:
	sudo rmmod temp2.ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
