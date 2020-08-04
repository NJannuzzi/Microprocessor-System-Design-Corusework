/*
 * CMPE121/L Spring 2019
 * Nick Jannuzzi 5/26/19
 * Source file for command line parser for Pi
 * Defines Functions for parsing command arguments into struct
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cmdargs.h"


#define free 1
#define trigger -1
#define pos 1
#define neg -1


//enumerates for string inputs
enum Scopemode{freemode = 1,triggermode = 0};
enum TriggerSlope{positive = 1, negative = 0};


int DefaultVal(CommandArgs *Commands, int argc, char *argv[])
{
 
	//initializes to default values as specified by the manual
	Commands->mode = free;
	Commands->trigger_level = 2500;
	Commands->trigger_slope = pos;
	Commands->sample_rate = 1;
	Commands->trigger_channel = 1;
	Commands->Xscale = 1000;
	Commands->Yscale = 1000;
	
}



/*ParseArgs using getopt() function 
 *Returns the struct containing the parsed data 
 */ 
int ParseArgs(CommandArgs *Commands, int argc,char *argv[])
{
	//initilaize the struct for the arguments
	//initialized to default values as specified by the project manual
	//struct CommandArgs *Commands = newCmdargs();
	int opt;
	while((opt = getopt(argc, argv, ":m:t:s:r:c:x:y:")) != -1)
	{
		switch(opt)
		{
		
			//handles parsing for the mode input
			case 'm':
			
			if(strcmp(optarg, "free") == 0)
			{
				Commands->mode = freemode;
				break;
			}else if(strcmp(optarg, "trigger") == 0)
			{
				Commands->mode = triggermode;
				break;
			}else
			{
				printf("Invalid input, Input for scope mode must be either free or trigger!\n");
				exit(1); 
			}
			
				
			//handles parsing for trigger level
			case 't':
			Commands->trigger_level = atoi(optarg);
			if((Commands->trigger_level  > 5000) || (Commands->trigger_level  < 0) || 
			(Commands->trigger_level  % 100 != 0))
			{
				printf("Invalid input, Input for trigger level must be between 0 & 5000 in increments of 100!\n");
				exit(1);
			}else
			{
				
				break;			
			}
			
			//handles parsing for trigger slope
			case 's':

			if(strcmp(optarg, "pos") == 0)
			{
				Commands->trigger_slope = positive;
				break;				

			}else if(strcmp(optarg, "neg") == 0)
			{
				Commands->trigger_slope = negative;
				break;
			}else
			{
				printf("Invalid input, Input for trigger slope must be either pos or neg!\n");
				exit(1);	
			}
			
			//handles parsing sample rate
			case 'r':
			Commands->sample_rate = atoi(optarg);
			if((Commands->sample_rate  == 1) || (Commands->sample_rate  == 10) || 
			(Commands->sample_rate == 20) || (Commands->sample_rate== 50) || 
			(Commands->sample_rate  == 100))
			{
				break;
			}
			else
			{
				printf("Invalid input, Input for sample rate must be either 1,10,20,50,or 100!\n");
				exit(1);
				
				
			}
			
			//handles parsing trigger channel
			case 'c':
			Commands->trigger_channel = atoi(optarg);
			if(Commands->trigger_channel == 1 || Commands->trigger_channel  == 2)
			{
				
				break;

			}else
			{
				printf("Invalid argument, trigger channel must be either 1 or 2!\n");
				exit(1);
			}
			
			
			//handles parsing for xscale value
			case 'x':
			Commands->Xscale = atoi(optarg);
			if((Commands->Xscale  == 100) || (Commands->Xscale  == 500) 
			|| (Commands->Xscale  == 1000) || (Commands->Xscale  == 2000) 
			|| (Commands->Xscale == 5000) ||(Commands->Xscale  == 10000) || 
			(Commands->Xscale == 50000) || (Commands->Xscale  == 100000))
			{
				
				break;

				
			}else
			{
				printf("Invalid argument, x-axis scaling must be one of these values: 100,500,1000,2000,5000,10000,50000,100000!\n");
				exit(1);	
			}
			
			
			//handles parsing for yscale value
			case 'y':
			Commands->Yscale = atoi(optarg);
			if((Commands->Yscale == 100) || (Commands->Yscale  == 500) || 
			(Commands->Yscale  == 1000) || (Commands->Yscale == 2000) || 
			(Commands->Yscale  == 2500))
			{
				
				break;
			}else
			{
				printf("Invalid argument, y-axis scaling must be one of these values: 100,500,1000,2000,2500!\n");
				exit(1);
			}
			//handles extra argument types
			case '?':
					printf("unknown input option: %c\n",optopt);
					break;
					
			
			
		}
		
	}
	//prints the additional inputs for unknown argument types
		for(; optind <argc; optind++)
		{
				printf("Extra Arguments: %s\n", argv[optind]);
		}	
		//prints arguments(for testing purposes)
		printf("--------\n");
		if(Commands->mode == freemode)
		{
				printf("Mode:free\n");
		}else
		{
				printf("Mode:trigger\n");
		}	
		if(Commands->trigger_slope == positive)
		{
				printf("Trigger Slope:pos\n");
		}else
		{	
				printf("Trigger Slope:neg\n");
		}
		printf("Trigger Level:%d\n", Commands->trigger_level);
		printf("Trigger Channel:%d\n", Commands->trigger_channel);
		printf("Sample Rate:%d\n", Commands->sample_rate);
		printf("X-axis Scale:%d\n", Commands->Xscale);
		printf("Y-axis Scale:%d\n--------\n", Commands->Yscale);

		
		
	
	
	
}
