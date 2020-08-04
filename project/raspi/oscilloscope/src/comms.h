/*
 * CMPE121/L Spring 2019
 * Nick Jannuzzi 5/26/19
 * Header file for communication protocol for Pi
 * Defines Functions prototypes for USBFS and I2C protocols
 */
#ifndef comms_H
#define  comms_H 
#include <libusb.h>
 /*
  *Function Prototypes for communication protocols 
  */ 
  
//initialize USBFS Connection  
int USB_Init(libusb_device_handle** dev);


//initialize I2C Connection
int I2C_Init(int *fd);


//Read from USB Connection
int USB_Read(libusb_device_handle** dev, char *rx_data, unsigned char EP);


//Write to I2C Connection
int I2C_Write(int *fd, int I2C_WriteVal);

//Read from I2C Connection
int I2C_Read(int *fd);



#endif
