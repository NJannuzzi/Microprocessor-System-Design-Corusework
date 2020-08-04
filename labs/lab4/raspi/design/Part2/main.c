//CMPE121 LAB4 PART2
//SINE WAVE TRANSMISION USING MANY METHODS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <wiringPi.h>
#include <fcntl.h>


#define BAUDRATE B115200
#define ERROR -1

int main(int argc, char * argv[])
{
	struct termios serial;
	char* dev_id = "/dev/serial0";
	char rxbuffer;//Receiving data FIFO buffer


	printf("Initializing %s\n",dev_id);

	int FileDesc = open(dev_id,O_RDWR | O_NOCTTY | O_NDELAY);
	
	//error checking things
	if(FileDesc == ERROR)
	{
		perror(dev_id);
		return -1;

	}

	//more error setup
	if(tcgetattr(FileDesc, &serial) < 0)
	{
		perror("Config Retrieval Error");
		return -1;

	}


	//UART parameters
	serial.c_iflag = 0;
	serial.c_oflag = 0;
	serial.c_lflag = 0;
	serial.c_cflag = BAUDRATE | CS8 | CREAD | PARENB | PARODD;
	//sets bauds, data size, parity and parity type
	

	serial.c_cc[VMIN] = 0;
	serial.c_cc[VTIME] = 0;
	//set to 0 for nonblocking code
	

	tcsetattr(FileDesc, TCSANOW, &serial);
	wiringPiSetup();

	//read string first
	int RX = 0;
	int TX = 0;
	for(;;)
	{
		//read the RX
		RX = read(FileDesc,&rxbuffer,1);
		if( RX < 0)
		{
			perror("Reading Error!");
			return -1;
		}
		//write to TX 
		TX = write(FileDesc, &rxbuffer,1);
		while(TX < 0)
		{
			//check if not a fatal error
			//if a fatal error, abort
			if(errno != EAGAIN)
			{
				perror("Transmission Write Error!");
				return -1;
				
			}
			//keep trying if its not that bad
			else
			{
				TX = write(FileDesc, &rxbuffer,1);
			}
		}



	}
	close(FileDesc);
}



