
//Lab4 Part1
//Nicholas Jannuzzi
//CMPE121/L Spring 2019
//Control LED using PWM and UART transfer between PSoC 5LP and Raspberry
//Pi B+


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define LED_PIN 1
#define BAUDRATE B115200
#define SCALING 4
#define RXMAX 1023


int main (int argc, char * argv[]){

	//termios struct that contains UART constraints
	struct termios serial;
	char* dev_id = "/dev/serial0";//Device ID for UART
	char  rxbuffer;//Receiving data FIFO buffer


	//open 
	printf("Initializing\n");

	int FileDesc = open(dev_id, O_RDWR | O_NOCTTY | O_NDELAY);


	if(FileDesc == -1)
	{
	       perror(dev_id);
	       return -1;	

	}
	
	// Get UART config
	if(tcgetattr(FileDesc, &serial) < 0)
	{
	   perror("Configuration Error!");
	   return -1;
	}	


		
	//Parameters for termios structure setup
	serial.c_iflag = 0;
	serial.c_oflag = 0;
	serial.c_lflag = 0;
	serial.c_cflag = BAUDRATE | CS8 | CREAD | PARENB | PARODD;
	//sets up baudrate, data length, RX enabled, odd parity enabled
	
	//set to nonblocking code value of 0
	serial.c_cc[VMIN] = 0;
	serial.c_cc[VTIME] = 0;	

	//set parameters
	tcsetattr(FileDesc, TCSANOW, &serial);
	
	wiringPiSetup();
	pinMode(LED_PIN, PWM_OUTPUT);

//	int RC = 0;
	//constant loop to write to pwm after reading RX data buffer value
	for(;;)
	{
		//read rxbuffer, store to char, dereference the char
		int RX = read(FileDesc, &rxbuffer, 1);
		if (RX < 0)
		{
			perror("Reading Error! ");
			return -1;
		}
		else if(RX != 0)
		{
		//number mapping to get full range on led
		int pot = rxbuffer;
		pot = pot*255/145;; 
		//scale and write to pin
		pwmWrite(LED_PIN,pot * SCALING);
		printf("Pot Value: %d\n", pot);
		}

	}

	close(FileDesc);

}






