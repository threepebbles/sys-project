/* Device driver for buzzer */

#include <linux/gpio.h> 
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include "../../include/gpio.h"

#define BUZZER_MAJOR_NUM 212
#define BUZZER_DEV_NAME "buzzer"

ssize_t buzzer_open(struct inode *pinode, struct file *pfile){
	printk("open buzzer char drv\n");
	return 0;
}

ssize_t buzzer_release(struct inode *pinode, struct file *pfile){
    printk("release buzzer char drv\n");
    return 0;
}

ssize_t buzzer_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
	printk("write buzzer char drv\n");

	char msg[3];
	int err = copy_from_user(msg, buffer, length);
	
	if(err < 0){
		printk(KERN_ERR "copy_from_user failed\n");
		return -1;
	}
	printk("msg: %s\n", msg);

	// Beep Sound on
	if(strcmp(msg, "on") == 0){
		gpio_direction_output(BUZZER_BCM_NUM, 1);
	}

	// Beep Sound off
	if(strcmp(msg, "of") == 0){
		gpio_direction_output(BUZZER_BCM_NUM, 0);
	}
	return length;
}

struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = buzzer_open,
	.write = buzzer_write,
	.release = buzzer_release,
};

int __init buzzer_init(void) {
	printk("Hello buzzer drv\n");

	gpio_request(BUZZER_BCM_NUM, "BUZZER_BCM_NUM");
	gpio_direction_output(BUZZER_BCM_NUM, 0);

	register_chrdev(BUZZER_MAJOR_NUM, BUZZER_DEV_NAME, &fop);
	dev_t devno = MKDEV(BUZZER_MAJOR_NUM, 0);
	printk("sudo mknod -m 666 /dev/buzzer_driver c %d %d\n", MAJOR(devno), MINOR(devno));

	return 0;
}

void __exit buzzer_exit(void) {
	printk(KERN_ALERT "Goodbye buzzer drv\n");

	gpio_free(BUZZER_BCM_NUM);
	unregister_chrdev(BUZZER_MAJOR_NUM, BUZZER_DEV_NAME);
}

module_init(buzzer_init);
module_exit(buzzer_exit);

MODULE_LICENSE("GPL");