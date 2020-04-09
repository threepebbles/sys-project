KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := btn_drv3.o

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod btn_drv3.ko major=222
	sudo mknod -m 777 /dev/btn_drv3 c 222 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	sudo insmod btn_drv3.ko major=222
	sudo mknod -m 777 /dev/btn_drv3 c 222 0

uninstall:
	sudo rmmod btn_drv3
	sudo rm -rf /dev/btn_drv3
