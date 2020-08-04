/*
 *CMPE121/L Spring 2019
 *Nick Jannuzzi
 *Main control loop file for final lab project
 *Utilizes several libraries to implement a dual channel oscilloscope whose parameters are configurable via command line arguments    
 *For documentation on how to enter the command line parameters, please see cmdargs.h/.c for details 
 *Samples data from pins and potentiometers on the PSoC before transferring data to the Pi for handling & display to graphical interface
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
#include <unistd.h>
#include <shapes.h>
#include <fontinfo.h>
#include <math.h>
#include <libusb.h>
//library functions for other parts
#include "data.h"
#include "cmdargs.h"
#include "comms.h"
#include "graphics.h"



#define USB_VENDOR_ID 0x04B4
#define USB_PRODUCT_ID 0x8051
#define USB_EP1 0x01
#define USB_EP2 0x03
#define BUFF_EXTEND 192
//initialize serial channels and obtain inputs
int main(int argc, char * argv[])
{
	
	//initialize the struct for the command args
	CommandArgs Commands;
	//Initializes the arguments struct with the default values
	DefaultVal(&Commands, argc, argv);
	//parse the command line
	ParseArgs(&Commands, argc, argv);
	
	//definitions for gaphics
	int xscale = Commands.Xscale;
	int yscale = Commands.Yscale;
	int height;
	int width;
	int margin = 10;
	int xdivisions = 10;
	int ydivisions = 8;
	int xstart = margin;
	int xlimit = width - 2*margin;
	int ystart = margin;
	int ylimit = height - 2*margin;
	int pixels_per_volt;
	//convert to get smaples/frame
	int sample_num =  xscale*Commands.sample_rate/100 ;
	//(1000*height)/(ydivisions * yscale);
	
	//data sample arrays
	char chan1_data[sample_num + BUFF_EXTEND];
	char chan2_data[sample_num + BUFF_EXTEND];
	
	//data point arrays
	data_point chan1_points[sample_num + BUFF_EXTEND];
	data_point chan2_points[sample_num + BUFF_EXTEND];	
	
	VGfloat textcolor[4] = {0,227, 255, 3.0};
	VGfloat Chan1color[4] = {255, 0, 255, 3.0};
	VGfloat Chan2color[4] = {255, 255, 0, 3.0};
	//usb device info
	libusb_device_handle* dev = NULL; //pointer to data structure USB device	
	char USB1_data[64] = {0};//Transmit data block
	char USB2_data[64] = {0};//receive data block
	int rcvd2_bytes;//Bytes Received
	int rcvd_bytes;//Bytes transmitted
	int return_val;//determines success or failure
	int return_val2;
	//I2C device info
	int fd;
	
	
	
	
	
	
	//Initialize I2C connection, check if connection established
	if(I2C_Init(&fd) != 0)
	{
		printf("failed");
		return -1;
		
	}else
	{
		//printf("I2C connected\n");
	}

	
	//Initialize USB connection, check if connection established
	if(USB_Init(&dev) != 0)
	{
		printf("Failed to establish USB connection\n");
		return -1;
	}else
	{
		
		//printf("USB connected\n");
	}
	
	//graphical display setup
	saveterm(); // Save current screen
	init(&width, &height); // Initialize display and get width and height
	rawterm(); // Needed to receive control characters from keyboard, such as ESC
	//send sampling rate to the PSoC
	
	int temp1;
	int temp2;
	int Offset1 = 0;
	int Offset2 = 0;	

    int Frc = 0;
	int endchar = 0x1b;
	I2C_Write(&fd, Commands.sample_rate);
	
	//main loop for free mode
	if(Commands.mode ==1)
	{
		for(;;)
		{
			 int key;
	
		    
	        key = getchar();
	        if (key == endchar || key == '\n') {
	            break;
	        }
	        
	        
	        
			//sample potentiometers
			temp1 = I2C_Read(&fd);
			temp2 = I2C_Read(&fd);
			if(temp1 !=255)Offset1 = temp1;
			if(temp2 !=255)Offset2 = temp2;
			printf("Offset1: %d\n",Offset1);
			printf("Offset2: %d\n",Offset2);
			//process into larger array
			for(Frc = 0; Frc < sample_num + BUFF_EXTEND; Frc+=64)
			{
			    
			    USB_Read(&dev,USB1_data, USB_EP1);
				USB_Read(&dev,USB2_data, USB_EP2);
				for(int x = 0; x < 64; x++)
				{
					//load data into large array by 64 byte chunks
					chan1_data[Frc+x] = USB1_data[x];
					
					chan2_data[Frc+x] = USB2_data[x]; 
					//printf("%d\n", chan1_data[Frc+x]);
				}

			}
			
			for (int i = 0; i < sample_num; i++) {
				printf("%d\n",USB2_data[i]);
			}
			Start(width, height);
			pixels_per_volt = (1000*height)/(ydivisions * yscale);
			//process samples and draw screen
			
	 
			//draw background and grid lines
			DrawBackground(width,height,xdivisions,ydivisions,margin);
			DrawSettings(&Commands, xscale, yscale, width-300, height-50, textcolor);		
	        SampleProcess(&chan1_data[64],sample_num, margin, width - 2*margin, pixels_per_volt, chan1_points);
			SampleProcess(&chan2_data[64],sample_num, margin, width - 2*margin, pixels_per_volt, chan2_points);
			DrawWaveForm(chan1_points, sample_num, Offset1 + 300,  Chan1color);
	        DrawWaveForm(chan2_points, sample_num, Offset2,  Chan2color); 
			
			End();
		    USB_Read(&dev,USB1_data, USB_EP1);
		    USB_Read(&dev,USB2_data, USB_EP2);
			//~ USB_Read(&dev,USB1_data, USB_EP1, USB2_data, USB_EP2);
			
		}
		restoreterm();
		finish();
	}
	//main loop for trigger mode
	if(Commands.mode == 0)
	{
		for(;;)
		{
			USB_Read(&dev,USB1_data, USB_EP1);
			USB_Read(&dev,USB2_data, USB_EP2);
			//which call happens depends upon the trigger channel
			if(Commands.trigger_channel == 1)
			{
				//execute plotting loop but calculate trigger
				Start(width, height);
				pixels_per_volt = (1000*height)/(ydivisions * yscale);
				//call trigger function to calcualte data array		
				temp1 = I2C_Read(&fd);
				temp2 = I2C_Read(&fd);
				if(temp1 !=255)Offset1 = temp1;
				if(temp2 !=255)Offset2 = temp2;
				printf("Offset1: %d\n",Offset1);
				printf("Offset2: %d\n",Offset2);
				settrigger(&dev, USB1_data, USB2_data,&Commands,USB_EP1, USB_EP2,sample_num, chan1_data, chan2_data);
				DrawBackground(width,height,xdivisions,ydivisions,margin);
				DrawSettings(&Commands, xscale, yscale, width-300, height-50, textcolor);					
		        SampleProcess(&chan1_data[64],sample_num, margin, width - 2*margin, pixels_per_volt, chan1_points);
		        SampleProcess(&chan2_data[64],sample_num, margin, width - 2*margin, pixels_per_volt, chan2_points);
		        DrawWaveForm(chan1_points, sample_num, 500+Offset1,  Chan1color);	
		        DrawWaveForm(chan2_points, sample_num, 300+Offset2,  Chan2color); 					
				End();	
				USB_Read(&dev,USB1_data, USB_EP1);
				USB_Read(&dev,USB2_data, USB_EP2);
			}else
			{
				//execute plotting loop but calculate trigger
				Start(width, height);
				pixels_per_volt = (1000*height)/(ydivisions * yscale);	
				//call trigger function to calcualte data array		
				temp1 = I2C_Read(&fd);
				temp2 = I2C_Read(&fd);
				if(temp1 !=255)Offset1 = temp1;
				if(temp2 !=255)Offset2 = temp2;
				printf("Offset1: %d\n",Offset1);
				printf("Offset2: %d\n",Offset2);
				settrigger(&dev, USB1_data, USB2_data,&Commands,USB_EP1, USB_EP2,sample_num, chan1_data, chan2_data);
				DrawBackground(width,height,xdivisions,ydivisions,margin);
				DrawSettings(&Commands, xscale, yscale, width-300, height-50, textcolor);					
		        SampleProcess(&chan1_data[64],sample_num, margin, width - 2*margin, pixels_per_volt, chan1_points);
		        SampleProcess(&chan2_data[64],sample_num, margin, width - 2*margin, pixels_per_volt, chan2_points);
		        DrawWaveForm(chan1_points, sample_num, 500+Offset1,  Chan1color); 
		        DrawWaveForm(chan2_points, sample_num, 300+Offset2,  Chan2color); 		        		
		        End();
				USB_Read(&dev,USB1_data, USB_EP1);
				USB_Read(&dev,USB2_data, USB_EP2);		         
		       
			}
		}
	}
	
	
	
	
	return 0;
}
//printf("tarnation: %d\n", USB1_data[i]);
