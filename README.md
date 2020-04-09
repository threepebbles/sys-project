# 2018-2 System programming Project
## Requirements

- Two components: should be implemented by using device driver (kernel module)
  - button, LED, buzzer, ultrasonic distance sensor, servo motor, pressure sensor
- Two components: should be implemented by using wiringPi
  - LCD, gyro sensor

  
## Members & Roles
- Min Seohyeon  : Ultrasonic distance sensor, LED, motor, client application, socket communication
- Ju Jinhyeong  : LCD, buttons, server application
- Park Daeul    : gyro sensor, buzzer
- No Gyudong    : pressure sensor, buzzer

## How to implement
- Ultrasonic distance sensor(HC-SR04) -> device driver
- LED                                 -> device driver
- LCD                                 -> wiringPi
- buttons                             -> device driver
- motor                               -> device driver
- gyro sensor                         -> wiringPi
- buzzer                              -> device driver
- pressure sensor                     -> device driver

=> wiringPi 2개, device driver 6개