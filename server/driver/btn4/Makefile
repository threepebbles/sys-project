KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := btn_drv4.o

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod btn_drv4.ko major=223
	sudo mknod -m 777 /dev/btn_drv4 c 223 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	sudo insmod btn_drv4.ko major=223
	sudo mknod -m 777 /dev/btn_drv4 c 223 0

uninstall:
	sudo rmmod btn_drv4
	sudo rm -rf /dev/btn_drv4
