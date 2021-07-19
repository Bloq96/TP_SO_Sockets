obj-m += do.o

all: compile

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run:
	sudo insmod do.ko

clean:
	sudo rmmod do.ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
