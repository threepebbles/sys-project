#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV_PATH "/dev/buzzer_driver"

int buzzer_fd, ret;

void buzzer_on() {
	char message[3] = "on";
	
	if((buzzer_fd = open(DEV_PATH, O_RDWR | O_NONBLOCK)) < 0) {
		printf("buzzer_fd open failed\n");
		exit(-1);
	}
	ret = write(buzzer_fd, message, (strlen(message)+1));
	if(ret < 0){
		printf("buzzer_fd write failed\n");
		exit(-1);
	}
    printf("buzzer on success!\n");
	close(buzzer_fd);
}

void buzzer_off() {
	char message[3] = "of";
	
	if((buzzer_fd = open(DEV_PATH, O_RDWR | O_NONBLOCK)) < 0) {
		printf("buzzer_fd open failed\n");
		exit(-1);
	}
	ret = write(buzzer_fd, message, (strlen(message)+1));
	if(ret < 0){
		printf("buzzer_fd write failed\n");
		exit(-1);
	}
	printf("buzzer off success!\n");
	close(buzzer_fd);
}

