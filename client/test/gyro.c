#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

//Device Address
#define DEVICE_ADDRESS 0x68

//Register Address
#define SMPLRT_DIV 0x15

#define INT_CFG   0x17
#define INT_STATUS   0x1A

#define GYRO_XOUT_H  0x1D
#define GYRO_XOUT_L  0x1E

#define GYRO_YOUT_H  0x1F
#define GYRO_YOUT_L  0x20

#define GYRO_ZOUT_H  0x21
#define GYRO_ZOUT_L  0x22

#define PWR_MGM   0x3E

#define SCALE_FACTOR 150

int gyro_fd;
float xPos, yPos, zPos;

void GYRO_Init(){
    wiringPiI2CWriteReg8 (gyro_fd, SMPLRT_DIV, 0x07);	/* Write to sample rate register */
	
    //PWR CHECK!!
    wiringPiI2CWriteReg8 (gyro_fd, PWR_MGM, 0);	/* Write to power management register */

    wiringPiI2CWriteReg8 (gyro_fd, INT_CFG, 0x01);	/*Write to interrupt enable register */
    wiringPiI2CWriteReg8 (gyro_fd, INT_STATUS, 0x01);	/*Write to interrupt enable register */

}

int read_raw_data(int address){
	int high_byte, low_byte, value_decimal;
	
        high_byte = wiringPiI2CReadReg8(gyro_fd, addr);
	low_byte = wiringPiI2CReadReg8(gyro_fd, addr + 1);	

	value_decimal = (high_byte << 8) | low_byte;
	return value_decimal;
}


int main() {
    gyro_fd = wiringPiI2CSetup(DEVICE_ADDRESS);   /*Initializes I2C with device Address*/
    GYRO_Init();		                

    while(1){

		xPos = read_raw_data(GYRO_XOUT_H)/SCALE_FACTOR;
		yPos = read_raw_data(GYRO_YOUT_H)/SCALE_FACTOR;
		zPos = read_raw_data(GYRO_ZOUT_H)/SCALE_FACTOR;
		
		printf("\n xPos = %.3f \t yPos = %.3f \t zPos = %.3f \n",Gx,Gy,Gz);
		delay(300);

    }
    return 0;
}
