/*//////////////////////////////////////////////////////////////////////
File: data.c
 
Description: Functions for processing and displaying data
*///////////////////////////////////////////////////////////////////////

#include "scope.h"

#define CH1_H 500
#define CH2_H 20

int processTrigger(cmdargs ca, char data[]);

/* processAndPlot finds the trigger index if enabled and
 * processes raw ADC data into two arrays of data_points. The
 * function then reads in the potentiometer values from the
 * I2C and plots the two waveforms on their respective yoffset
 */
void processAndPlot(cmdargs ca, char *ch1_data, char *ch2_data) {
	int toffset = 0;
	
	// cut off unreliable data
	if (ca.sps >= 200) {
		ch1_data += 128;
		ch2_data += 128;
	}
	
	// process trigger
	if (ca.mode == TRIGGER) {
		if (ca.trig_ch == 1) 
			toffset = processTrigger(ca, ch1_data);
		else
			toffset = processTrigger(ca, ch2_data);
	}
	ch1_data += toffset;
	ch2_data += toffset;
	
	data_point ch1_points[ca.sps], ch2_points[ca.sps];
	
	processSamples(ch1_data, ch1_points, ca.sps);
	processSamples(ch2_data, ch2_points, ca.sps);
	
	int ch1_yoff, ch2_yoff;
	i2cReceive(&ch1_yoff, &ch2_yoff);
	
	ch1_yoff += CH1_H;
	ch2_yoff += CH2_H;
	
	plotWave(ch1_points, ca.sps, ch1_yoff, 1);
	plotWave(ch2_points, ca.sps, ch2_yoff, 2);
	
	// Screen ready for display
	End();
}


/* processTrigger walks through raw ADC data and returns the first index
 * that is detected by the trigger conditions
*/
int processTrigger(cmdargs ca, char data[]) {
	int toffset = 0;
	float trigger = (ca.trig_level * 256) / (float)5000;
	int val[2] = {data[0], 0};
	
	for (int i = 4; i < ca.sps; i+=4) {
		val[1] = (data[i-4] + 2*data[i-3] + 3*data[i-2] + 2*data[i-1] + data[i])/8;
			
		if (ca.trig_slope == POS) {
			if (val[0] < trigger && trigger < val[1]) {
				toffset = i-4;
				break;
			}
		} else if (val[0] > trigger && trigger > val[1]) {
			toffset = i-4;
			break;
		}
			
		val[0] = val[1];
	}
	return toffset;
}
