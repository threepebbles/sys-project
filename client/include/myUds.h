#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define UDS_PATH "/dev/uds_driver"
const double speed_of_sound = 0.034;	// unit: cm/us
int uds_fd;

double uds_get_distance() {
	uds_fd = open(UDS_PATH, O_RDONLY | O_RSYNC);
	if(uds_fd==-1){
		perror("uds device file open error");
		return -1;
	}
	
	long long duration=-1;
	double distance=0;
	if(read(uds_fd, &duration, sizeof(duration))<0) {
		perror("read error");
		return -1;
	}

	sleep(1);
	/* round trip distance: dutaration(us) * speed of sound(cm/us)
	 * speed of sound: 340m/s = 0.034cm/us
	 */
	distance = (duration*speed_of_sound)/2;
	close(uds_fd);

	printf("distance: %lfcm\n", distance);
	return distance;
}