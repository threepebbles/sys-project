#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "../include/myPressure.h"
#include "../include/myBuzzer.h"


void alarm_sigint(int signo){
    buzzer_off();
    exit(0);
}

int main(){
	signal(SIGINT, (void*)alarm_sigint);
    while(1){
		if(is_locked()){
			printf("locked!\n");
		}
		else{
			printf("unlocked\n");
		}
        if(is_pushed() && is_locked()){
            buzzer_on();
        }
        else{
            buzzer_off();
        }
		sleep(1);
	}
    return 0;
}
