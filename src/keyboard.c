#include "descriptors.h"

#include <avr/wdt.h>

// The watchdog isn't used, but is still disabled in case it gets enabled accidentily,
// as recommended in the datasheet
void kill_watchdog(void) __attribute__((naked)) __attribute__((section (".init3")));
void kill_watchdog(void) {
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
}

int main(void) {
    while (1) {
        __asm__ __volatile__ ("nop" ::);
        // TODO
    }
} 
