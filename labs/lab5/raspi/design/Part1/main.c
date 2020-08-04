/*
 *CMPE121/L Spring 2019
 *Nicholas Jannuzzi
 *Lab 5 Part 1: PWM control of LED using USB
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <libusb.h>
#include <wiringPi.h>
#define SCALE 4
#define LED_PIN 1

int main (int argc, char * argv[])
{
	//pointer to USB device data structure
	libusb_device_handle* dev;

	//no need for tx data in this part as we are no transmitting
	//changing it to size of 1 byte since its only working with 1 byte at a time
	char rx_data[1];
	//number of bytes received
	int rec_bytes;
	int return_val;


	//initialize USB library(LIBUSB)
	libusb_init(NULL);


	//Open USB device
	//Vendor ID = 0x04B4
	//Product ID = 0x8051
	dev = libusb_open_device_with_vid_pid(NULL, 0x04B4, 0x8051);

	
	if(dev == NULL)
	{
		perror("ERROR! Unable to locate device\n");
		return -1;
	}

	//Clear USB just to be sure we are not running anything from previous runs	
	if(libusb_reset_device(dev) != 0)
	{
		perror("Failed to reset device\n");	
		return -1;
	}	



	//sets the configuration of the USB device
	if(libusb_set_configuration(dev, 1) != 0)
	{
		perror("Failed to configure USB device!\n");
		return -1;
	}


	//Claim USB interface
	if(libusb_claim_interface(dev, 0) != 0)
	{
		perror("Failed to claim USB interface!\n");
		return -1;
	}

	uint8_t Pot = 0;
	wiringPiSetup();
	pinMode(LED_PIN, PWM_OUTPUT);
	printf("Setup Complete\n");
	for(;;)
	{
	//Perform transfer IN from device to host(PSoC -> Pi)
	
		return_val = libusb_bulk_transfer(dev,(0x01 | 0x80) ,rx_data, 1  , &rec_bytes, 0);
		if(return_val == 0)
		{
			//saturate and scale
			printf("%s\n",libusb_strerror(return_val));	
			if(rx_data[0] > 255)rx_data[0] = 255;
			if(rx_data[0] < 0)rx_data[0] = 0;
			Pot = rx_data[0]*254/173;
			printf("%d\n",Pot);

			pwmWrite(LED_PIN, Pot*SCALE);
			

		}
		else if(return_val < 0)
		{
			printf("%d\n",return_val);
			return -1;
		}

	}
	libusb_close(dev);
}
















































