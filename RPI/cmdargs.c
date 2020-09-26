/*//////////////////////////////////////////////////////////////////////
File: cmdargs.c
 
Description: Functions for parsing and printing command line arguments
*///////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include "scope.h"

/* parseArgs returns a struct containing all parsed osciloscope
 * parameters 
 */
cmdargs parseArgs(int argc, char *const *argv) {
	// create data structure
	cmdargs ca;
	
	//set default values
	ca.mode = TRIGGER;
	ca.trig_level = 2500;
	ca.trig_slope = POS;
	ca.samp_rate = 10;
	ca.trig_ch = 1;
	ca.xscale = 1000;
	ca.yscale = 1000;
	
	int opt;
	int num;
	while((opt = getopt(argc, argv, "m:t:s:r:c:x:y:")) != -1) {
		switch(opt) {
			case 'm': // MODE
				if ((strcmp(optarg, "free")) == 0) {
					ca.mode = FREE;
				} else if ((strcmp(optarg, "trigger")) != 0) {
					printf("Invalid mode\n");
					exit(EXIT_FAILURE);
				}
				break;
			
			case 't': // TRIGGER LEVEL
				num = atoi(optarg);
				if ((0 <= num <= 5000) && (num % 100) == 0) {
					ca.trig_level = num;
				} else {
					printf("Invalid trigger level\n");
					exit(EXIT_FAILURE);
				}
				break;
			
			case 's': // TRIGGER SLOPE
				if ((strcmp(optarg, "neg")) == 0) {
					ca.trig_slope = NEG;
				} else if ((strcmp(optarg, "pos")) != 0) {
					printf("Invalid trigger slope\n");
					exit(EXIT_FAILURE);
				}
				break;
			
			case 'r': // SAMPLE RATE
				num = atoi(optarg);
				if (num == 1 || num == 10 || num == 20 ||
					num == 50 || num == 100) {
					
					ca.samp_rate = num;
				} else {
					printf("Invalid sample rate\n");
					exit(EXIT_FAILURE);
				}
				
				break;
			
			case 'c': // TRIGGER CHANNEL
				num = atoi(optarg);
				if (num = 2) {
					ca.trig_ch = num;
				} else if (num != 1) {
					printf("Invalid trigger channel\n");
					exit(EXIT_FAILURE);
				}
				break;
			
			case 'x': // XSCALE
				num = atoi(optarg);
				if (num == 100 || num == 500 || num == 1000 ||
					num == 2000 || num == 5000 || num == 10000 ||
					num == 50000 || num == 100000) {
					
					ca.xscale = num;
				} else {
					printf("Invalid xscale\n");
					exit(EXIT_FAILURE);
				}
				break;
			
			case 'y': // YSCALE:
				num = atoi(optarg);
				if (num == 100 || num == 500 || num == 1000 ||
					num == 2000 || num == 2500) {
						
					ca.yscale = num;
				} else {
					printf("Invalid yscale\n");
					exit(EXIT_FAILURE);
				}
		}
	}
	ca.sps = (ca.samp_rate * ca.xscale * xdiv) / 1000;
	return ca;
}

/* printArgs prints the values from a cmdargs struct on the terminal
 */
void printArgs(cmdargs	ca) {
	printf("-- PARSED CMD ARGS --\n");
	if (ca.mode == FREE) {
		printf("Mode: free\n");
	} else {
		printf("Mode: trigger\n");
	}
	printf("Trig level: %d\n", ca.trig_level);
	if (ca.mode == POS) {
		printf("Trig slope:  pos\n");
	} else {
		printf("Trig slope:  neg\n");
	}
	printf("Samp rate: %d\n", ca.samp_rate);
	printf("Trig chan: %d\n", ca.trig_ch);
	printf("Xscale: %d\n", ca.xscale);
	printf("Yscale: %d\n", ca.yscale);
	printf("SPS: %d\n", ca.sps);
	
	printf("----------------------\n\n");
}
