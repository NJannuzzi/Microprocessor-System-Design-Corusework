//*****************************************************************************
//CMPE121/L Spring 2019
//Lab 5 Part 2: Reconstruct 100Hz sine wav using USB
// Nick Jannuzzi
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <wiringPi.h>
#include <errno.h>
#include <libusb.h>

int main (int argc, char * argv[]){

    libusb_device_handle* dev; // Pointer to data structure representing USB device
  
    char tx_data[64]; // Transmit data block
    char rx_data[64]; // Receive data block
    int sent_bytes; // Bytes transmitted
    int rcvd_bytes; // Bytes received
    int return_val;
  
    libusb_init(NULL); // Initialize the LIBUSB library
  
    // Open the USB device (the Cypress device has
    // Vendor ID = 0x04B4 and Product ID = 0x8051)
    dev = libusb_open_device_with_vid_pid(NULL, 0x04B4, 0x8051);
  
    if (dev == NULL){
      perror("device not found\n");
      return -1;
    }
  
    // Reset the USB device.
    // This step is not always needed, but clears any residual state from
    // previous invocations of the program.
    if (libusb_reset_device(dev) != 0){
      perror("Device reset failed\n");
      return -1;
    } 
  
    // Set configuration of USB device
    if (libusb_set_configuration(dev, 1) != 0){
      perror("Set configuration failed\n");
      return -1;
    } 
  
  
    // Claim the interface.  This step is needed before any I/Os can be
    // issued to the USB device.
    if (libusb_claim_interface(dev, 0) !=0){
        perror("Cannot find interface");
	return -1;
      }
  
  
  
  
   for(;;)
   { 
    // Perform the IN transfer (from device to host).
    // Read data from device
      return_val = libusb_bulk_transfer(dev,(0x01 | 0x80),rx_data,64,&rcvd_bytes,0);
      printf("%d\n",rx_data[0]);
      if (return_val == 0){
         // Perform the OUT transfer (from host back to device).
        return_val = libusb_bulk_transfer(dev, 0x02,rx_data,64, &sent_bytes,0);
        if (return_val == 0){}
        else{
          perror("WRITE BROKE");
	  return -1;
        }
      }else{
          perror("READ BROKE");
	  return -1;
      }
      printf("%s\n",strerror(return_val));
  }
  libusb_close(dev);
}




