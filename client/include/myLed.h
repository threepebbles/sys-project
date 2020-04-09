#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LED_PATH "/dev/led_driver"

int led_fd, ret;
const int HIGH = 1;
const int LOW = 0;

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
