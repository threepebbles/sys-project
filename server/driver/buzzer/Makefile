KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := buzzer_drv.o

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	sudo insmod buzzer_drv.ko
	sudo mknod -m 666 /dev/buzzer_drv c 213 0

install:
	sudo insmod buzzer_drv.ko
	sudo mknod -m 666 /dev/buzzer_drv c 213 0

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

uninstall:
	sudo rmmod buzzer_drv
	sudo rm /dev/buzzer_drv
