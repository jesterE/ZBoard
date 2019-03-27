#pragma once

#include <stdint.h>

/**
\brief Translates a row and column into a scancode based on the keymap.

The keymap is currently just a static array located in key_map.c

\todo Add support for multiple different layers.

\todo Add support for dynamically adjusting the keymap at runtime.
*/
uint8_t get_scancode(uint8_t col, uint8_t row);

