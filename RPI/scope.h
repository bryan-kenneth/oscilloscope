/*//////////////////////////////////////////////////////////////////////
File: scope.h
 
Description: Header file for RPI PSoC Oscilloscope
*///////////////////////////////////////////////////////////////////////

#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <shapes.h>

#define FREE 	false
#define TRIGGER true
#define NEG false
#define POS true
#define xdiv 12
#define ydiv 10

// Data Structures
typedef struct {
	bool mode;
	int trig_level;
	bool trig_slope;
	int samp_rate;
	int trig_ch;
	int xscale;
	int yscale;
	int sps; // samples per second
	
} cmdargs;

typedef struct {
	VGfloat x;
	VGfloat y;
} data_point;

/***** Function Prototypes *****/

// usbcomm.c
void usbInit(void);
void usbReceive(char *ch1_data, char *ch2_data, int length);

// i2ccomm.c
void i2cInit(int sample_rate);
void i2cReceive (int *p1, int *p2);

// cmdargs.c
cmdargs parseArgs(int argc, char *const *argv);
void printArgs(cmdargs ca);

// graphics.c
void windowInit(cmdargs ca);
void windowReset(cmdargs ca);
void processSamples(char *data, data_point *point_array, int count);
void plotWave(data_point *data, int sps, int yoff, int ch);
void plotWavePoly(data_point *data, int sps, int yoff, int ch);

// data.c
void processAndPlot(cmdargs ca, char *ch1_data, char *ch2_data);

#endif
