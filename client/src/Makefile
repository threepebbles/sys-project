client_app: client.c
	gcc -o client_app client.c
gyro_app: gyro.c
	gcc -o gyro_app gyro.c -lwiringPi

all: client_app gyro_app

clean:
	rm client_app
	rm gyro_app