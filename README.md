# GSL1680 Arduino driver

pin | function  | Teensy 4.1 Using Wire2
----|-----------|------------
1   | SCL       | 24
2   | SDA       | 25
3   | VDD (3v3) | 3v3
4   | Wake      | 26
5   | Int       | 27
6   | Gnd       | GND

Warning : Firmware is really heavy so this library cannot be used in some Arduino boards

## Thanks
Information gleaned from https://github.com/rastersoft/gsl1680 and various other sources
Firmware for the specific panel was found here: http://www.buydisplay.com/default/5-inch-tft-lcd-module-800x480-display-w-controller-i2c-serial-spi
Use part of wolfmanjm source code: https://github.com/wolfmanjm/GSL1680


## Changes by G0ORX - John Melton

Added optional TwoWire parameter to begin function to allow user to specify which I2C bus to use (defaults to Wire).

Replaced gslX680firmware.h with latest version from buydisplay.com to support 800x480 touchscreen and assigned the the firmware to FLASHMEM.

Note:  for T41-EP use Wire2 (pins 24 and 25)

