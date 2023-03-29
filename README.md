# GSL1680 Arduino driver

**This project is not in active development since I don't have the hardware anymore, but if a bug is found, please consider opening an issue or a pull request**

pin | function  | Arduino Uno
----|-----------|------------
1   | SCL       | A5
2   | SDA       | A4
3   | VDD (3v3) | 3v3
4   | Wake      | 4
5   | Int       | 2
6   | Gnd       | GND

Warning : Firmware is really heavy so this library cannot be used in some Arduino boards

## Thanks
Information gleaned from https://github.com/rastersoft/gsl1680 and various other sources
Firmware for the specific panel was found here: http://www.buydisplay.com/default/5-inch-tft-lcd-module-800x480-display-w-controller-i2c-serial-spi
Use part of wolfmanjm source code: https://github.com/wolfmanjm/GSL1680


Changes by g0orx - John Melton

Added TwoWire parameter to begin function to allow user to specify which I2C bus to use (defaults to Wire).

Replaced gslX680firmware.h with latest version from buydisplay.com to support 800x480 touchscreen

Note:  for T41-EP use Wire2 and set WAKE to pin 26 and INTRPT to pin 27

