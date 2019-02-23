#pragma once

#include <avr/pgmspace.h>

#include <LUFA/Drivers/USB/USB.h>

typedef struct  __attribute__((packed)) {
    USB_Descriptor_Configuration_Header_t header;

    USB_Descriptor_Interface_t keyboard_interface_descriptor;
    USB_HID_Descriptor_HID_t keyboard_hid_descriptor;
    USB_Descriptor_Endpoint_t keyboard_endpoint_descriptor;

} UsbConfigurationDescriptor;

typedef enum {
    LanguageString = 0,
    ManufacturerString = 1,
    ProductString = 2
} DeviceStringIndex;

#define KEYBOARD_INTERFACE_NUMBER 0
#define KEYBOARD_ENDPOINT_ADDRESS 1
#define KEYBOARD_ENDPOINT_SIZE 8

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex, const void **const descriptor_address);

