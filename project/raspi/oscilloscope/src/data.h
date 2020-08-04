/*
 *CMPE121/L Spring 2019
 *Nicholas Jannuzzi  
 *Data library handles detecting trigger conditions   
 */ 
#ifndef data_H
#define data_H
#include "cmdargs.h"

//sets the trigger if activated
/*input: data- input data
 * Commands: parser info 
 * triggerdata:output array
 * 
 */ 
int settrigger(libusb_device_handle** dev, char* data, char* data2,CommandArgs *Commands,unsigned char EP,unsigned char EP2, int sample_number, char* triggerdata, char* triggerdata2);


#endif
