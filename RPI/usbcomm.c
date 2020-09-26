/*//////////////////////////////////////////////////////////////////////
File: i2ccomm.c
 
Description: Handles USB communications
*///////////////////////////////////////////////////////////////////////

#include <libusb.h>
#include <string.h>
#include "scope.h"

#define CH1_IN_EP 0x81
#define CH2_IN_EP 0x82
#define TIMEOUT 0 //in milliseconds (0 = disabled)

// Pointer to data structure representing USB device
libusb_device_handle* dev;

void usbInit(void) {
	// Initialize the LIBUSB library
	libusb_init(NULL);
	
	// Open the USB device (the Cypress device has
	// Vendor ID = 0x04B4 and Product ID = 0x8051)
	dev = libusb_open_device_with_vid_pid(NULL, 0x04B4, 0x8051);
	if (dev == NULL){
		perror("USB device not found\n");
	}
	
	// Reset the USB device
	if (libusb_reset_device(dev) != 0){
		perror("Device reset failed\n");
	} 

	// Set configuration of USB device
	if (libusb_set_configuration(dev, 1) != 0){
		perror("Set configuration failed\n");
	} 

	// Claim the interface
	if (libusb_claim_interface(dev, 0) !=0){
		perror("Cannot claim interface");
	}
	
	printf("USB connected\n");
}

void usbReceive(char *ch1_data, char *ch2_data, int length) {
	int return_val, received;
	
	// Perform the IN transfer for CH1 (from device to host)
	return_val = libusb_bulk_transfer(dev, CH1_IN_EP, ch1_data, length,
										  &received, TIMEOUT);
	if (return_val != 0)
		perror("USB CH2 recieve failed");
	
	// Perform the IN transfer for CH1 (from device to host)
	return_val = libusb_bulk_transfer(dev, CH2_IN_EP, ch2_data, length,
										  &received, TIMEOUT);
	if (return_val != 0)
		perror("USB CH2 recieve failed");
}
