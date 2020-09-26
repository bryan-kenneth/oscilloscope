/*//////////////////////////////////////////////////////////////////////
File: i2ccomm.c
 
Description: Handles I2C communications
*///////////////////////////////////////////////////////////////////////

#include <wiringPiI2C.h>
#include "scope.h"

//7-bit I2C address of PSoC
#define I2C_SLAVE_ADDR 0x53 

int i2c_fd;

/* i2cInit sets up I2C for communication with PSoC slave
 */
void i2cInit(int sample_rate) {
	// Initialize the I2C interface
	if ((i2c_fd = wiringPiI2CSetup(I2C_SLAVE_ADDR)) == -1) {
		perror("I2C device not found\n");
	}
	
	// Send sample rate to PSoC
	wiringPiI2CWrite(i2c_fd, sample_rate);
}

/* i2cReceive used to get potentiometer values from PSoC
 */
void i2cReceive (int *p1, int *p2) {
	(*p1) = wiringPiI2CRead(i2c_fd);
	(*p2) = wiringPiI2CRead(i2c_fd);
}
