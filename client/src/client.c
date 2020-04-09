#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "../include/myUds.h"
#include "../include/myLed.h"
#include "../include/myBuzzer.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 6789

const char close_msg[] = "close";
const char quit_msg[] = "quit";
static int client_socket;

static void* signal_handler(int signo);
int is_closed(double distance);

int main(int argc, char **argv) {
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char serverIp[20]={};
  struct hostent *server_host;

  signal(SIGINT, signal_handler);

  if (argc == 2) {
    strncpy(serverIp, argv[1], strlen(argv[1]));
  } 
  else {
    printf("usage: ./client_app  [server ip address]\n");
    exit(-1);
  }
  printf("Connecting to %s:%d...\n", serverIp, PORT);

  client_socket = socket(AF_INET, SOCK_STREAM, 0);  // make socket descriptor
  if (client_socket == -1) {
    perror("socket() error!\n");
    exit(-1);
  }
  server_host = gethostbyname(serverIp);
  if(server_host==NULL) {
    perror("gethostbyname failed\n");
    exit(-1);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;                     // use IPv4
  serv_addr.sin_addr.s_addr = inet_addr(serverIp);    // server address
  serv_addr.sin_port = htons(PORT);                   // server port to connect

  while(1) {
    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {    // try to connect to server
        printf("connect() failed\n");
        sleep(2); continue;
    }
    printf("[CLIENT] connedted to %s:%d!\n", serverIp, PORT);
    
    printf("[CLIENT] Checking if door is opened...\n");
    while(1) {
      double dis = 0;
      /* Step 1: ultrasonic distance sensor check if the door is opened or not */
      while(is_closed(dis = uds_get_distance())) { // check every 1 second
        printf("door is closed\n");
        usleep(1000000);  // polling every 1 second
      }
      printf("door is opened!\n");

      /* Step 2: As the door is opened, the LED is on */
      led_on();

      /* Step 3: if the door is closed for 2 seconds, it is locked automaticcaly */
      int second_count = 2;
      while(second_count > 0) { // check every 1 second
        usleep(1000000);  // sleep 1 second
        dis = uds_get_distance();
        
        if(!is_closed(dis))   // if door is opend, reset count
          second_count = 2;
        else second_count--;  // if door is closed, decrease count

        if(second_count==0) break;  // door is closed for 2 seconds
      }
      led_off();

      /* Step 4: Tell the server to close the door */
      send(client_socket, close_msg, strlen(close_msg), 0);
      sleep(1); // wait during server is closing the door
    }
  }

  return 0;
}

int is_closed(double distance) {
  if(2.0<=distance && distance<=4.0) return 1;
  return 0;
}

// When client_app is suddenly terminated by ctrl+C, signal_handler is called
static void* signal_handler(int signo) {
  if(signo==SIGINT) {
    printf("SIGINT occured!\n");
    send(client_socket, quit_msg, strlen(quit_msg), 0); // send quit message to server to disconnect
    exit(0);
  }
}