#include "key_map.h"

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

// The next 3 arrays contain a mapping from each key to a scancode.
//
// Each key can have up to 3 scancodes bound to it, with each scancode 
// on a different layer.
//
// The middle layer is active by default. While holding down the layer up key,
// the scancode mapping for the top layer is used instead. The layer down key
// does the same thing, but for the bottom layer.
// 
// a scancode of 0 means one of two things:
//      * no scancode is bound to the key
//      * one of the layer changing keys is located there
//
// if the 0 represents a layer changing key, then make sure
// to record its position in either LAYER_UP_KEY_POSITION
// or LAYER_DOWN_KEY_POSITION
static const uint8_t TOP_LAYER_MAPPING[NUM_COLUMNS][NUM_ROWS] PROGMEM = {
    {HID_KEYBOARD_SC_EQUAL_AND_PLUS, HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE, HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS, HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS,
     HID_KEYBOARD_SC_8_AND_ASTERISK, HID_KEYBOARD_SC_7_AND_AMPERSAND, HID_KEYBOARD_SC_6_AND_CARET, HID_KEYBOARD_SC_5_AND_PERCENTAGE,
     HID_KEYBOARD_SC_4_AND_DOLLAR, HID_KEYBOARD_SC_3_AND_HASHMARK, HID_KEYBOARD_SC_2_AND_AT, HID_KEYBOARD_SC_1_AND_EXCLAMATION
    },

    {HID_KEYBOARD_SC_BACKSLASH_AND_PIPE, HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE, HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE, 0,
     0, 0, 0, 0,
     0, 0, 0, HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE
    },

    {HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, HID_KEYBOARD_SC_LEFT_SHIFT
    },

    {HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK, 0, 0, HID_KEYBOARD_SC_RIGHT_CONTROL,
     0, HID_KEYBOARD_SC_SPACE, HID_KEYBOARD_SC_SPACE, 0,
     HID_KEYBOARD_SC_LEFT_ALT, HID_KEYBOARD_SC_LEFT_GUI, HID_KEYBOARD_SC_LEFT_CONTROL, HID_KEYBOARD_SC_LEFT_CONTROL
    }
};

static const uint8_t MIDDLE_LAYER_MAPPING[NUM_COLUMNS][NUM_ROWS] PROGMEM = {
    {HID_KEYBOARD_SC_BACKSPACE, HID_KEYBOARD_SC_P, HID_KEYBOARD_SC_O, HID_KEYBOARD_SC_I,
     HID_KEYBOARD_SC_U, HID_KEYBOARD_SC_Y, HID_KEYBOARD_SC_T, HID_KEYBOARD_SC_R,
     HID_KEYBOARD_SC_E, HID_KEYBOARD_SC_W, HID_KEYBOARD_SC_Q, HID_KEYBOARD_SC_ESCAPE    
    },

    {HID_KEYBOARD_SC_ENTER, HID_KEYBOARD_SC_SEMICOLON_AND_COLON, HID_KEYBOARD_SC_L, HID_KEYBOARD_SC_K,
     HID_KEYBOARD_SC_J, HID_KEYBOARD_SC_H, HID_KEYBOARD_SC_G, HID_KEYBOARD_SC_F,    
     HID_KEYBOARD_SC_D, HID_KEYBOARD_SC_S, HID_KEYBOARD_SC_A, HID_KEYBOARD_SC_TAB    
    },

    {HID_KEYBOARD_SC_RIGHT_SHIFT, HID_KEYBOARD_SC_UP_ARROW, HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN, HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN,
     HID_KEYBOARD_SC_M, HID_KEYBOARD_SC_N, HID_KEYBOARD_SC_B, HID_KEYBOARD_SC_V,
     HID_KEYBOARD_SC_C, HID_KEYBOARD_SC_X, HID_KEYBOARD_SC_Z, HID_KEYBOARD_SC_LEFT_SHIFT 
    },

    {HID_KEYBOARD_SC_RIGHT_ARROW, HID_KEYBOARD_SC_DOWN_ARROW, HID_KEYBOARD_SC_LEFT_ARROW, HID_KEYBOARD_SC_RIGHT_CONTROL,
     0, HID_KEYBOARD_SC_SPACE, HID_KEYBOARD_SC_SPACE, 0,
     HID_KEYBOARD_SC_LEFT_ALT, HID_KEYBOARD_SC_LEFT_GUI, HID_KEYBOARD_SC_LEFT_CONTROL, HID_KEYBOARD_SC_LEFT_CONTROL 
    }
};

static const uint8_t BOTTOM_LAYER_MAPPING[NUM_COLUMNS][NUM_ROWS] PROGMEM = {
    {HID_KEYBOARD_SC_F12, HID_KEYBOARD_SC_F11, HID_KEYBOARD_SC_F10, HID_KEYBOARD_SC_F9,
     HID_KEYBOARD_SC_F8, HID_KEYBOARD_SC_F7, HID_KEYBOARD_SC_F6, HID_KEYBOARD_SC_F5,
     HID_KEYBOARD_SC_F4, HID_KEYBOARD_SC_F3, HID_KEYBOARD_SC_F2, HID_KEYBOARD_SC_F1
    },

    {0, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0
    },

    {0, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, HID_KEYBOARD_SC_LEFT_SHIFT
    },

    {HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK, 0, 0, HID_KEYBOARD_SC_RIGHT_CONTROL,
     0, HID_KEYBOARD_SC_SPACE, HID_KEYBOARD_SC_SPACE, 0,
     HID_KEYBOARD_SC_LEFT_ALT, HID_KEYBOARD_SC_LEFT_GUI, HID_KEYBOARD_SC_LEFT_CONTROL, HID_KEYBOARD_SC_LEFT_CONTROL
    }
};

// The position of the layer down and up keys on the keyboard,
// stored column first, then row
static const uint8_t LAYER_UP_KEY_POSITION[2] = {3, 7};
static const uint8_t LAYER_DOWN_KEY_POSITION[2] = {3, 4};

uint8_t get_scancodes(bool (*pressed_keys)[NUM_COLUMNS][NUM_ROWS], uint8_t *output_buffer, size_t output_size) {
    size_t scancodes_written_out = 0;

    bool layer_up = (*pressed_keys)[LAYER_UP_KEY_POSITION[0]][LAYER_UP_KEY_POSITION[1]];
    bool layer_down = (*pressed_keys)[LAYER_DOWN_KEY_POSITION[0]][LAYER_DOWN_KEY_POSITION[1]]; 


    for (uint8_t col = 0; col < NUM_COLUMNS; col++) {
        for (uint8_t row = 0; row < NUM_ROWS; row++) {
            if ((*pressed_keys)[col][row]) {
                if (layer_up) {
                    uint8_t scancode = pgm_read_byte(&TOP_LAYER_MAPPING[col][row]);
                    output_buffer[scancodes_written_out] = scancode;
                    scancodes_written_out++;
                } else if (layer_down) {
                    uint8_t scancode = pgm_read_byte(&BOTTOM_LAYER_MAPPING[col][row]);
                    output_buffer[scancodes_written_out] = scancode;
                    scancodes_written_out++;
                } else {
                    uint8_t scancode = pgm_read_byte(&MIDDLE_LAYER_MAPPING[col][row]);
                    output_buffer[scancodes_written_out] = scancode;
                    scancodes_written_out++;
                }
            }


            if (scancodes_written_out >= output_size) {
                return scancodes_written_out;
            }
        }
    }

    return scancodes_written_out;
}

