
#include <time.h>
#include <stdio.h>
#include "../include/myGyro.h"
#include "../include/myBuzzer.h"


void gyro_loop();

int main() {
	gyro_fd = wiringPiI2CSetup(DEVICE_ADDRESS);
	gyro_loop();
}

void gyro_loop() {
  while(1) {
    if(is_moved()) {
    	buzzer_on();
    }
    else{
    	buzzer_off();
    }
    sleep(1);
  }
}