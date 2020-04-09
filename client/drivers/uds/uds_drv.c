/* Device driver for UDS(ultrasonic distance sensor) hc-sr04 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>		// copy_to_user
#include <linux/kdev_t.h>

#include <linux/gpio.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/ktime.h>
/* functions related to linux/ktime.h
 * ktime_get(): get the monotonic time in ktime_t format
 * ktime_sub(a, b): return ktime_t format for a - b
 * ktime_to_ns(): convert ktime_t format to nanosecend
 */
#include <linux/delay.h>		// udelay()
#include "../../include/gpio.h"

#define UDS_MAJOR_NUM 211
#define UDS_DEV_NAME "UDS"

int irq_num;
int is_echo_clear;				// 1 if echo pin is falled(HIGH->LOW), otherwise 0
static ktime_t echo_start, echo_end;

DECLARE_WAIT_QUEUE_HEAD(wait_queue); 	// interrupt waiting queue
DEFINE_MUTEX(mutex);					// define and initialize mutex

static irqreturn_t uds_irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
	local_irq_disable();	// block interrupt while interrupt handler is running
	if(gpio_get_value(UDS_ECHO_BCM_NUM)==LOW) {	// ECHO pin gets HIGH->LOW
		echo_end = ktime_get();
		is_echo_clear = 1;
		wake_up_interruptible(&wait_queue); // wake up the sleeping interruptible process in waiting queue
	}
	else {	// ECHO pin gets LOW->HIGH
		echo_start = ktime_get();
	}
	local_irq_enable();
	return IRQ_HANDLED;
}

int uds_open (struct inode *pinode, struct file *pfile) {
	printk("open uds char drv\n");
	
	pfile->private_data = ((void *) MINOR(pinode->i_rdev));	// minor number = 0
	return 0;
}

int uds_release (struct inode *pinode, struct file *pfile) {
	printk("release uds char drv\n");
	return 0;
}

/* 
 * Every user want to get distance from the ultrasonic distance sensor, uds_read is called
 */
ssize_t uds_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	long long duration;

	printk("read uds char drv\n");
	if (length != sizeof(duration)) {
		printk(KERN_DEBUG "uds_read: Invalid length size\n");
		return -EINVAL;	// Invalid argument
	}

	if (mutex_lock_interruptible(&mutex))	// try to get mutex. if process fails to get the mutex, it is switched to interruptible sleep state.
		return -ERESTARTSYS;	// used for blocking IO

	/* critical section starts */
	switch (((int) pfile->private_data)) {	// minor number of the device driver
		case 0:	// minor number of uds
		{
			is_echo_clear = 0;
			gpio_set_value(UDS_TRIGGER_BCM_NUM, HIGH);
			udelay(10);	// TRIGGER pin have to be HIGH for at least 10 us according to datasheet
			gpio_set_value(UDS_TRIGGER_BCM_NUM, LOW);

			// wait_event_timeout: sleep in waiting queue until a condition gets true or a timeout elapses
			// when the ECHO pin receives ultrasonic or 1/10s(100000us) elapse, it wakes up.
			// UDS can detect up to 1700cm (10000us * 0.034cm/us / 2)
			// constant HZ: number of timer ticks per second.
			wait_event_timeout(wait_queue, is_echo_clear, HZ / 10.0);

			if (!is_echo_clear) {	// wait_event_timeout is not called after transmitting wave(TRIGGER)
				printk(KERN_ERR "uds_read: timeout elapses\n");
				mutex_unlock(&mutex);
				return -EIO;	// Input/output error
			}
			duration = ktime_to_us(ktime_sub(echo_end, echo_start));	// unit: us
		}
		break;

		default:
			printk("uds_read: Invalid device minor number %d\n", (int) pfile->private_data);
			mutex_unlock(&mutex);
			return -EINVAL; // Invalid argument
	}

	printk(KERN_INFO "duration: %lld\n", duration);
	if(copy_to_user(buffer, &duration, sizeof(duration))) {
		printk(KERN_ERR "uds_read: copy_to_user failed\n");
		mutex_unlock(&mutex);
		return -EACCES; // Permission denied (POSIX.1)
	}
	mutex_unlock(&mutex);
	/* critical section ends */

	return sizeof(duration);
}

// ssize_t uds_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
// 	printk("write uds char drv%d\n", length);
// 	printk(KERN_ALERT "It shouldn't be used\n");
// 	return length;
// }

struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = uds_open,
	.release = uds_release,
	.read = uds_read,
	// .write = uds_write,
};

int __init uds_init(void) {
    printk("Hello uds drv\n");

	int err;
	/* request use of TRIGGER pin with direction output */
    if((err = gpio_request_one(UDS_TRIGGER_BCM_NUM, GPIOF_DIR_OUT, "uds trigger"))) {
    	printk(KERN_ERR "gpio_request_one trigger failed\n");
		return -1;
    }
    /* request use of ECHO pin direction input */
	if((err = gpio_request_one(UDS_ECHO_BCM_NUM, GPIOF_DIR_IN, "uds echo"))) {
		printk(KERN_ERR "gpio_request_one echo failed\n");
		return -1;
	}
	/* interrupt request ECHO pin */
	if((irq_num = gpio_to_irq(UDS_ECHO_BCM_NUM))<0) {
		printk(KERN_ERR "gpio_to_irq echo failed\n");
		return -1;
	}

	/* register interrupt */
	if ((err = request_irq(irq_num, (irq_handler_t) uds_irq_handler, // register interrupt handler
		IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,	// interrupt occured when ECHO pin gets rising or falling
	UDS_DEV_NAME, NULL))) {
		printk(KERN_ERR "request_irq failed\n");
		return -1;
	}

    register_chrdev(UDS_MAJOR_NUM, UDS_DEV_NAME, &fop);
	dev_t devno = MKDEV(UDS_MAJOR_NUM, 0);
	printk("sudo mknod -m 666 /dev/uds_driver c %d %d\n", MAJOR(devno), MINOR(devno));

	return 0;
}

void __exit uds_exit(void) {
	printk("Goodbye uds drv\n");
	
	free_irq(irq_num, NULL);
	gpio_free(UDS_TRIGGER_BCM_NUM);
	gpio_free(UDS_ECHO_BCM_NUM);
	unregister_chrdev(UDS_MAJOR_NUM, UDS_DEV_NAME);
}

module_init(uds_init);
module_exit(uds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("heiler(SeohyeonMin)");
MODULE_DESCRIPTION("HC-SR04 device driver");
