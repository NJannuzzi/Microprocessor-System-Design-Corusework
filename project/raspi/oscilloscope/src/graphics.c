/*
 *CMPE121/L Spring 2019
 *Author: Nicholas Jannuzzi  
 *Graphics package for dual channel oscilloscope using OpenVG API  
 * 
 * 
 */ 
 
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <shapes.h>
#include <fontinfo.h>
#include <math.h>
#include "graphics.h"
//draws oscilloscope display background
//inputs: 
/* w: width of screen
 * h: height pf screen
 * xdiv: number of x divisions
 * ydiv: number of y divisions
 * margin: margin round the image
 */
void DrawBackground(int w, int h, int xdiv, int ydiv, int margin)
{
	VGfloat x1 = margin;
	VGfloat y1 = margin;
	VGfloat x2 = w - 2*margin;
	VGfloat y2 = h - 2*margin;

	  Background(128, 128, 128);

	  Stroke(255, 255, 255, 1);
	  StrokeWidth(1);
  
	  Fill(0, 0, 0, 1);
	  Rect(10, 10, w-20, h-20); // Draw framing rectangle

	  grid(x1, y1, xdiv, ydiv, x2, y2); // Draw grid lines
}


void grid(VGfloat x, VGfloat y,int nx, int ny,int w, int h) 
{
  VGfloat ix, iy;
  Stroke(128, 128, 128, 0.5); // Set color
  StrokeWidth(2); // Set width of lines
  for (ix = x; ix <= x + w; ix += w/nx)
  {
    Line(ix, y, ix, y + h);
  }

  for (iy = y; iy <= y + h; iy += h/ny) 
  {
    Line(x, iy, x + w, iy);
  }
}

	
//prints the display segment containing oscilloscope settings
/*inputs:
 *xscale: x-axis scale
 *yscale: y-axis scale
 *x: x coordinate
 *y: y coordinate
 *tcolor: text color    
 */ 
void DrawSettings(CommandArgs *Commands, int xscale, int yscale, int x, int y, VGfloat tcolor[4])
{ 
	//string to store text in for printing
	char string[100];
	setfill(tcolor);
	//printfs the x scale
	if (Commands->Xscale >= 1000)
	{
	    sprintf(string, "X scale = %0d ms/division", Commands->Xscale/1000);
	}else
	{
		sprintf(string, "X scale = %0d us/division", Commands->Xscale);  
	}
	//prints the y scale
	Text(x -50, y, string, SansTypeface, 18);	    
	sprintf(string, "Y scale = %3.2f V/division", Commands->Yscale * 0.001);
	Text(x - 50, y - 50, string, SansTypeface, 18);
	//prints run mode and trigger slope if needed
	if (Commands->mode == 1)
	{
		sprintf(string,"Mode: Free");
	}
	else
	{
		sprintf(string,"Mode: Trigger");
	}
	Text(x - 50, y - 100, string, SansTypeface, 18);
	if(Commands->mode == 0)
	{
		//prints trigger channel
		sprintf(string, "Trigger Channel: %d", Commands->trigger_channel);
		Text(x - 50, y - 200, string, SansTypeface, 18);
		if(Commands->trigger_slope == 1)
		{
			sprintf(string,"Trigger Slope: Positive");
		}else
		{
			sprintf(string,"Trigger Slope: Negative");
		}
		Text(x - 50, y - 250, string, SansTypeface, 18);
		//prints trigger level
		sprintf(string, "Trigger Level: %3.1fV", Commands->trigger_level * 0.001);
		Text(x - 50, y - 300, string, SansTypeface, 18);

		
		
	}
	//prints sample rate
	sprintf(string, "Sample Rate: %dKs/second", Commands->sample_rate);
	Text(x - 50, y - 150, string, SansTypeface, 18);
}	


//process waveform smaples into screen coordinates
/*inputs;
 *data: pointer to data array containing the data 
 *sample_number: the number of samples 
 *xstart: the x-coordinate where plotting will start
 *xend: the x-coordinate where plotting will end
 *yscale: y-axis scaling factor
 *point_array the array containing the processed points    
 */ 
void SampleProcess(char *data, int sample_number, int xstart, int xend, float yscale, data_point *point_array)
{
	VGfloat x1, y1;
	//~ VGfloat x2, y2;
	data_point point;
	//~ data_point point2;
	//for loop to iterate and convert data accordingly
	for(int i = 0; i < sample_number; i++)
	{
		x1 = xstart + (xend -xstart)*i/sample_number;
		y1 = data[i] * 5 * yscale/256;
		point.x = x1;
		point.y = y1;
		point_array[i] = point;

		//~ x2 = xstart + (xend -xstart)*i/sample_number;
		//~ y2 = data[i] * 5 * yscale/256;
		//~ point2.x = x2;
		//~ point2.y = y2;
		//~ point_array2[i] = point2;

	}	
}



//process the waveform data and converts it into the coordinates on the screen
/*inputs:
 * *data: the data to plot 
 *  sample_number: number of sample points
 *  offset: vertical offset defined by the potentiometers
 *  linecolor: determines color of the waveform 
 */
void DrawWaveForm(data_point *data, int sample_number, int offset, VGfloat linecolor[4])
{
	data_point point;
	VGfloat x1;
	VGfloat x2;
	VGfloat y1;
	VGfloat y2;
	
	Stroke(linecolor[0], linecolor[1], linecolor[2], linecolor[3]);
	StrokeWidth(4);
	x1 = point.x;
	y1 = point.y;
	
	
	
	//for loop to set points for plotting
	for(int k = 0; k<sample_number; k++)
	{
		point = data[k];
		x2 = point.x;
		y2 = point.y + offset;
		Line(x1,y1, x2, y2);
		x1 = x2;
		y1 =y2;
	}
		
		
}
	



