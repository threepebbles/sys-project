#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define SERVO_PATH "/dev/servo_drv"

#define SERVO_IOCTL_MAGIC 'M'
#define SERVO_IOCTL_WRITE _IOW(0, 1, unsigned char) // write data to device driver

#define DOOR_CLOSE  0
#define DOOR_OPEN   1

int servo_fd, ret;

/* door_lock()
 * move servo motor by cmd
 * 0: stop, 1: +90 degree(close the door), 2:-90 degree(open the door)
 */
void door_lock(int cmd) {
    servo_fd = open(SERVO_PATH, O_RDWR);
    if(servo_fd==-1) {
        perror("servo_fd open failed\n");
        exit(-1);
    }
    ret = ioctl(servo_fd, SERVO_IOCTL_WRITE, &cmd); // servo_ioctl() is called.
    if(ret == -1) {
        printf("ioctl failed\n");
    }
    close(servo_fd);
}
