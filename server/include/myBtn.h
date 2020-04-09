#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <lcd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>

#include "./myServo.h"

#define BTN_DRV1_PATH "/dev/btn_drv1"
#define BTN_DRV2_PATH "/dev/btn_drv2"
#define BTN_DRV3_PATH "/dev/btn_drv3"
#define BTN_DRV4_PATH "/dev/btn_drv4"
#define SERVO_MOTER_DRV_PATH "/dev/servo_drv"
#define LOCK_FILE "/home/pi/Desktop/sys-project/server/src/lck"

#define SERVO_IOCTL_MAGIC 'M'
#define SERVO_IOCTL_WRITE _IOW(0, 1, unsigned char) // write data to device driver

#define LCD_RS 11
#define LCD_E 10
#define LCD_D4 6
#define LCD_D5 5
#define LCD_D6 4
#define LCD_D7 1

int lcd;
int cnt;

int fd[5], fail, fail_cnt;
const char password[] = "11211";	// password
const char btn_drv_path[5][20] = {BTN_DRV1_PATH, BTN_DRV2_PATH, BTN_DRV3_PATH, BTN_DRV4_PATH, SERVO_MOTER_DRV_PATH};
char buffer[100];
int buf_cnt;

pthread_t tid[5];
pthread_mutex_t mutex;

/****************************************************/
/*													*/
/*	produre: set_lock, set_unlock, is_locked		*/
/*													*/
/*	purpose: 										*/
/*		set_lock:	 make empty lock file			*/
/*		set_unlock:	 remove empty lock file			*/
/*		is_locked:	 check lock file is existed		*/
/*													*/
/****************************************************/
/* region lock */
void set_lock(){
	if(open(LOCK_FILE, O_CREAT, 0777) < 0){
		perror("Lock create failed");
		exit(-1);
	}
}

void set_unlock(){
	remove(LOCK_FILE);
}

int is_locked(){
	return access(LOCK_FILE, F_OK) == 0 ? 1 : 0;
}
/* unregion lock */

/****************************************************/
/*													*/
/*	produre: lcdWrite								*/
/*													*/
/*	purpose: Puts words after lcd Clear				*/
/*													*/
/****************************************************/
void lcdWrite(char buffer[]){
	lcdClear(lcd);
	lcdPuts(lcd, buffer);
}

void sigint(int signo){
	lcdWrite("Program down");
	exit(0);
}

/****************************************************/
/*													*/
/*	produre: prgram_init							*/
/*													*/
/*	purpose: initailize program						*/
/*													*/
/****************************************************/
void program_init(){
	signal(SIGINT, (void*)sigint);						// set signal SIGINT
	wiringPiSetup();
	if(lcd = lcdInit(2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0)) {
	    fprintf(stderr, "lcd init failed\n");
	    return;
	}

	lcdPosition(lcd, 0, 0);
	lcdClear(lcd);
	pthread_mutex_init(&mutex, NULL);
}

/****************************************************/
/*													*/
/*	produre: btn1									*/
/*													*/
/*	purpose: when push button1, lcd will put '1'	*/
/*													*/
/****************************************************/
void* btn1(void* arg){
	int on = 0, prev=0;
	while(1){
		int err = read(fd[0], &on, sizeof(int));
		if(err == 0){					//read fail
			perror("read1 failed\n");
			return NULL;
		}
		if(!prev && on && !fail){		// if prev is low and cur is high, it means button is pushed.
			if(!is_locked()){			// if door is unlock, push button doesn't response.
				lcdWrite("Safe is unlocked");
				continue;
			}
			printf("button1 is pushed!\n");
			buffer[buf_cnt++] = '1';
			lcdWrite(buffer);
		}
		else if(prev && !on && !fail){	// if prev is high, cur is low, it means button is released.
			printf("button1 is released!\n");
			usleep(1000);				// to fix problem that when button is released, the signal is rising.
		}
		prev = on;
	}
}

/****************************************************/
/*													*/
/*	produre: btn2									*/
/*													*/
/*	purpose: when push button2, lcd will put '2'	*/
/*													*/
/****************************************************/
void* btn2(void* arg){
	int on = 0, prev = 0;
	while(1){
        int err = read(fd[1], &on, sizeof(int));
        if(err == 0){									// read fail
            perror("read2 failed\n");
            return NULL;
        }
		if(!prev && on && !fail){						// same with btn1. button pushed.
			if(!is_locked()){							// if door is unlock, push button doesn't response.
				lcdWrite("Safe is unlocked");
				continue;
			}
			printf("button2 is pushed!\n");
			buffer[buf_cnt++] = '2';
			lcdWrite(buffer);
		}
		else if(prev && !on && !fail){					// same with btn1. button released.
			printf("button2 is released!\n");
			usleep(1000);								// same with btn1.
		}
		prev = on;
	}
}

/****************************************************/
/*													*/
/*	produre: btn3									*/
/*													*/
/*	purpose: when push button3, enter the password.	*/
/*													*/
/****************************************************/
void* btn3(void* arg){
	int on = 0, prev = 0, time = 5;

	char wrong[30] = {0}, wait[30]={0};
	while(1){
		int err = read(fd[2], &on, sizeof(int));
		if(err == 0){									// read fail
			perror("read3 failed\n");
			return NULL;
		}
		if(!prev && on){								// same with btn1. button pushed.
			if(!is_locked()){							//same with btn1. if locked, button doesn't response.
				lcdWrite("Safe is unlocked");
				continue;
			}
			printf("button3 is pushed\n");
			if(strcmp(buffer, password) == 0){			// compare with password...
				lcdWrite("Pass!");
				door_lock(DOOR_OPEN);					// if password is correct, door open
				fail_cnt=0;
				pthread_mutex_lock(&mutex);	
				set_unlock();							// marked door is unlocked.
				pthread_mutex_unlock(&mutex);
			}
			else{
				memset(wrong, 0, sizeof(wrong));
				if(++fail_cnt == 3){
					fail = 1;
					time = 5;							// timer initialize
					do{
						memset(wait, 0, sizeof(wait));
						sprintf(wait, "Wait %d seconds", time);
						lcdWrite(wait);
						sleep(1);
					}while(time-- != 1);

					fail_cnt=0;
					fail = 0;
					lcdWrite("Push Password");
				}
				else{
					sprintf(wrong, "Wrong %d Times", fail_cnt);
					lcdWrite(wrong);
				}
			}
			memset(buffer, 0, sizeof(buffer));
			buf_cnt = 0;
		}
		else if(prev && !on){							// same with btn1, button released
			printf("button3 is released!\n");
			usleep(1000);
		}
		prev = on;
	}
}

/****************************************************/
/*													*/
/*	produre: btn4									*/
/*													*/
/*	purpose: when push button4, delete the password	*/
/*													*/
/****************************************************/
void* btn4(void* arg){
	int on = 0, prev = 0;
	while(1){
		int err = read(fd[3], &on, sizeof(int));
		if(err == 0){									// read fail
			perror("read4 failed\n");
			return NULL;
		}
		if(!prev && on && !fail){						// same with btn1, button is pushed
			if(!is_locked()){
				lcdWrite("Safe is unlocked");
				continue;
			}
			printf("buton4 is pushed\n");
			if(buf_cnt != 0){
				buffer[--buf_cnt] = 0;					// delete the password
			}
			lcdWrite(buffer);
		}
		else if(prev && !on && !fail){
			printf("button4 is released!\n");
			usleep(1000);
		}
		prev = on;
	}
}

/****************************************************/
/*													*/
/*	produre: program_on								*/
/*													*/
/*	purpose: make threads and initailize			*/
/*													*/
/****************************************************/
int program_on(){
	pthread_mutex_lock(&mutex);
	set_lock();											// set lock
	pthread_mutex_unlock(&mutex);
	for(int i=0; i<5; i++){
		fd[i] = open(btn_drv_path[i], O_RDWR | O_RSYNC);	// open
		if(fd[i] < 0){
			fprintf(stderr, "%d open failed\n", i+1);
			exit(-1);
		}
	}
	
	lcdPuts(lcd, "Push Password");

	// make threads
	pthread_create(&tid[0], NULL, btn1, NULL);
	pthread_create(&tid[1], NULL, btn2, NULL);
	pthread_create(&tid[2], NULL, btn3, NULL);
	pthread_create(&tid[3], NULL, btn4, NULL);

	// join threads
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);
	pthread_join(tid[3], NULL);

	for(int i=0; i<5; i++){
		close(fd[i]);
	}
}
