#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LED_PATH "/dev/led_driver"

int led_fd, ret;
int HIGH =1;
int LOW = 0;

void led_on() {
	led_fd = open(LED_PATH, O_RDWR | O_RSYNC);
	ret = write(led_fd, &HIGH, sizeof(HIGH));
	if(ret<0){
		fprintf(stderr, "led_on failed\n");
		exit(-1);
	}
	printf("led is on!\n");
	close(led_fd);
}

void led_off() {
	led_fd = open(LED_PATH, O_RDWR | O_RSYNC);
	ret = write(led_fd, &LOW, sizeof(LOW));
	if(ret<0){
		fprintf(stderr, "write failed\n");
		exit(-1);
	}
	printf("led is off!\n");
	close(led_fd);
}


int main() {
	int cmd = 0;
	/* cmd=0 : turn the led off
	 * cmd=1 : turn the led on
	 * cmd=-1 : exit
	 */
	while(1) {
		printf("0: led off\n1: led on\n-1: exit program\n");
		printf("cmd: ");
		scanf("%d", &cmd);
		if(cmd==-1) {
			break;
		}
		switch(cmd) {
			case -1:
			{
				printf("exit!\n");
				break;
			}
			case 0:
			{
				led_off();
				break;
			}
			case 1:
			{
				led_on();
				break;
			}
			default:
			printf("invalid command\n");
			break;
		}
	}
	
	printf("program ended\n");
	return 0;
}