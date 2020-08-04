 /*
 * CMPE121/L Spring 2019
 * Author: Nick Jannuzzi 5/26/19
 * Source file for communication protocol for Pi
 * Defines Functions for USBFS and I2C protocols
 */
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <fcntl.h>
 #include <unistd.h>
 #include <termios.h>
 #include <wiringPi.h>
 #include <wiringPiI2C.h>
 #include <errno.h>
 #include "comms.h"




//macro for the 7-bit I2C slave address of the PSoC
#define I2C_SLAVE_ADDR 0x53
#define USB_VENDOR_ID 	0x04B4
#define USB_PRODUCT_ID 	0x8051

//USB device variables(global)
libusb_device_handle* dev; //pointer to data structure USB device
char rx_data[64];//Transmit data block
char rx2_data[64];//receive data block
int rcvd2_bytes;//Bytes Received
int rcvd_bytes;//Bytes transmitted
int return_val;//determines success or failure
int return_val2;

//I2C device variables(global)
int fd;
//initialize USB communication 
//returns a 0 if successful, -1 if failure occurs 
//takes in a doule pointer to the USB device handle
int USB_Init(libusb_device_handle** dev)
{
    //libusb_device_handle* dev; // Pointer to data structure representing USB device
  
    libusb_init(NULL); // Initialize the LIBUSB library
  
    // Open the USB device (the Cypress device has
    // Vendor ID = 0x04B4 and Product ID = 0x8051)
	*dev = libusb_open_device_with_vid_pid(NULL, USB_VENDOR_ID, USB_PRODUCT_ID);
	
	//check if the device is available  
	if(*dev == NULL)
	{
		//error and exit if device cannot be found
		printf("ERROR: USB device not found\n");
		return -1;
	}else{
		
		//printf("USB device found\n");
	}
	//Reset the USB device
	//This step is not mandatory, but will clear residual states from previous run cycles
	if(libusb_reset_device(*dev) != 0)
	{
		//if reset fails, error and exit
		printf("%s\n",strerror(errno));
		printf("ERROR: Failed to reset the device\n");
		return -1;
	}else{
		//printf("USB device reset\n");
	}
	
	//Set configuration
	if(libusb_set_configuration(*dev, 1) != 0)
	{
		printf("%s\n",strerror(errno));
		printf("ERROR: Failed to set device configuration\n");
		return -1;
		
	}else{
		//printf("USB device config set\n");
	}
	
	//claim the USB interface
	//required before we can perform I/O to the device
	if(libusb_claim_interface(*dev,0) != 0)
	{
		printf("%s\n",strerror(errno));
		printf("ERROR: Cannot find USB device interface\n");
		return -1;
	}else{
		//printf("USB device interface claimed\n");
	}
	
	return 0;
} 




//initializes the I2S connection
//returns a 0 if successful, -1 if failure occurs at anypoint
//input is a pointer to the file descriptor of the I2C device
int I2C_Init(int *fd)
{
	
	//Initialize the I2C interface
	//Returns standard file descriptor
	*fd = wiringPiI2CSetup(I2C_SLAVE_ADDR);
	if(*fd == -1)
	{
		printf("ERROR: I2C Device not found\n");
		return -1;
	}else
	{
		//printf("I2C device found\n");
	}
	return 0;
	
}

//perform the IN transfer from the USB device to host
//takes in a double pointer to the device, the pointer to the array the data is stored in, and the endpoint of the channel we wish to connect to 
int USB_Read(libusb_device_handle** dev, char *rx_data, unsigned char EP)
{
	//uses bulk transfer function to read data in from device
	//printf("Reading EP %u:",EP);
		return_val = libusb_bulk_transfer(*dev,(EP | 0x80), rx_data, 64, &rcvd_bytes, 0);
		//printf("return_val%d\n",rx_data[0]);
		if(return_val == 0)
		{
			return 0;
			//testing purposes
			//printf("received: %d\n", rcvd_bytes);
		}	
			
}

//write data using I2C to PSoC
int I2C_Write(int *fd, int I2C_WriteVal)
{
		//uses wiringPi commands to write data
		wiringPiI2CWrite(*fd, I2C_WriteVal);
	    //a file descriptor of zero means it failed
		if(fd == 0)
		{
			printf("ERROR: I2C write failed\n");
			return -1;
		}else
		{
			return 0;
		}	
}


//read data from PSoC using I2C
int I2C_Read(int *fd)
{
		//int I2C_ReadData = 0;
		//uses wiringPi commands to read data
		//I2C_ReadData= wiringPiI2CRead(fd);
		//printf("Value: %d\n", wiringPiI2CRead(fd));
		return wiringPiI2CRead(*fd);	
		
	
}





