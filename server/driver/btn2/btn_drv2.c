#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/gpio.h>

#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/ktime.h>

#include "../../include/gpio.h"

#define BTN_DEV_NAME "BTN"

const int GPIO_ON = 1;
const int GPIO_OFF = 0;

int pin_n;
unsigned int* gpio_base_addr;

struct cdev my_cdev;
dev_t devno;
char* msg;
int major;
int irq_num;

module_param(major, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(major, "major");

int btn_open(struct inode* pinode, struct file* pfile){
	printk(KERN_INFO "Button open\n");
    gpio_base_addr = get_vir_addr((unsigned int*)GPIO_BASE);
	pin_n = BTN2_BCM_NUM;
    my_gpio_set_mode(gpio_base_addr, pin_n, GPIO_IN);

    return 0;
}

ssize_t btn_read(struct file* pfile, char __user* buffer, size_t length, loff_t* offset){
	int ret;
	// printk(KERN_INFO "Button read\n");
	ret = my_gpio_get(gpio_base_addr, pin_n);
	if(copy_to_user(buffer, &ret, sizeof(int))){
		printk(KERN_ALERT "copy_to_user failed\n");
		return 0;
	}
	return 1;
}

ssize_t btn_write(struct file* pfile, const char __user* buffer, size_t length, loff_t* offset){
	printk(KERN_INFO "Button write\n");
	return 0;
}

int btn_release(struct inode* pinode, struct file* pfile){
    printk(KERN_INFO "Button close\n");
    
	return 0;
}

struct file_operations fop = {
    .owner      = THIS_MODULE,
    .open       = btn_open,
    .read       = btn_read,
	.write		= btn_write,
    .release    = btn_release,
};

int __init btn_init(void){
	int err;
	printk(KERN_INFO "Button init\n");
	if(major){
		devno = MKDEV(major, 0);
		register_chrdev_region(devno, 1, "Button drv");
	}
	else{
		alloc_chrdev_region(&devno, 0, 1, "Button drv");
		major = MAJOR(devno);
	}

	cdev_init(&my_cdev, &fop);
	my_cdev.owner = THIS_MODULE;
	err = cdev_add(&my_cdev, devno, 1);

	if(err < 0){
		printk("Device add error\n");
		return -1;
	}

	printk("Hello. This is Button dev.\n");
	printk("sudo mknod -m 777 /dev/btn_drv c %d %d\n", MAJOR(devno), MINOR(devno));

	msg = (char*)kmalloc(32, GFP_KERNEL);
	if(msg != NULL){
		printk("malloc allocator address: 0x%p\n", msg);
	}
    return 0;
}

void __exit btn_exit(void){
    printk(KERN_INFO "Button exit\n");
    
	if(msg){
		kfree(msg);
	}
	unregister_chrdev_region(devno, 1);
	cdev_del(&my_cdev);
}

module_init(btn_init);
module_exit(btn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ju(JinhyeongJu)");
MODULE_DESCRIPTION("button device driver");

