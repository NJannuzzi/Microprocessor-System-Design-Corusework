/*
 *CMPE121/L Spring 2019
 *Author: Nicholas Jannuzzi  
 *Graphics package for dual channel oscilloscope using OpenVG API  
 * 
 * 
 */ 
#ifndef graphics_H
#define graphics_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <shapes.h>
#include <fontinfo.h>
#include <math.h>
#include "cmdargs.h"

typedef struct{
	VGfloat x;
	VGfloat y;
}data_point;



//draws oscilloscope display background
//inputs: 
/* w: width of screen
 * h: height pf screen
 * xdiv: number of x divisions
 * ydiv: number of y divisions
 * margin: margin round the image
 */
void DrawBackground(int w, int h, int xdiv, int ydiv, int margin);


//draws oscilloscope grid for usage in the background
//inputs: 
/* w: width of screen
 * h: height pf screen
 * nx: number of x divisions
 * ny: number of y divisions
 * x: coordinates of lower left corner of screen
 * y: coordinates of lower left corner of screen
 */
void grid(VGfloat x, VGfloat y,int nx, int ny,int w, int h); 



//printfs the display segment containing oscilloscope settings
/*inputs:
 *xscale: x-axis scale
 *yscale: y-axis scale
 *x: x coordinate
 *y: y coordinate
 *tcolor: text color    
 */ 
void DrawSettings(CommandArgs *Commands, int xscale, int yscale, int x, int y, VGfloat tcolor[4]);



//process waveform smaples into screen coordinates
/*inputs;
 *data: pointer to data array containing the data 
 *sample_number: the number of samples 
 *xstart: the x-coordinate where plotting will start
 *xend: the x-coordinate where plotting will end
 *yscale: y-axis scaling factor
 *point_array the array containing the processed points    
 */ 
void SampleProcess(char *data, int sample_number, int xstart, int xend, float yscale, data_point *point_array);


//char *data, char *data2, int sample_number, int xstart, int xend, float yscale, data_point *point_array, data_point *point_array2
//char *data, int sample_number, int xstart, int xend, float yscale, data_point *point_array
//process the waveform data and converts it into the coordinates on the screen
/*inputs:
 * *data: the data to plot 
 *  sample_number: number of sample points
 *  offset: vertical offset defined by the potentiometers
 *  linecolor: determines color of the waveform 
 */ 
void DrawWaveForm(data_point *data, int sample_number, int offset, VGfloat linecolor[4]);

#endif
