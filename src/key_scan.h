#pragma once

#include <stdbool.h>

#define NUM_COLUMNS 4
#define NUM_ROWS 12

/**
\brief Initializes the row and column pins for key press scanning.

Should be called once before using scan_keys()
*/
void setup_pins(void);

/**
\brief Determines which keys the user is currently pressing.

\param pressed_keys Array to store the pressed keys.
    true indicates the key is being pressed, false means that it isn't.
    This array should not be modified between calls to this function,
    in order for the debouncing logic to work correctly.
*/
void scan_keys(bool (*pressed_keys)[NUM_COLUMNS][NUM_ROWS]);


