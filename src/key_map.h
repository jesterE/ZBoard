#pragma once

#include <stdint.h>
#include <stddef.h>

#include "key_scan.h"

/**
\brief Translates rows and columns into scancodes based on the keymap.

The keymap is currently just a static array located in key_map.c

\todo Add support for dynamically adjusting the keymap at runtime.

\param pressed_keys Array of all the keys currently being pressed.
\param output_buffer Buffer where the scancode values for all pressed keys will be written to.
\param output_size The size of output_buffer. This function will only translate scancodes up until
    this limit is reached.

\returns The number of scancodes written into the output buffer. 
*/
uint8_t get_scancodes(bool (*pressed_keys)[NUM_COLUMNS][NUM_ROWS], uint8_t *output_buffer, size_t output_size );

