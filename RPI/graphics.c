/*//////////////////////////////////////////////////////////////////////
File: graphics.c
 
Description: Handles visuals for oscilloscope
*///////////////////////////////////////////////////////////////////////

#include "scope.h"

#define M 25
// Width, Height, Pixels per volt
int W, H, pxpv;

// Function Prototypes
void drawBackground(void);
void printSettings(cmdargs ca);

/**********************************************************************/

/* windowInit gets the width and hight of the screen, draws the
 * background, and calculates pixels per volt
 */
void windowInit(cmdargs ca) {
	init(&W, &H);
	Start(W, H);
	drawBackground();
	
	pxpv = ((H - 2*M) * 1000) / (ydiv * ca.yscale);
}

/* windowReset clears the current screen. draws the background, and
 * prints the oscilloscope parameters onto the screen
 */
void windowReset(cmdargs ca) {
	WindowClear();
	drawBackground();
	printSettings(ca);
}

/**********************************************************************/

void processSamples(char *data, data_point *point_array, int count) {
	data_point p;
	for (int i = 0; i < count; i++) {
		p.x = M + ((W - 2*M) / (float)(count - 1)) * i;
		p.y = ((data[i] * 5* pxpv)/256);
		point_array[i] = p;
	}
}

void plotWave(data_point *data, int sps, int yoff, int ch) {
	StrokeWidth(4);
	if (ch == 1) {
		Stroke(color_yellow, 0.75);
	} else {
		Stroke(color_green, 0.75);
	}
		
	
	int xs, xe, ys, ye;
	
	data_point p = data[0];
	xs = p.x;
	ys = p.y + yoff;
	for (int i = 1; i < sps; i++) {
		p = data[i];
		xe = p.x;
		ye = p.y + yoff;
		Line(xs, ys, xe, ye);
		xs = xe;
		ys = ye;
	}
}

void plotWavePoly(data_point *data, int sps, int yoff, int ch) {
	StrokeWidth(4);
	if (ch == 1) {
		Stroke(color_yellow, 1);
	} else {
		Stroke(color_green, 1);
	}
	
	VGfloat x[sps], y[sps];
	data_point p;
	for (int i = 0; i < sps; i++) {
		p = data[i];
		x[i] = p.x;
		y[i] = p.y + yoff;
	}
	
	Polyline(x, y, sps);
}

/**********************************************************************/

void drawBackground(void) {
	StrokeWidth(0);
	Background(color_grey);
	
	Fill(25, 25, 25, 1);
	Rect(M, M, W - 2*M, H - 2*M);
	
	Stroke(color_grey, 0.5);
	StrokeWidth(2);
	
	for (float x = M; x <= W - M; x += (W - 2*M) / (float)xdiv) { 
		Line(x, M, x, H - M);
	}
	for (float y = M; y <= H - M; y += (H - 2*M) / (float)ydiv) { 
		Line(M, y, W - M, y);
	}
}

void printSettings(cmdargs ca) {
	char str[100];
	VGfloat blue[4] = {color_black, 1};
	setfill(blue);
	if (ca.mode == TRIGGER){
		sprintf(str, "Mode: Trigger", ca.xscale);
	} else {
		sprintf(str, " Mode: Free", ca.xscale);
	}
	Text(M + 10, H - 20, str, SansTypeface, 15);
	
	sprintf(str, " Trig ch: %d", ca.trig_ch);
	Text(510, H - 20, str, SansTypeface, 15);
	
	sprintf(str, "Trig level: %1.1fV", ca.trig_level / (float) 1000);
	Text(W - 800, H - 20, str, SansTypeface, 15);
	
	if (ca.trig_slope = POS){
		sprintf(str, "Trig slope: Pos");
	} else {
		sprintf(str, "Trig slope: Neg");
	}
	Text(W - 180, H - 20, str, SansTypeface, 15);
	
	sprintf(str, "%d samples/s", ca.samp_rate);
	Text(M + 10, 5, str, SansTypeface, 15);
	
	if (ca.xscale >= 1000) {
		sprintf(str, " Xscale: %dms/div", ca.xscale / 1000);
	} else {
		sprintf(str, "Xscale: %dus/div", ca.xscale);
	}
	Text(W - 500, 5, str, SansTypeface, 15);
	
	if (ca.yscale >= 1000) {
		sprintf(str, " Yscale: %1.1fV/div", ca.yscale / (float)1000);
	} else {
		sprintf(str, "Yscale: %dmV/div", ca.yscale);
	}
	Text(W - 250, 5, str, SansTypeface, 15);
}



