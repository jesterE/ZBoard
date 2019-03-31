# ZBoard
This repository contains firmware for a keyboard I built using the Teensy 2.0

Disclaimer: This is a hobbyist project that is currently lacking many features. 
If you are looking for firmware for a keyboard project of your own, I would advise you to look elsewhere.

### Current Features:
  * 6-key rollover
  * Support for boot mode protocol
  * Three separate key mapping layers
  
### Planned Features:
  * N-key rollover
  * Support for changing keymap without recompiling (will require additional software running on host)
  * Support for more than 3 layers
  

## Compiling
This project requires GNU Make, as well as a version of avr-gcc with support for C11 in order to compile.
The build system used is [the same one used by the LUFA library](https://github.com/abcminiuser/lufa/tree/a8546529926fc87b5d73b75217ec011962e8ccaa/LUFA/Build/DMBS): 
simply running `make` will build a hex file that can be flashed onto the chip.

