KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := led_drv.o

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod led_drv.ko
	sudo mknod -m 666 /dev/led_driver c 210 0

install:
	sudo insmod led_drv.ko
	sudo mknod -m 666 /dev/led_driver c 210 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

uninstall:
	sudo rmmod led_drv
	sudo rm /dev/led_driver
