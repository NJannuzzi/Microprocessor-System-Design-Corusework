#ifndef cmdargs_H
#define cmdargs_H
/*
 * CMPE121/L Spring 2019
 * Nick Jannuzzi 5/26/19
 * Header file for command line parser for Pi
 * Defines Functions prototype for parsing command arguments
 */
 
/* Command arguments for oscillosocpe
 * -m <mode>: Sets the mode, can be either free or trigger
 * free sets scope to free run mode, displaying a free running signal that may not b stable
 * trigger mode aligns sampled signal using a trigger level and slope, which will stabilize repeating waveforms
 * default is trigger mode
 * 
 * -t <trigger-level>: Specifies trigger level in millivolts, ranges from 0 to 5000 in 100mv steps. 
 * Default is 2500 (2.5V)
 * 
 * 
 * -s <trigger-slope>: Sets slope fo the trigger to either pos(positive) or neg(negative).
 * Indicates whether the time sweep of the waveform should start on a rising edge or falling edge when the signal crosses trigger level
 * 
 * -r <sample-rate>: Defines sampling rate for ADCs of the two channels, measured in kilosamples/second
 * Can be 1,10,20,50 or 100 kilosamples/sec
 * Default is 1 kilosamples/second
 * 
 * 
 * -c <trigger-channel>: determines the channel that is the source of the trigger (1 or 2)
 * Channel 1 is the defult tirgger channel
 * 
 * -x <xscale>: defines horizontal scale of waveform display in microseconds
 * Value can be 100, 500, 1000, 2000, 5000, 10000, 50000, or 100000. 
 * Default value is 1000(1 millisecond/division)
 * 
 * -y <yscale>: defines vertical scale of waveform display in millivolts
 * Value can be 100, 500, 1000, 2000, or 2500. 
 * Default value is 1000(1V/division)
 */ 


/*
 *Command line parser returns a struct containing the data parsed 
 */
typedef struct CommandArgs 
{
	//scope mode
	int mode; /* -m option */
	//trigger level
	int trigger_level; /* -t option */
	//trigger slope
	int trigger_slope; /* -s option */
	//sample rate
	int sample_rate; /* -r option */
	//trigger channel
	int trigger_channel; /* -c option */
	//X divsion scale
	int Xscale; /* -x option */
	//Y division scale
	int Yscale; /* -y option */
}CommandArgs;

//struct CommandArgObj* CommandArgs;

int DefaultVal(CommandArgs *Commands, int argc, char *argv[]);


/*@Function Parse Arguments
 *@parameters the inputs mentioned above
 *@return the struct containing the parsed data   
 */ 
int ParseArgs(CommandArgs *Commands,int argc,char *argv[]);
	
#endif
