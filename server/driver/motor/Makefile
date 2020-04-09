KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := servo_drv.o

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod servo_drv.ko
	sudo mknod -m 777 /dev/servo_drv c 212 0

install:
	sudo insmod servo_drv.ko
	sudo mknod -m 666 /dev/servo_drv c 212 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

uninstall:
	sudo rmmod servo_drv
	sudo rm /dev/servo_drv
