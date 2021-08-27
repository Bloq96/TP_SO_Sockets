obj-m += server.o
obj-m += client.o

all: compile

run: run_server run_client

remove: remove_server remove_client

clear: remove clean

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run_server:
	sudo insmod server.ko

run_client:
	sudo insmod client.ko

remove_server:
	sudo rmmod server.ko

remove_client:
	sudo rmmod client.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
