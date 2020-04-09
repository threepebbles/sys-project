/* Device driver for servo motor SG-5010 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>      // copy_to_user
#include <linux/kdev_t.h>

#include <linux/gpio.h>
#include <linux/slab.h>  // function realated to control string

#include <linux/delay.h> // udelay()
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
/*
struct timer_list {
    sturct list_head entry;             // entry in linked list of timers
    unsigned long expires;              // expiration value, in jiffies
    void (*function)(unsigned long);    // timer handler
    unsigned long data;                 // argument to the handler
    struct tvec_t_base_s *base;         // internal timer field, don't touch
}
*/
#include "../../include/gpio.h"

extern unsigned long volatile __cacheline_aligned_in_smp __jiffy_arch_data jiffies; // kernel version 4.14.79

#define SERVO_DEV_NAME "servo motor"
#define SERVO_MAJOR_NUM 212
#define SERVO_PULSE_CYCLE 2   // Pulse cycle 20ms (2 ticks). jiffies clock ensure 1 tick is 10ms.
#define SERVO_IOCTL_WRITE _IOW(0, 1, unsigned char) // write data to device driver

struct timer_list servo_timer;
int msg;

// pulse every 20ms to persist degree
void servo_timer_callback(unsigned long arg) {
    gpio_set_value(SERVO_BCM_NUM, HIGH);
    udelay(PULSE_TIME[msg]); // persist HIGH for appropriate time
    gpio_set_value(SERVO_BCM_NUM, LOW);
    mod_timer(&servo_timer, jiffies + SERVO_PULSE_CYCLE);  // reset expire time
}

int servo_open(struct inode *SERVO_BCM_NUMode, struct file *pfile) {
    printk("open servo char drv\n");
    return 0;
}

int servo_release(struct inode *SERVO_BCM_NUMode, struct file *pfile) {
    printk("release servo char drv\n");
    return 0;
}

long servo_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg) {
    switch(cmd) {
        int client_msg=-1;
        case SERVO_IOCTL_WRITE:
        {
            if(copy_from_user(&client_msg, arg, sizeof((int)arg))) {
                printk(KERN_ERR "servo_ioctl: copy_from_user failed\n");
                return -1;
            }
            printk("client_msg: %d\n", client_msg);
            
            // 0: close, 1: open
            if(0<= client_msg && client_msg <= 1) {
                msg = client_msg;
                mod_timer(&servo_timer, jiffies + SERVO_PULSE_CYCLE);
            }
            else
                printk("invalid client_msg\n");
        }
        break;
        default:
            printk("invalid command...\n");
            break;
    }

    return 0;
}

struct file_operations fop = {
    .owner = THIS_MODULE,
    .open = servo_open,
    .release = servo_release,
    .unlocked_ioctl = servo_ioctl,
};

int __init servo_init(void) { 
    printk("Hello servo drv\n");
    int err;

    register_chrdev(SERVO_MAJOR_NUM, SERVO_DEV_NAME, &fop);
    
    init_timer(&servo_timer);   // setup timer
    servo_timer.function = servo_timer_callback;          // callback function
    servo_timer.expires = jiffies + SERVO_PULSE_CYCLE; // setting pulse cycle
    add_timer(&servo_timer);                           // start servo_timer

    /* request use of servo motor pin with direction output */
    if((err = gpio_request_one(SERVO_BCM_NUM, GPIOF_DIR_OUT, "servo motor"))) {
        printk(KERN_ERR "gpio_request_one failed\n");
        return -1;
    }

    dev_t devno = MKDEV(SERVO_MAJOR_NUM, 0);
    printk("sudo mknod -m 666 /dev/servo_driver c %d %d\n", MAJOR(devno), MINOR(devno));

    return 0;
}

void __exit servo_exit(void) {
    printk("Goodbye servo drv\n");

    del_timer(&servo_timer);   // remove timer
    gpio_free(SERVO_BCM_NUM);
    unregister_chrdev(SERVO_MAJOR_NUM, SERVO_DEV_NAME);
}

module_init(servo_init);
module_exit(servo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("heiler(SeohyeonMin)");
MODULE_DESCRIPTION("SG-5010 device driver");
