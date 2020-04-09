/* header for General Purpose I/O (data sheet ch6) in kernel */

#include <linux/kernel.h>
#include <asm/io.h> // iomap

/* BCM pins in Raspberry Pi used as client */
#define LED_BCM_NUM 27     		// BCM number of LED (GPIO GEN)
#define UDS_TRIGGER_BCM_NUM 13	// BCM number of UDS trigger (GPIO GEN)
#define UDS_ECHO_BCM_NUM 19		// BCM number of UDS echo (GPIO GEN)
#define BUZZER_BCM_NUM 25		// BCM number of BUZZER (GPIO GEN)

#define HIGH 1
#define LOW 0

#define GPIO_IN 0x000
#define GPIO_OUT 0x001
#define GPIO_BASE 0x3F200000

#define PAGESIZE (4096)

/* macros for error */
#define VIR_ADDR_ERROR 100
#define SET_ERROR 101
#define GET_ERROR 102
#define MODE_ERROR 103
#define CLEAR_ERROR 104

static int ERROR = 0;

/* get the virtual address by using ioremap */
unsigned int* get_vir_addr(unsigned int* addr) {
	unsigned int* gpio_base_addr = ioremap((unsigned long)addr, PAGESIZE);

	if(gpio_base_addr < 0){
		printk(KERN_ALERT "get_gpio_base_addr: ioremap failed\n");
		ERROR = VIR_ADDR_ERROR;
		return 0;
	}
	
	return (unsigned int*) gpio_base_addr;
}

/* GPIO Function Select Register (GPFSELn)
 * change the mode of a port.
 * we generally use just two modes GPIO_IN, GPIO_OUT.
 * GPIO_IN: use GPIO as input, GPIO_OUT: use GPIO as output
 */
void my_gpio_set_mode(unsigned int* addr, int port, int mode){
	unsigned int* a;
	unsigned int mask;
	if(0 > port || port > 31){
		printk(KERN_ALERT "port out of range: %d\n", port);
		ERROR = MODE_ERROR;
		return;
	}
	a = (unsigned int*)(addr + (port/10));
	mask = ~(0x7 << ((port%10) * 3));
	*a &= mask;
	*a |= (mode & 0x7) << ((port%10) * 3);
}

/* GPIO Pin Output Set Registers (GPSETn)
 * gpio_set() changes the value of GPSETn register.
 * make GPIO pin high.
 * If the pin is used as input, it has no effect.
 */
void my_gpio_set(unsigned int* addr, int port){
	if(0 > port || port > 31){
		printk(KERN_ALERT "gpio_set: port out of range: %d\n", port);
		ERROR = SET_ERROR;
		return;
	}
	*(addr + 7) = 0x1 << port; // 7 * sizeof(int) = 0x1C bytes
}


/* GPIO Pin Ouput Clear Registers (GPCLRn)
 * gpio_clear() changes the value of GPCLRn register
 * make GPIO pin low.
 * If the pin is used as input, it has no effect.
 */
void my_gpio_clear(unsigned int* addr, int port){
	if(0 > port || port > 31){
        printk(KERN_ALERT "gpio_clear: port out of range: %d\n", port);
		ERROR = CLEAR_ERROR;
        return;
    }
	*(addr + 10) = 0x1 << port;
}

/* GPIO Pin Level Registers (GPLEVn)
 * return the actual value of the pin.
 */
int my_gpio_get(unsigned int* addr, int port){
	int ret=0;
	if(0 > port || port > 31){
		printk(KERN_ALERT "gpio_get: port out of range: %d\n", port);
		ERROR = GET_ERROR;
		return -1;
	}
	ret = (*(addr + 13) & (0x1 << port));
	return ret == 0 ? 0 : 1;
}

void my_gpio_free(unsigned int* addr) {
	iounmap(addr);
}
