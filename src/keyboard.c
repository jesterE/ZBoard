#include "descriptors.h"
#include "timer_utils.h"

#include <avr/wdt.h>
#include <avr/power.h>

#define TEENSY2_ENABLE_LED_OUTPUT() (DDRD |= (1 << 6))
#define TEENSY2_LED_ON() (PORTD |= (1 << 6))
#define TEENSY2_LED_OFF() (PORTD &= ~(1 << 6))
#define TEENSY2_LED_TOGGLE() (PORTD ^= (1 << 6))

// this application uses timer0 to periodically scan for pressed keys.
// the 1024 prescale is used to allow measurements on a millisecond time scale
// with 64us resolution
#define TIMER0_PRESCALE 1024

// The watchdog isn't used, but is still disabled in case it gets enabled accidentily,
// as recommended in the datasheet
void kill_watchdog(void) __attribute__((naked)) __attribute__((section (".init3")));
void kill_watchdog(void) {
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
}

static uint8_t previous_keyboard_hid_report_buffer[sizeof(USB_KeyboardReport_Data_t)];

static USB_ClassInfo_HID_Device_t keyboard_hid_state = {
    .Config = {
        .InterfaceNumber = KEYBOARD_INTERFACE_NUMBER,
        .ReportINEndpoint = {
            .Address = KEYBOARD_ENDPOINT_ADDRESS,
            .Size = KEYBOARD_ENDPOINT_SIZE,
            .Banks = 1,
        },
        .PrevReportINBuffer = previous_keyboard_hid_report_buffer,
        .PrevReportINBufferSize = sizeof(previous_keyboard_hid_report_buffer)
    }
};

int main(void) {
    // disable cpu clock prescaling: run at 16MHz
    clock_prescale_set(clock_div_1);

    TEENSY2_ENABLE_LED_OUTPUT();
    USB_Init();

    // set timer0 clock prescaler to divide by 1024
    // 1 tick = 64us
    TCCR0B |= (0b101);

    sei();

    uint8_t ticks = 0;
    while (1) {
        if (TCNT0 >= TIMER0_VALUE_MS(15, TIMER0_PRESCALE)) {
            if (ticks >= 10) {
                TEENSY2_LED_TOGGLE();
                ticks = 0;
            } else {
                ticks++;
            }
            TCNT0 = 0;
        }

        HID_Device_USBTask(&keyboard_hid_state);
        USB_USBTask();
    }
} 

void EVENT_USB_Device_ConfigurationChanged(void) {
    HID_Device_ConfigureEndpoints(&keyboard_hid_state);
    USB_Device_EnableSOFEvents();
}

void EVENT_USB_Device_ControlRequest(void) {
    HID_Device_ProcessControlRequest(&keyboard_hid_state);
}

void EVENT_USB_Device_StartOfFrame(void) {
    HID_Device_MillisecondElapsed(&keyboard_hid_state);
}

bool CALLBACK_HID_Device_CreateHIDReport(
    USB_ClassInfo_HID_Device_t *const hid_state,
    uint8_t *const report_id,
    const uint8_t report_type,
    void *report_data,
    uint16_t *const report_size)
{
    (void) hid_state;
    (void) report_id;
    (void) report_type;

    USB_KeyboardReport_Data_t *keyboard_report = report_data;
    keyboard_report->KeyCode[0] = HID_KEYBOARD_SC_A;
    *report_size = sizeof(USB_KeyboardReport_Data_t);
    return false;
}

void CALLBACK_HID_Device_ProcessHIDReport(
    USB_ClassInfo_HID_Device_t *const hid_state,
    const uint8_t report_id,
    const uint8_t report_type,
    const void *report_data,
    const uint16_t report_size)
{
    (void) hid_state;
    (void) report_id;
    (void) report_type;
    (void) report_size;

    const uint8_t *report = report_data;
    if (*report & HID_KEYBOARD_LED_CAPSLOCK) {
        TEENSY2_LED_ON();
    } else {
        TEENSY2_LED_OFF();
    }
}

