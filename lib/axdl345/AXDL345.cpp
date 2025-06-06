/*
    ADXL345.h
    Library for accelerometer_ADXL345

    Copyright (c) 2013 seeed technology inc.
    Author        :   FrankieChu
    Create Time   :   Jan 2013
    Change Log    :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "AXDL345.h"
#include "pi2c.h"

#define ADXL345_DEVICE (0x53)    // ADXL345 device address
#define ADXL345_TO_READ (6)      // num of uint8_ts we are going to read each time (two uint8_ts for each axis)

ADXL345::ADXL345() {
    pi2cPath = (char*)"/dev/i2c-1";
    status = ADXL345_OK;
    error_code = ADXL345_NO_ERROR;

    pi2cFd = -1;
    gains[0] = 0.00376390;
    gains[1] = 0.00376009;
    gains[2] = 0.00349265;
}

void ADXL345::powerOn() {
    pi2cFd = pi2cOpen(pi2cPath, devAddr);
    //Turning on the ADXL345
    writeTo(ADXL345_POWER_CTL, 0);
    writeTo(ADXL345_POWER_CTL, 16);
    writeTo(ADXL345_POWER_CTL, 8);
}

// Reads the acceleration into three variable x, y and z
void ADXL345::readAccel(int* xyz) {
    readXYZ(xyz, xyz + 1, xyz + 2);
}
void ADXL345::readXYZ(int* x, int* y, int* z) {
    readFrom(ADXL345_DATAX0, ADXL345_TO_READ, buffer); //read the acceleration data from the ADXL345
    *x = (short)((((unsigned short)buffer[1]) << 8) | buffer[0]);
    *y = (short)((((unsigned short)buffer[3]) << 8) | buffer[2]);
    *z = (short)((((unsigned short)buffer[5]) << 8) | buffer[4]);
}

void ADXL345::getAcceleration(double* xyz) {
    int i;
    int xyz_int[3];
    readAccel(xyz_int);
    for (i = 0; i < 3; i++) {
        xyz[i] = xyz_int[i] * gains[i];
    }
}
// Writes val to address register on device
void ADXL345::writeTo(uint8_t address, uint8_t val) {
    Wire.beginTransmission(ADXL345_DEVICE); // start transmission to device
    Wire.write(address);             // send register address
    Wire.write(val);                 // send value to write
    Wire.endTransmission();         // end transmission
}

// Reads num uint8_ts starting from address register on device in to buffer array
void ADXL345::readFrom(uint8_t address, int num, uint8_t buffer[]) {
    Wire.beginTransmission(ADXL345_DEVICE); // start transmission to device
    Wire.write(address);             // sends address to read from
    Wire.endTransmission();         // end transmission

    Wire.beginTransmission(ADXL345_DEVICE); // start transmission to device
    Wire.requestFrom(ADXL345_DEVICE, num);    // request 6 uint8_ts from device

    int i = 0;
    while (Wire.available()) {      // device may send less than requested (abnormal)
        buffer[i] = Wire.read();    // receive a uint8_t
        i++;
    }
    if (i != num) {
        status = ADXL345_ERROR;
        error_code = ADXL345_READ_ERROR;
    }
    Wire.endTransmission();         // end transmission
}

// Gets the range setting and return it into rangeSetting
// it can be 2, 4, 8 or 16
void ADXL345::getRangeSetting(uint8_t* rangeSetting) {
    uint8_t _b;
    readFrom(ADXL345_DATA_FORMAT, 1, &_b);
    *rangeSetting = _b & 0B00000011;
}

// Sets the range setting, possible values are: 2, 4, 8, 16
void ADXL345::setRangeSetting(int val) {
    uint8_t _s;
    uint8_t _b;

    switch (val) {
        case 2:
            _s = 0B00000000;
            break;
        case 4:
            _s = 0B00000001;
            break;
        case 8:
            _s = 0B00000010;
            break;
        case 16:
            _s = 0B00000011;
            break;
        default:
            _s = 0B00000000;
    }
    readFrom(ADXL345_DATA_FORMAT, 1, &_b);
    _s |= (_b & 0B11101100);
    writeTo(ADXL345_DATA_FORMAT, _s);
}
// gets the state of the SELF_TEST bit
bool ADXL345::getSelfTestBit() {
    return getRegisterBit(ADXL345_DATA_FORMAT, 7);
}

// Sets the SELF-TEST bit
// if set to 1 it applies a self-test force to the sensor causing a shift in the output data
// if set to 0 it disables the self-test force
void ADXL345::setSelfTestBit(bool selfTestBit) {
    setRegisterBit(ADXL345_DATA_FORMAT, 7, selfTestBit);
}

// Gets the state of the SPI bit
bool ADXL345::getSpiBit() {
    return getRegisterBit(ADXL345_DATA_FORMAT, 6);
}

// Sets the SPI bit
// if set to 1 it sets the device to 3-wire mode
// if set to 0 it sets the device to 4-wire SPI mode
void ADXL345::setSpiBit(bool spiBit) {
    setRegisterBit(ADXL345_DATA_FORMAT, 6, spiBit);
}

// Gets the state of the INT_INVERT bit
bool ADXL345::getInterruptLevelBit() {
    return getRegisterBit(ADXL345_DATA_FORMAT, 5);
}

// Sets the INT_INVERT bit
// if set to 0 sets the interrupts to active high
// if set to 1 sets the interrupts to active low
void ADXL345::setInterruptLevelBit(bool interruptLevelBit) {
    setRegisterBit(ADXL345_DATA_FORMAT, 5, interruptLevelBit);
}

// Gets the state of the FULL_RES bit
bool ADXL345::getFullResBit() {
    return getRegisterBit(ADXL345_DATA_FORMAT, 3);
}

// Sets the FULL_RES bit
// if set to 1, the device is in full resolution mode, where the output resolution increases with the
//   g range set by the range bits to maintain a 4mg/LSB scal factor
// if set to 0, the device is in 10-bit mode, and the range buts determine the maximum g range
//   and scale factor
void ADXL345::setFullResBit(bool fullResBit) {
    setRegisterBit(ADXL345_DATA_FORMAT, 3, fullResBit);
}

// Gets the state of the justify bit
bool ADXL345::getJustifyBit() {
    return getRegisterBit(ADXL345_DATA_FORMAT, 2);
}

// Sets the JUSTIFY bit
// if sets to 1 selects the left justified mode
// if sets to 0 selects right justified mode with sign extension
void ADXL345::setJustifyBit(bool justifyBit) {
    setRegisterBit(ADXL345_DATA_FORMAT, 2, justifyBit);
}

// Sets the THRESH_TAP uint8_t value
// it should be between 0 and 255
// the scale factor is 62.5 mg/LSB
// A value of 0 may result in undesirable behavior
void ADXL345::setTapThreshold(int tapThreshold) {
    tapThreshold = constrain(tapThreshold, 0, 255);
    uint8_t _b = uint8_t(tapThreshold);
    writeTo(ADXL345_THRESH_TAP, _b);
}

// Gets the THRESH_TAP uint8_t value
// return value is comprised between 0 and 255
// the scale factor is 62.5 mg/LSB
int ADXL345::getTapThreshold() {
    uint8_t _b;
    readFrom(ADXL345_THRESH_TAP, 1, &_b);
    return int (_b);
}

// set/get the gain for each axis in Gs / count
void ADXL345::setAxisGains(double* _gains) {
    int i;
    for (i = 0; i < 3; i++) {
        gains[i] = _gains[i];
    }
}
void ADXL345::getAxisGains(double* _gains) {
    int i;
    for (i = 0; i < 3; i++) {
        _gains[i] = gains[i];
    }
}


// Sets the OFSX, OFSY and OFSZ uint8_ts
// OFSX, OFSY and OFSZ are user offset adjustments in twos complement format with
// a scale factor of 15,6mg/LSB
// OFSX, OFSY and OFSZ should be comprised between
void ADXL345::setAxisOffset(int x, int y, int z) {
    writeTo(ADXL345_OFSX, uint8_t(x));
    writeTo(ADXL345_OFSY, uint8_t(y));
    writeTo(ADXL345_OFSZ, uint8_t(z));
}

// Gets the OFSX, OFSY and OFSZ uint8_ts
void ADXL345::getAxisOffset(int* x, int* y, int* z) {
    uint8_t _b;
    readFrom(ADXL345_OFSX, 1, &_b);
    *x = int (_b);
    readFrom(ADXL345_OFSY, 1, &_b);
    *y = int (_b);
    readFrom(ADXL345_OFSZ, 1, &_b);
    *z = int (_b);
}

// Sets the DUR uint8_t
// The DUR uint8_t contains an unsigned time value representing the maximum time
// that an event must be above THRESH_TAP threshold to qualify as a tap event
// The scale factor is 625µs/LSB
// A value of 0 disables the tap/double tap funcitons. Max value is 255.
void ADXL345::setTapDuration(int tapDuration) {
    tapDuration = constrain(tapDuration, 0, 255);
    uint8_t _b = uint8_t(tapDuration);
    writeTo(ADXL345_DUR, _b);
}

// Gets the DUR uint8_t
int ADXL345::getTapDuration() {
    uint8_t _b;
    readFrom(ADXL345_DUR, 1, &_b);
    return int (_b);
}

// Sets the latency (latent register) which contains an unsigned time value
// representing the wait time from the detection of a tap event to the start
// of the time window, during which a possible second tap can be detected.
// The scale factor is 1.25ms/LSB. A value of 0 disables the double tap function.
// It accepts a maximum value of 255.
void ADXL345::setDoubleTapLatency(int doubleTapLatency) {
    uint8_t _b = uint8_t(doubleTapLatency);
    writeTo(ADXL345_LATENT, _b);
}

// Gets the Latent value
int ADXL345::getDoubleTapLatency() {
    uint8_t _b;
    readFrom(ADXL345_LATENT, 1, &_b);
    return int (_b);
}

// Sets the Window register, which contains an unsigned time value representing
// the amount of time after the expiration of the latency time (Latent register)
// during which a second valud tap can begin. The scale factor is 1.25ms/LSB. A
// value of 0 disables the double tap function. The maximum value is 255.
void ADXL345::setDoubleTapWindow(int doubleTapWindow) {
    doubleTapWindow = constrain(doubleTapWindow, 0, 255);
    uint8_t _b = uint8_t(doubleTapWindow);
    writeTo(ADXL345_WINDOW, _b);
}

// Gets the Window register
int ADXL345::getDoubleTapWindow() {
    uint8_t _b;
    readFrom(ADXL345_WINDOW, 1, &_b);
    return int (_b);
}

// Sets the THRESH_ACT uint8_t which holds the threshold value for detecting activity.
// The data format is unsigned, so the magnitude of the activity event is compared
// with the value is compared with the value in the THRESH_ACT register. The scale
// factor is 62.5mg/LSB. A value of 0 may result in undesirable behavior if the
// activity interrupt is enabled. The maximum value is 255.
void ADXL345::setActivityThreshold(int activityThreshold) {
    activityThreshold = constrain(activityThreshold, 0, 255);
    uint8_t _b = uint8_t(activityThreshold);
    writeTo(ADXL345_THRESH_ACT, _b);
}

// Gets the THRESH_ACT uint8_t
int ADXL345::getActivityThreshold() {
    uint8_t _b;
    readFrom(ADXL345_THRESH_ACT, 1, &_b);
    return int (_b);
}

// Sets the THRESH_INACT uint8_t which holds the threshold value for detecting inactivity.
// The data format is unsigned, so the magnitude of the inactivity event is compared
// with the value is compared with the value in the THRESH_INACT register. The scale
// factor is 62.5mg/LSB. A value of 0 may result in undesirable behavior if the
// inactivity interrupt is enabled. The maximum value is 255.
void ADXL345::setInactivityThreshold(int inactivityThreshold) {
    inactivityThreshold = constrain(inactivityThreshold, 0, 255);
    uint8_t _b = uint8_t(inactivityThreshold);
    writeTo(ADXL345_THRESH_INACT, _b);
}

// Gets the THRESH_INACT uint8_t
int ADXL345::getInactivityThreshold() {
    uint8_t _b;
    readFrom(ADXL345_THRESH_INACT, 1, &_b);
    return int (_b);
}

// Sets the TIME_INACT register, which contains an unsigned time value representing the
// amount of time that acceleration must be less thant the value in the THRESH_INACT
// register for inactivity to be declared. The scale factor is 1sec/LSB. The value must
// be between 0 and 255.
void ADXL345::setTimeInactivity(int timeInactivity) {
    timeInactivity = constrain(timeInactivity, 0, 255);
    uint8_t _b = uint8_t(timeInactivity);
    writeTo(ADXL345_TIME_INACT, _b);
}

// Gets the TIME_INACT register
int ADXL345::getTimeInactivity() {
    uint8_t _b;
    readFrom(ADXL345_TIME_INACT, 1, &_b);
    return int (_b);
}

// Sets the THRESH_FF register which holds the threshold value, in an unsigned format, for
// free-fall detection. The root-sum-square (RSS) value of all axes is calculated and
// compared whith the value in THRESH_FF to determine if a free-fall event occured. The
// scale factor is 62.5mg/LSB. A value of 0 may result in undesirable behavior if the free-fall
// interrupt is enabled. The maximum value is 255.
void ADXL345::setFreeFallThreshold(int freeFallThreshold) {
    freeFallThreshold = constrain(freeFallThreshold, 0, 255);
    uint8_t _b = uint8_t(freeFallThreshold);
    writeTo(ADXL345_THRESH_FF, _b);
}

// Gets the THRESH_FF register.
int ADXL345::getFreeFallThreshold() {
    uint8_t _b;
    readFrom(ADXL345_THRESH_FF, 1, &_b);
    return int (_b);
}

// Sets the TIME_FF register, which holds an unsigned time value representing the minimum
// time that the RSS value of all axes must be less than THRESH_FF to generate a free-fall
// interrupt. The scale factor is 5ms/LSB. A value of 0 may result in undesirable behavior if
// the free-fall interrupt is enabled. The maximum value is 255.
void ADXL345::setFreeFallDuration(int freeFallDuration) {
    freeFallDuration = constrain(freeFallDuration, 0, 255);
    uint8_t _b = uint8_t(freeFallDuration);
    writeTo(ADXL345_TIME_FF, _b);
}

// Gets the TIME_FF register.
int ADXL345::getFreeFallDuration() {
    uint8_t _b;
    readFrom(ADXL345_TIME_FF, 1, &_b);
    return int (_b);
}

bool ADXL345::isActivityXEnabled() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 6);
}
bool ADXL345::isActivityYEnabled() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 5);
}
bool ADXL345::isActivityZEnabled() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 4);
}
bool ADXL345::isInactivityXEnabled() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 2);
}
bool ADXL345::isInactivityYEnabled() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 1);
}
bool ADXL345::isInactivityZEnabled() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 0);
}

void ADXL345::setActivityX(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 6, state);
}
void ADXL345::setActivityY(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 5, state);
}
void ADXL345::setActivityZ(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 4, state);
}
void ADXL345::setInactivityX(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 2, state);
}
void ADXL345::setInactivityY(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 1, state);
}
void ADXL345::setInactivityZ(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 0, state);
}

bool ADXL345::isActivityAc() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 7);
}
bool ADXL345::isInactivityAc() {
    return getRegisterBit(ADXL345_ACT_INACT_CTL, 3);
}

void ADXL345::setActivityAc(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 7, state);
}
void ADXL345::setInactivityAc(bool state) {
    setRegisterBit(ADXL345_ACT_INACT_CTL, 3, state);
}

bool ADXL345::getSuppressBit() {
    return getRegisterBit(ADXL345_TAP_AXES, 3);
}
void ADXL345::setSuppressBit(bool state) {
    setRegisterBit(ADXL345_TAP_AXES, 3, state);
}

bool ADXL345::isTapDetectionOnX() {
    return getRegisterBit(ADXL345_TAP_AXES, 2);
}
void ADXL345::setTapDetectionOnX(bool state) {
    setRegisterBit(ADXL345_TAP_AXES, 2, state);
}
bool ADXL345::isTapDetectionOnY() {
    return getRegisterBit(ADXL345_TAP_AXES, 1);
}
void ADXL345::setTapDetectionOnY(bool state) {
    setRegisterBit(ADXL345_TAP_AXES, 1, state);
}
bool ADXL345::isTapDetectionOnZ() {
    return getRegisterBit(ADXL345_TAP_AXES, 0);
}
void ADXL345::setTapDetectionOnZ(bool state) {
    setRegisterBit(ADXL345_TAP_AXES, 0, state);
}

bool ADXL345::isActivitySourceOnX() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 6);
}
bool ADXL345::isActivitySourceOnY() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 5);
}
bool ADXL345::isActivitySourceOnZ() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 4);
}

bool ADXL345::isTapSourceOnX() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 2);
}
bool ADXL345::isTapSourceOnY() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 1);
}
bool ADXL345::isTapSourceOnZ() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 0);
}

bool ADXL345::isAsleep() {
    return getRegisterBit(ADXL345_ACT_TAP_STATUS, 3);
}

bool ADXL345::isLowPower() {
    return getRegisterBit(ADXL345_BW_RATE, 4);
}
void ADXL345::setLowPower(bool state) {
    setRegisterBit(ADXL345_BW_RATE, 4, state);
}

double ADXL345::getRate() {
    uint8_t _b;
    readFrom(ADXL345_BW_RATE, 1, &_b);

    _b &= B00001111;
    return (pow(2, ((int) _b) - 6)) * 6.25;
}

void ADXL345::setRate(double rate) {
    uint8_t _b, _s;
    int v = (int)(rate / 6.25);
    int r = 0;
    while (v >>= 1) {
        r++;
    }
    if (r <= 9) {
        readFrom(ADXL345_BW_RATE, 1, &_b);
        _s = (uint8_t)(r + 6) | (_b & 0B11110000);
        writeTo(ADXL345_BW_RATE, _s);
    }
}

void ADXL345::set_bw(uint8_t bw_code) {
    if ((bw_code < ADXL345_BW_3) || (bw_code > ADXL345_BW_1600)) {
        status = false;
        error_code = ADXL345_BAD_ARG;
    } else {
        writeTo(ADXL345_BW_RATE, bw_code);
    }
}

uint8_t ADXL345::get_bw_code() {
    uint8_t bw_code;
    readFrom(ADXL345_BW_RATE, 1, &bw_code);
    return bw_code;
}





//Used to check if action was triggered in interrupts
//Example triggered(interrupts, ADXL345_SINGLE_TAP);
bool ADXL345::triggered(uint8_t interrupts, int mask) {
    return ((interrupts >> mask) & 1);
}


/*
    ADXL345_DATA_READY
    ADXL345_SINGLE_TAP
    ADXL345_DOUBLE_TAP
    ADXL345_ACTIVITY
    ADXL345_INACTIVITY
    ADXL345_FREE_FALL
    ADXL345_WATERMARK
    ADXL345_OVERRUNY
*/





uint8_t ADXL345::getInterruptSource() {
    uint8_t _b;
    readFrom(ADXL345_INT_SOURCE, 1, &_b);
    return _b;
}

bool ADXL345::getInterruptSource(uint8_t interruptBit) {
    return getRegisterBit(ADXL345_INT_SOURCE, interruptBit);
}

bool ADXL345::getInterruptMapping(uint8_t interruptBit) {
    return getRegisterBit(ADXL345_INT_MAP, interruptBit);
}

// Set the mapping of an interrupt to pin1 or pin2
// eg: setInterruptMapping(ADXL345_INT_DOUBLE_TAP_BIT,ADXL345_INT2_PIN);
void ADXL345::setInterruptMapping(uint8_t interruptBit, bool interruptPin) {
    setRegisterBit(ADXL345_INT_MAP, interruptBit, interruptPin);
}

bool ADXL345::isInterruptEnabled(uint8_t interruptBit) {
    return getRegisterBit(ADXL345_INT_ENABLE, interruptBit);
}

void ADXL345::setInterrupt(uint8_t interruptBit, bool state) {
    setRegisterBit(ADXL345_INT_ENABLE, interruptBit, state);
}

void ADXL345::setRegisterBit(uint8_t regAdress, int bitPos, bool state) {
    uint8_t _b;
    readFrom(regAdress, 1, &_b);
    if (state) {
        _b |= (1 << bitPos);  // forces nth bit of _b to be 1.  all other bits left alone.
    } else {
        _b &= ~(1 << bitPos); // forces nth bit of _b to be 0.  all other bits left alone.
    }
    writeTo(regAdress, _b);
}



bool ADXL345::getRegisterBit(uint8_t regAdress, int bitPos) {
    uint8_t _b;
    readFrom(regAdress, 1, &_b);
    return ((_b >> bitPos) & 1);
}

// print all register value to the serial ouptut, which requires it to be setup
// this can be used to manually to check the current configuration of the device
void ADXL345::printAllRegister() {
    uint8_t _b;
    Serial.print("0x00: ");
    readFrom(0x00, 1, &_b);
    print_uint8_t(_b);
    Serial.println("");
    int i;
    for (i = 29; i <= 57; i++) {
        Serial.print("0x");
        Serial.print(i, HEX);
        Serial.print(": ");
        readFrom(i, 1, &_b);
        print_uint8_t(_b);
        Serial.println("");
    }
}

// set the operation mode

void ADXL345::setMode(uint8_t operationMode) {
    uint8_t _b;
    readFrom(ADXL345_FIFO_CTL, 1, &_b);
    _b &= ~(0b11000000); //clearing bit 6 and 7
    _b |= (operationMode << 6); //setting op mode


    //setRegisterBit(ADXL345_FIFO_CTL, 6, operationMode);
    writeTo(ADXL345_FIFO_CTL, _b);
}
// readback mode

uint8_t ADXL345::getMode(void) {
    uint8_t _b;
    readFrom(ADXL345_FIFO_CTL, 1, &_b);
    _b &= 0b11000000;  //masking bit 6 and 7
    _b = (_b >> 6); //setting op mode
    return _b;
}

// set watermark

void ADXL345::setWatermark(uint8_t watermark) {
    uint8_t _b, _w;

    readFrom(ADXL345_FIFO_CTL, 1, &_b);
    _b &= (0b11100000); //clearing bit 0 to 4
    _w = watermark & (0b00011111);  //clearing highest 3 bits in waterlevel
    _b |= _w; //setting waterlevel in operationmode register
    //setRegisterBit(ADXL345_FIFO_CTL, 6, operationMode);
    writeTo(ADXL345_FIFO_CTL, _b);
}

// read how many samples in Fifi

uint8_t ADXL345::getFifoEntries(void) {
    uint8_t _b;
    readFrom(ADXL345_FIFO_STATUS, 1, &_b);
    _b &=  0b00111111;

    return _b;
}

void ADXL345::burstReadXYZ(int* x, int* y, int* z, uint8_t samples) {
    for (int i = 0; i < samples; i++) {
        readFrom(ADXL345_DATAX0, ADXL345_TO_READ, buffer); //read the acceleration data from the ADXL345
        x[i] = (short)((((unsigned short)buffer[1]) << 8) | buffer[0]);
        y[i] = (short)((((unsigned short)buffer[3]) << 8) | buffer[2]);
        z[i] = (short)((((unsigned short)buffer[5]) << 8) | buffer[4]);
    }
}




void print_uint8_t(uint8_t val) {
    int i;
    Serial.print("B");
    for (i = 7; i >= 0; i--) {
        Serial.print(val >> i & 1, BIN);
    }
}