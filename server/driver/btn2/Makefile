KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := btn_drv2.o

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod btn_drv2.ko major=221
	sudo mknod -m 777 /dev/btn_drv2 c 221 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf btn_app btn_test

install:
	sudo insmod btn_drv2.ko major=221
	sudo mknod -m 777 /dev/btn_drv2 c 221 0

uninstall:
	sudo rmmod btn_drv2
	sudo rm -rf /dev/btn_drv2

