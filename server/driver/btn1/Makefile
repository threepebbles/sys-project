KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := btn_drv1.o

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod btn_drv1.ko major=220
	sudo mknod -m 777 /dev/btn_drv1 c 220 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	sudo insmod btn_drv1.ko major=220
	sudo mknod -m 777 /dev/btn_drv1 c 220 0

uninstall:
	sudo rmmod btn_drv1
	sudo rm -rf /dev/btn_drv1
