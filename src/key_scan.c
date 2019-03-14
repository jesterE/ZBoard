#include "key_scan.h"

#include <avr/pgmspace.h>


// ports that will be switched to output low in order to scan for 
// pressed keys
static volatile uint8_t* const COLUMN_PORTS[NUM_COLUMNS] PROGMEM = {
    &DDRB, &DDRB, &DDRC, &DDRD
};

// goes with above array, determines the pin number of each port 
static uint8_t const COLUMN_PINS[NUM_COLUMNS] PROGMEM = {
    5, 4, 7, 4
};

// ports that will be set to input with pullup resistor.
// when pulled to logic low means a key is being pressed on that row
static volatile uint8_t* const ROW_PORTS[NUM_ROWS] PROGMEM = {
    &PINF, &PINB, &PINB,
    &PINB, &PINB, &PIND,
    &PIND, &PIND, &PIND,
    &PINC, &PIND, &PIND
};

// goes with above array, determines pin number for each port
static uint8_t const ROW_PINS[NUM_ROWS] PROGMEM = {
    0, 1, 2,
    3, 7, 0,
    1, 2, 3,
    6, 7, 5
};

static void setup_columns(void) {
    // initially configured as high impedance inputs
    // switched temporarily to output low during the scan routine
    DDRB &= ~(0b00110000);
    PORTB &= ~(0b00110000);

    DDRC &= ~(1 << 7);
    PORTC &= ~(1 << 7);

    DDRD &= ~(1 << 4);
    PORTD &= ~(1 << 4);
}

static void setup_rows(void) {
    // input with pullup resistor
    DDRF &= ~(1 << 0);
    PORTF |= (1 << 0);

    DDRB &= ~(0b10001110);
    PORTB |= (0b10001110);

    DDRD &= ~(0b10101111);
    PORTD |= (0b10101111);

    DDRC &= ~(1 << 6);
    PORTC |= (1 << 6);
}

void setup_pins(void) {
    setup_columns();
    setup_rows();
}

#define DEBOUNCE_COUNT 10

void scan_keys(bool (*pressed_keys)[NUM_COLUMNS][NUM_ROWS]) {
    // used to stop random fluctuations in the input signal from
    // affecting the output. This array keeps track of the 
    // number of consecutive reads required for each key
    // in order for it to be considered to have a stable 
    // value
    static uint8_t debounce_counters[NUM_COLUMNS][NUM_ROWS] = {
        [0 ... (NUM_COLUMNS - 1)] = {
            [0 ... (NUM_ROWS - 1)] = DEBOUNCE_COUNT
        }
    };

    for (uint8_t col = 0; col < NUM_COLUMNS; col++) {
        volatile uint8_t *col_port = pgm_read_ptr(&COLUMN_PORTS[col]);
        uint8_t col_pin = pgm_read_byte(&COLUMN_PINS[col]);

        // set the column pin to output low
        *col_port |= (1 << col_pin);

        for (uint8_t row = 0; row < NUM_ROWS; row++) {
            volatile uint8_t *row_port = pgm_read_ptr(&ROW_PORTS[row]);
            uint8_t row_pin = pgm_read_byte(&ROW_PINS[row]);

            // the press signal is active low, so we not the value to get
            // the proper boolean
            bool pressed = !(*row_port & (1 << row_pin));

            if (pressed != (*pressed_keys)[col][row]) {
                // detected a difference between the current state of the key
                // and what was read from the input pin. this means we now
                // wait until we get enough consecutive reads of the same value,
                // before we officially change the state of the key.
                debounce_counters[col][row]--;

                if (debounce_counters[col][row] == 0) {
                    (*pressed_keys)[col][row] = pressed;
                    debounce_counters[col][row] = DEBOUNCE_COUNT;
                }

            } else {
                debounce_counters[col][row] = DEBOUNCE_COUNT;
            }
        }

        // set the column pin back to high impedance
        *col_port &= ~(1 << col_pin);
    }
}
