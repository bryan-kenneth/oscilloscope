/*//////////////////////////////////////////////////////////////////////
File: main.c
 
Description: Main initialization and loop for RPI PSoC Oscilloscope
*///////////////////////////////////////////////////////////////////////

#include "scope.h"

int main (int argc, char *argv[]) {
	// Parse command line arguments
	cmdargs ca = parseArgs(argc, argv);
	printArgs(ca);
	
	// Initialize communications
	usbInit();
	i2cInit(ca.samp_rate);
	
	// x4 Oversample for data buffers
	int nsamples = 4*ca.sps;
	char ch1_data[nsamples], ch2_data[nsamples];
	
	// Initialize screen
	windowInit(ca);
	
	while(true) {
		// Clear window, draw background, print settings
		windowReset(ca);
		
		// Fill channel data buffers
		usbReceive(ch1_data, ch2_data, nsamples);
		
		// Handles trigger, processes, and plots
		processAndPlot(ca, ch1_data, ch2_data);
	}
}
