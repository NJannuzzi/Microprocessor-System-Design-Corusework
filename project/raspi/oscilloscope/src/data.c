/*
 *CMPE121/L Spring 2019
 *Nicholas Jannuzzi  
 *Data library handles detecting trigger conditions   
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
#include "comms.h"
#include "cmdargs.h"
#define POS 1
#define NEG 0
#define SCALE_DOWN 20
#define TRIGGERED 1
#define NOT_TRIGGERED 0
//sets trigger if selected
int settrigger(libusb_device_handle** dev, char* data, char* data2,CommandArgs *Commands,unsigned char EP,unsigned char EP2, int sample_number, char* triggerdata, char* triggerdata2)
{
	//array for running sum
	char runningsum [5];
	int weighted_sum_current;
	int weighted_sum_previous;
	int flag = NOT_TRIGGERED;
	int level = 0;
	//check for each channel and each slope, 4 combinations possible
		if(Commands->trigger_slope == POS)
		{
			level = Commands->trigger_level;
			//perform smoothing operation
			for(int x = 0; x < 64; x++)
			{
				//save data to running sum array		
				runningsum[0] = data[x];
				//take current moving average
				//set the previous sum to the current beforehand
				weighted_sum_previous = weighted_sum_current;
				weighted_sum_current = (runningsum[0] + 2*runningsum[1] + 3*runningsum[2] + 2*runningsum[3] + runningsum[4])/9;
				//positive trigger slope condition check
				if((runningsum[1] < level/SCALE_DOWN) && (runningsum[0] >= level/SCALE_DOWN) && (weighted_sum_current > weighted_sum_previous))
				{
						flag = TRIGGERED;
					
					
				}else
					{	//trigger conditions not satisfied
						flag = NOT_TRIGGERED;
						//shift data index up by one
						runningsum[1] = runningsum[0];
						runningsum[2] = runningsum[1];
						runningsum[3] = runningsum[2];
						runningsum[4] = runningsum[3];
						
				}
				//only happens if trigger is set
				if(flag == TRIGGERED)
				{
					int secondx = x;
					for(int i = 0; secondx < 64; secondx++,i++)
					{ 
						//save massive array index value as the previous x value
						triggerdata[i] = data[secondx]; 
					}
					for(int r = 0; r < sample_number; r+=64)
					{
						USB_Read(dev, data ,EP);
						USB_Read(dev, data2,EP2);
						//fill big array with data array + offset
						for(int o = 0; o <64; o++)
						{
							triggerdata[o+r] = data[o];
							triggerdata2[o+r] = data2[o];
						}
						
					}
				}
				
			}
			
			
		}else if(Commands->trigger_slope == NEG)
		{
			
			//perform smoothing operation
			for(int x = 0; x < 64; x++)
			{
				level = Commands->trigger_level;
				//save data to running sum array		
				runningsum[0] = data[x];
				runningsum[1] = data[x+1];
				runningsum[2] = data[x+2];
				runningsum[3] = data[x+3];
				runningsum[4] = data[x+4];
				//take current moving average
				//set the previous sum to the current beforehand
				weighted_sum_previous = weighted_sum_current;
				weighted_sum_current = (runningsum[0] + 2*runningsum[1] + 3*runningsum[2] + 2*runningsum[3] + runningsum[4])/9;
				//positive trigger slope condition check
				if((runningsum[1] > level/SCALE_DOWN) && (runningsum[0] <= level/SCALE_DOWN) && (weighted_sum_current < weighted_sum_previous))
				{
						flag = TRIGGERED;
					
					
				}else
				{	//trigger conditions not satisfied
						flag = NOT_TRIGGERED;
						//shift data index up by one
						runningsum[1] = runningsum[0];
						runningsum[2] = runningsum[1];
						runningsum[3] = runningsum[2];
						runningsum[4] = runningsum[3];
				}				
				//only happens if trigger is set
				if(flag == TRIGGERED)
				{
					int secondx = x;
					for(int i = 0; secondx < 64; secondx++,i++)
					{ 
						//save massive array index value as the previous value
						triggerdata[i] = data[secondx]; 
					}
					for(int r = 0; r < sample_number; r+=64)
					{
						USB_Read(dev, data ,EP);
						USB_Read(dev, data2 ,EP2);
						//fill big array with data array + offset
						for(int o = 0; o <64; o++)
						{
							triggerdata[o+r] = data[o];
							triggerdata2[o+r] = data2[o];
						}
						
					}
				}
				
			}
				
					
			}
		}
		
		
	
	

