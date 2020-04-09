#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "./myBtn.h"    // is_locked

// Pressure device File Path
#define PRESSURE_DEV_PATH "/dev/pressure_drv"

/* For detected handle is pushed
   if handle is pushed, 'is_pushed()' function return 1
   else (handle is NOT pushed) 'is_pushed()' function return 0
 */
int is_pushed(){
    int ret, fd;
	char re[6];
	fd = open(PRESSURE_DEV_PATH, O_RDONLY);

	//if Open Error, Then return errno 
	if(fd < 0){
		perror("***Failed Open pressure drv***\n");
		return errno;
	}
	ret = read(fd, re, 6);
	close(fd);	
	//Message from Kernel is "PRESS" means Pressure is detected, return 1
    if(strcmp(re, "PRESS") == 0) return 1;
	//Else Pressure is detected, return 0
	else return 0;
}
