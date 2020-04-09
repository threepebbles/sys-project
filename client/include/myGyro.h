#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

//Device Address
#define DEVICE_ADDRESS 0x68

//Register Address
#define GYRO_XOUT_H  0x1D
#define GYRO_XOUT_L  0x1E

#define GYRO_YOUT_H  0x1F
#define GYRO_YOUT_L  0x20

#define GYRO_ZOUT_H  0x21
#define GYRO_ZOUT_L  0x22

#define SCALE_FACTOR 150

int gyro_fd;

/* For read raw data from Register
   Gyro informations are saved in two 8-bit register
   The displacement of the address(High, Low ) is 1 
   For bring a value from two register, We use Bit operation
 */
int read_gyro_data(int addr){
   int high_byte, low_byte, value_decimal;

    //Bring value from High addr register
   high_byte = wiringPiI2CReadReg8(gyro_fd, addr);
   //Bring value from low addr register
   low_byte = wiringPiI2CReadReg8(gyro_fd, addr + 1);   

   //By Bit Operation
   value_decimal = (high_byte << 8) | low_byte;
   return value_decimal;
}

/* For check Safe is moved
   Read Gyro data by "read_gyro_data()" and Compare previous Data
   if Value is Changed, return 1, Else return 0
 */
int is_moved(){
   int xPos, yPos, zPos;
   xPos = read_gyro_data(GYRO_XOUT_H)/SCALE_FACTOR;
   yPos = read_gyro_data(GYRO_YOUT_H)/SCALE_FACTOR;
   zPos = read_gyro_data(GYRO_ZOUT_H)/SCALE_FACTOR;
   printf("x: %d, y: %d, z: %d\n", xPos, yPos, zPos);
   if(xPos > 30 || xPos < 0) return 1;
   if(yPos > 455 || yPos < 330 ) return 1;
   if(zPos > 455    || zPos < 330 ) return 1;
   return 0;
}