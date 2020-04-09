KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := uds_drv.o

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod uds_drv.ko
	sudo mknod -m 666 /dev/uds_driver c 211 0

install:
	sudo insmod uds_drv.ko
	sudo mknod -m 666 /dev/uds_driver c 211 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

uninstall:
	sudo rmmod uds_drv
	sudo rm /dev/uds_driver
