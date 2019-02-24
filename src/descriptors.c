#include "descriptors.h"

const USB_Descriptor_String_t PROGMEM LANGUAGE_STRING = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM MANUFACTURER_STRING = USB_STRING_DESCRIPTOR(L"TEST");
const USB_Descriptor_String_t PROGMEM PRODUCT_STRING = USB_STRING_DESCRIPTOR(L"KEYBOARD TESt");

const USB_Descriptor_Device_t PROGMEM DEVICE_DESCRIPTOR = {
    .Header = {
        .Size = sizeof(USB_Descriptor_Device_t), 
        .Type = DTYPE_Device
    },

    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID = 0x03EB,
    .ProductID = 0x2042,
    .ReleaseNumber = VERSION_BCD(0, 0, 1),

    .ManufacturerStrIndex = ManufacturerString,
    .ProductStrIndex = ProductString,
    .SerialNumStrIndex = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM KEYBOARD_REPORT[] = {
    HID_DESCRIPTOR_KEYBOARD(6)
};

const UsbConfigurationDescriptor PROGMEM CONFIGURATION_DESCRIPTOR = {
    .header = {
        .Header = {
            .Size = sizeof(USB_Descriptor_Configuration_Header_t), 
            .Type = DTYPE_Configuration
        },

        .TotalConfigurationSize = sizeof(UsbConfigurationDescriptor),
        .TotalInterfaces = 1,

        .ConfigurationNumber = 1,
        .ConfigurationStrIndex = NO_DESCRIPTOR,

        .ConfigAttributes = USB_CONFIG_ATTR_RESERVED,
        
        .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)

    },

    .keyboard_interface_descriptor = {
        .Header = {
            .Size = sizeof(USB_Descriptor_Interface_t),
            .Type = DTYPE_Interface
        },

        .InterfaceNumber = KEYBOARD_INTERFACE_NUMBER,
        .AlternateSetting = 0x00,

        .TotalEndpoints = 1,

        .Class = HID_CSCP_HIDClass,
        .SubClass = HID_CSCP_BootSubclass,
        .Protocol = HID_CSCP_KeyboardBootProtocol,

        .InterfaceStrIndex = NO_DESCRIPTOR
    },

    .keyboard_hid_descriptor = {
        .Header = {
            .Size = sizeof(USB_HID_Descriptor_HID_t),
            .Type = HID_DTYPE_HID
        },

        .HIDSpec = VERSION_BCD(1, 1, 1),
        .CountryCode = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType = HID_DTYPE_Report,
        .HIDReportLength = sizeof(KEYBOARD_REPORT),
    },

    .keyboard_endpoint_descriptor = {
        .Header = {
            .Size = sizeof(USB_Descriptor_Endpoint_t),
            .Type = DTYPE_Endpoint
        },

        .EndpointAddress = ENDPOINT_DIR_IN | KEYBOARD_ENDPOINT_ADDRESS,
        .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize = KEYBOARD_ENDPOINT_SIZE,
        .PollingIntervalMS = 0x05
    }
};

uint16_t CALLBACK_USB_GetDescriptor(
        const uint16_t w_value, 
        const uint16_t w_index, 
        const void **descriptor_address) 
{

    (void)w_index;

    uint8_t descriptor_type = w_value >> 8;
    uint8_t descriptor_index = w_value & 0xFF;

    switch (descriptor_type) {
        case DTYPE_Device:
            *descriptor_address = &DEVICE_DESCRIPTOR;
            return sizeof(DEVICE_DESCRIPTOR);
            break;

        case DTYPE_Configuration:
            *descriptor_address = &CONFIGURATION_DESCRIPTOR;
            return sizeof(CONFIGURATION_DESCRIPTOR);
            break;

        case DTYPE_String:
            switch (descriptor_index) {
                case LanguageString:
                    *descriptor_address = &LANGUAGE_STRING;
                    return pgm_read_byte(&LANGUAGE_STRING.Header.Size);
                    break;

                case ManufacturerString:
                    *descriptor_address = &MANUFACTURER_STRING;
                    return pgm_read_byte(&MANUFACTURER_STRING.Header.Size);
                    break;

                case ProductString:
                    *descriptor_address = &PRODUCT_STRING;
                    return pgm_read_byte(&PRODUCT_STRING.Header.Size);
                    break;
            }
            break;

        case HID_DTYPE_HID:
            *descriptor_address = &CONFIGURATION_DESCRIPTOR.keyboard_hid_descriptor;
            return sizeof(USB_HID_Descriptor_HID_t);
            break;

        case HID_DTYPE_Report:
            *descriptor_address = &KEYBOARD_REPORT;
            return sizeof(KEYBOARD_REPORT);
            break;
    }

    *descriptor_address = NULL;
    return NO_DESCRIPTOR;
}
