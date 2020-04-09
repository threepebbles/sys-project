
/* Pressur Device Driver  */
#include <linux/gpio.h> 
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>

#include "../../include/gpio.h"

#define PRESSURE_MAJOR_NUM 270
#define PRESSURE_DEV_NAME "pressure_drv"


//For Open pressure_drv
ssize_t pressure_open(struct inode *pinode, struct file *pfile){
    printk("open pressure char drv\n");
    return 0;
}

//For Close pressure_drv
ssize_t pressure_release(struct inode *pinode, struct file *pfile){
    printk("release pressure char drv\n");
    return 0;
}

//For User read
ssize_t pressure_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
    //if pressure is detected, set BMC_21_pin's value to 1
    if(gpio_get_value(PRESSURE_BCM_NUM) == 1){
        // And copy_to_user "PRESS" char 
        copy_to_user(buffer, "PRESS", length);
    }
    //if pressure is not detected, set BMC_21_pin's value to 0
    else{
        // And copy_to_user "NOTCH" char 
        copy_to_user(buffer, "NOTCH", length);
    }
    return 0;
} 

struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = pressure_open,
    .read = pressure_read,
	.release = pressure_release,
};


// For init pressure drv
int __init pressure_init(void) {
   printk(KERN_ALERT "---pressure_drv init---\n");
   gpio_request(PRESSURE_BCM_NUM, "PRESSURE_BCM_NUM");
   //For Use BCM_21 Pin Input
   gpio_direction_input(PRESSURE_BCM_NUM);
   register_chrdev(PRESSURE_MAJOR_NUM, PRESSURE_DEV_NAME, &fop);
   //Check Majr, minor number by kernel message
   dev_t devno = MKDEV(PRESSURE_MAJOR_NUM, 0);
   printk("sudo mknod -m 666 /dev/preesure_drv c %d %d\n", MAJOR(devno), MINOR(devno));
   return 0;
}


// For exit pressure drv
void __exit pressure_exit(void) {
   printk(KERN_ALERT "---pressure_drv exit---\n");
   unregister_chrdev(PRESSURE_MAJOR_NUM, PRESSURE_DEV_NAME);
}

module_init(pressure_init);
module_exit(pressure_exit);

//Pressure Device driver license
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PDE");