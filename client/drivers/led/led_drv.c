/* Device driver for LED */

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/uaccess.h>  // copy_from_user(), copy_to_user()
#include <linux/slab.h>
#include "../../include/gpio.h"

#define LED_MAJOR_NUM 210
#define LED_DEV_NAME "LED"

unsigned int* gpio_base_addr;

int led_open(struct inode *pinode, struct file *pfile) {
    printk("open led drv\n");
    gpio_base_addr = get_vir_addr((unsigned int*)GPIO_BASE);
    my_gpio_set_mode(gpio_base_addr, LED_BCM_NUM, GPIO_OUT);
    return 0;
}

int led_close(struct inode *pinode, struct file *pfile) { 
    printk("close led_dev\n");
}  

// // led_state()
// ssize_t led_read(struct file* pfile, char __user* buffer, size_t length, loff_t* offset){
//     printk("read led drv\n");
//     if(my_gpio_get(gpio_base_addr, LED_BCM_NUM)){
//         if(copy_to_user(buffer, &HIGH, sizeof(HIGH)) != 0){
//             printk(KERN_ALERT "copy_to_user failed\n");
//             return 0;
//         }
//     }
//     else{
//         if(copy_to_user(buffer, &LOW, sizeof(LOW)) != 0){
//             printk(KERN_ALERT "copy_to_user failed\n");
//             return 0;
//         }
//     }
//     return 0;
// }

// led_on(), led_off()
ssize_t led_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {
    printk("write led drv %d\n", length);
    int state = -1;
    int ret = copy_from_user(&state, buffer, length);
    if(ret < 0) {
        printk("copy_from_user failed\n");
        return -1;
    }

    printk("state: %d\n", state);
    if(state == 1) {
        my_gpio_set(gpio_base_addr, LED_BCM_NUM);
    }
    else if(state == 0) {
        my_gpio_clear(gpio_base_addr, LED_BCM_NUM);
    }
    return length;
}

struct file_operations fop = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_close,
    // .read = led_read,
    .write = led_write,
};
    
int __init led_init(void) {
    printk("Hello led drv\n");
    register_chrdev(LED_MAJOR_NUM, LED_DEV_NAME, &fop);
    dev_t devno = MKDEV(LED_MAJOR_NUM, 0);
	printk("sudo mknod -m 666 /dev/led_driver c %d %d\n", MAJOR(devno), MINOR(devno));

    return 0;
}

void __exit led_exit(void) {
    printk("Goodbye led drv\n");
    unregister_chrdev(LED_MAJOR_NUM, LED_DEV_NAME);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("heiler(SeohyeonMin)");
MODULE_DESCRIPTION("LED device driver");