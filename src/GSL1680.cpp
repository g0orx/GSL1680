/**************************************************************************/
/*!
    @file     GSL1680.cpp
    @author   g0orx - John Melton

    @added TwoWire parameter to begin function to allow user to specify which I2C bus to use.

    @thanks to Skallwar code based on ^?https://github.com/Skallwar/GSL1680
    @thanks to wolfmanjm for the source code this lib is bassed on : https://github.com/wolfmanjm/GSL1680

    @modified by John Melton G0ORX to allow define of I2C inteface to use
*/

#include <Arduino.h>
#include <Wire.h>

#include <stdint.h>

#include "GSL1680.h"
#include "gslX680firmware.h"

// Registres
#define I2CADDR 0x40
#define DATA_REG 0x80
#define STATUS_REG 0xE0
#define PAGE_REG 0xF0

//CONST
#define BUFSIZE 32

#define SERIAL_ERROR if(GSL1680_DEBUG_ERROR)Serial
#define SERIAL_INFORMATION if(GSL1680_DEBUG_INFO)Serial

struct Touch_event ts_event;

GSL1680::GSL1680() {
    GSL1680_DEBUG_ERROR = false;
    GSL1680_DEBUG_INFO = false;
}

GSL1680::GSL1680(bool error, bool info) {
    GSL1680_DEBUG_ERROR = error;
    GSL1680_DEBUG_INFO = info;
}

void GSL1680::begin(uint8_t WAKE, uint8_t INTRPT, TwoWire *I2Cpipe = &Wire)
{
    SERIAL_INFORMATION.println("GSL1680: Start boot up sequence");
    __Wire=I2Cpipe;
    pinMode(WAKE, OUTPUT);          //
    digitalWrite(WAKE, LOW);        //
    pinMode(INTRPT, INPUT_PULLUP);  // Startup sequence PIN part
    delay(100);

  SERIAL_INFORMATION.println("Toggle Wake");
	digitalWrite(WAKE, HIGH);
	delay(50);
	digitalWrite(WAKE, LOW);
	delay(50);
	digitalWrite(WAKE, HIGH);
	delay(30);
        delay(250);
    __Wire->begin();

    // CTP startup sequence
	SERIAL_INFORMATION.println("Clear reg");
	clear_reg();
	SERIAL_INFORMATION.println("Reset chip");
	reset();
	SERIAL_INFORMATION.println("GSL1680: Load FW");
	loadfw();
	//startup_chip();
	SERIAL_INFORMATION.println("Reset chip 2");
	reset();
	SERIAL_INFORMATION.println("Startup Chip");
    startchip();
	SERIAL_INFORMATION.println("GSL1680: Boot up complete");
}

void GSL1680::clear_reg()
{
    uint8_t REG[4] = {0xE0, 0x80, 0xE4, 0xE0};
    uint8_t DATA[4] = {0x88, 0x01, 0x04, 0x00};
    uint8_t TIMER[4] = {20, 5, 5, 20};

    //__Wire->beginTransmission(I2CADDR);

    int i;
    int  r;
    for (i = 0; i < 4; ++i) {
        __Wire->beginTransmission(I2CADDR);
        __Wire->write(REG[i]);
        __Wire->write(DATA[i]);
        r = __Wire->endTransmission();
        if(r!=0) {
          SERIAL_INFORMATION.print("clear_reg: endTransmission: error=");
          SERIAL_INFORMATION.print(r);
          SERIAL_INFORMATION.print(" reg=");
          SERIAL_INFORMATION.print(REG[i],HEX);
          SERIAL_INFORMATION.print(" data=");
          SERIAL_INFORMATION.println(DATA[i],HEX);
        }
        delay(TIMER[i]);
    }

    //int r = __Wire->endTransmission();
    //if (r != 0){
    //    SERIAL_ERROR.print("i2c write error: "); SERIAL_ERROR.print(r); SERIAL_ERROR.print(" "); SERIAL_ERROR.println(REG[i], HEX);
    //}

}

void GSL1680::reset()
{
    uint8_t REG[2] = {STATUS_REG, 0xE4};
    uint8_t DATA[2] = {0x88, 0x04};
    uint8_t TIMER[2] = {20, 10};

    //__Wire->beginTransmission(I2CADDR);

    int i;
    int r;
    for (i = 0; i < 2; ++i) {
        __Wire->beginTransmission(I2CADDR);
        __Wire->write(REG[i]);
        __Wire->write(DATA[i]);
        r = __Wire->endTransmission();
        delay(TIMER[i]);
    }

    //int r = __Wire->endTransmission();
    //if (r != 0){
    //    SERIAL_ERROR.print("i2c write error: "); SERIAL_ERROR.print(r); SERIAL_ERROR.print(" "); SERIAL_ERROR.println(REG[i], HEX);
    //}

    uint8_t DATA_2[4] = {0};

    datasend (0xBC, DATA_2, 4);
    delay(10);
}

void GSL1680::loadfw()
{
    uint8_t addr;
    uint8_t Wrbuf[4];
    size_t source_len = sizeof(GSLX680_FW) / sizeof(struct fw_data);

    for (size_t source_line = 0; source_line < source_len; source_line++) {
        addr = GSLX680_FW[source_line].offset;
        Wrbuf[0] = (char)(GSLX680_FW[source_line].val & 0x000000ff);
        Wrbuf[1] = (char)((GSLX680_FW[source_line].val & 0x0000ff00) >> 8);
        Wrbuf[2] = (char)((GSLX680_FW[source_line].val & 0x00ff0000) >> 16);
        Wrbuf[3] = (char)((GSLX680_FW[source_line].val & 0xff000000) >> 24);

        datasend(addr, Wrbuf, 4);
    }
}

void GSL1680::startchip()
{
    __Wire->beginTransmission(I2CADDR);

    __Wire->write(0xE0);   //Registre
    __Wire->write(0x00);   //DATA

    int r = __Wire->endTransmission();
    if (r != 0){
        SERIAL_ERROR.print("i2c write error: "); SERIAL_ERROR.print(r); SERIAL_ERROR.print(" "); SERIAL_ERROR.println(0xE0, HEX);
    }
}

void GSL1680::sleep()
{

}

void GSL1680::datasend(uint8_t REG, uint8_t DATA[], uint16_t NB)
{
    __Wire->beginTransmission(I2CADDR);

    __Wire->write(REG);

    for (uint16_t i = 0; i < NB; i++) {
        __Wire->write(DATA[i]);
    }

    int r = __Wire->endTransmission();
    if (r != 0) {
        SERIAL_ERROR.print("i2c write error: "); SERIAL_ERROR.print(r); SERIAL_ERROR.print(" "); SERIAL_ERROR.println(REG, HEX);
    }
}

uint8_t GSL1680::dataread()
{
    uint8_t TOUCHRECDATA[24] = {0};

    __Wire->beginTransmission(I2CADDR);

    __Wire->write(DATA_REG);

    int n = __Wire->endTransmission();
    if (n != 0) {
        SERIAL_ERROR.print("i2c write error: "); SERIAL_ERROR.print(n); SERIAL_ERROR.print(" "); SERIAL_ERROR.println(DATA_REG, HEX);
    }

    n = __Wire->requestFrom(I2CADDR, 24);
    if (n != 24) {
        SERIAL_ERROR.print("i2c read error: did not get expected count "); SERIAL_ERROR.print(n); SERIAL_ERROR.print("/"); SERIAL_ERROR.println("24");
    }

    for(int i = 0; i<n; i++) {
        TOUCHRECDATA[i] = __Wire->read();
    }

    ts_event.NBfingers = TOUCHRECDATA[0];
    for(int i = 0; i<ts_event.NBfingers; i++) {
        ts_event.fingers[i].x = ( (((uint32_t)TOUCHRECDATA[(i*4)+5])<<8) | (uint32_t)TOUCHRECDATA[(i*4)+4] ) & 0x00000FFF; // 12 bits of X coord
        ts_event.fingers[i].y = ( (((uint32_t)TOUCHRECDATA[(i*4)+7])<<8) | (uint32_t)TOUCHRECDATA[(i*4)+6] ) & 0x00000FFF;
        ts_event.fingers[i].fingerID = (uint32_t)TOUCHRECDATA[(i*4)+7] >> 4; // finger that did the touch
    }

    return ts_event.NBfingers;
}

uint8_t GSL1680::readFingerID(int NB)
{
    return ts_event.fingers[NB].fingerID;
}

uint32_t GSL1680::readFingerX(int NB)
{
    return ts_event.fingers[NB].x;
}

uint32_t GSL1680::readFingerY(int NB)
{
    return ts_event.fingers[NB].y;
}
