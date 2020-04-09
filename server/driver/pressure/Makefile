KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := pressure_drv.o

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod pressure_drv.ko
	sudo mknod -m 666 /dev/pressure_drv c 270 0

install:
	sudo insmod pressure_drv.ko
	sudo mknod -m 666 /dev/pressure_drv c 270 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

uninstall:
	sudo rmmod pressure_drv
	sudo rm /dev/pressure_drv
