/*
 * usb.cpp
 *
 * This module has the wrapper functions to access USB driver functions.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
*/


#ifdef Q_OS_WIN32
#include <setupapi.h>
#endif
#include "usb.h"
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

/***************************************************
*                  GLOBAL VARIABLES
****************************************************/
static hid_device *DeviceHandle;	//Handle to write
//In/Out buffers equal to HID endpoint size + 1
//First byte is for Windows internal use and it is always 0
unsigned char OutputBuffer[USB_MAX_PACKET_SIZE+1];
unsigned char InputBuffer[USB_MAX_PACKET_SIZE+1];

static bool USBConnected = false;      //Boolean true when device is connected

bool USB_IsConnected()
{
    return USBConnected;
}

int USB_Init(void)
{
    return hid_init();
}

int USB_Exit(void)
{
    return hid_exit();
}

int USB_Open(int id)
{
    // Open the device using the VID, PID,
    // and optionally the Serial number.
	struct hid_device_info *devs, *cur_dev;

	devs = hid_enumerate(MY_VID, MY_PID);
	cur_dev = devs;
	
	int i = 0;

	while (cur_dev) {

		if (wcscmp(cur_dev->product_string, L"DDP442X") == 0)
		{
			if (i == id)
				DeviceHandle = hid_open_path(cur_dev->path);

			printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
			printf("\n");
			printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
			printf("  Product:      %ls\n", cur_dev->product_string);
			printf("  Release:      %hx\n", cur_dev->release_number);
			printf("  Interface:    %d\n", cur_dev->interface_number);
			printf("\n");

			i++;
		}

		cur_dev = cur_dev->next;
	}

	hid_free_enumeration(devs);
		
	//DeviceHandle = hid_open(MY_VID, MY_PID, NULL);
	
    if(DeviceHandle == NULL)
    {
        USBConnected = false;
        return -1;
    }
    USBConnected = true;
    return 0;
}

int USB_Write()
{
    if(DeviceHandle == NULL)
        return -1;

    return hid_write(DeviceHandle, OutputBuffer, USB_MIN_PACKET_SIZE+1);

}

int USB_Read()
{
    if(DeviceHandle == NULL)
        return -1;

    return hid_read_timeout(DeviceHandle, InputBuffer, USB_MIN_PACKET_SIZE+1, 2000);
}

int USB_Close()
{
    hid_close(DeviceHandle);
    USBConnected = false;

    return 0;
}

