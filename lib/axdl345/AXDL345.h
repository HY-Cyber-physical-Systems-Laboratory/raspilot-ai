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


#ifndef ADXL345_h
#define ADXL345_h

#define ADXL345_ADDRESS_AD0_LOW     0x68
#define AXXL345_DEFAULT_ADDRESS     ADXL345_ADDRESS_AD0_LOW

/* ------- Register names ------- */
#define ADXL345_DEVID 0x00
#define ADXL345_RESERVED1 0x01
#define ADXL345_THRESH_TAP 0x1d
#define ADXL345_OFSX 0x1e
#define ADXL345_OFSY 0x1f
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2a
#define ADXL345_ACT_TAP_STATUS 0x2b
#define ADXL345_BW_RATE 0x2c
#define ADXL345_POWER_CTL 0x2d
#define ADXL345_INT_ENABLE 0x2e
#define ADXL345_INT_MAP 0x2f
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

#define ADXL345_BW_1600 0xF // 1111
#define ADXL345_BW_800  0xE // 1110
#define ADXL345_BW_400  0xD // 1101  
#define ADXL345_BW_200  0xC // 1100
#define ADXL345_BW_100  0xB // 1011  
#define ADXL345_BW_50   0xA // 1010 
#define ADXL345_BW_25   0x9 // 1001 
#define ADXL345_BW_12   0x8 // 1000 
#define ADXL345_BW_6    0x7 // 0111
#define ADXL345_BW_3    0x6 // 0110


/*
    Interrupt PINs
    INT1: 0
    INT2: 1
*/
#define ADXL345_INT1_PIN 0x00
#define ADXL345_INT2_PIN 0x01

/*Interrupt bit position*/
#define ADXL345_INT_DATA_READY_BIT 0x07
#define ADXL345_INT_SINGLE_TAP_BIT 0x06
#define ADXL345_INT_DOUBLE_TAP_BIT 0x05
#define ADXL345_INT_ACTIVITY_BIT   0x04
#define ADXL345_INT_INACTIVITY_BIT 0x03
#define ADXL345_INT_FREE_FALL_BIT  0x02
#define ADXL345_INT_WATERMARK_BIT  0x01
#define ADXL345_INT_OVERRUNY_BIT   0x00

#define ADXL345_DATA_READY 0x07
#define ADXL345_SINGLE_TAP 0x06
#define ADXL345_DOUBLE_TAP 0x05
#define ADXL345_ACTIVITY   0x04
#define ADXL345_INACTIVITY 0x03
#define ADXL345_FREE_FALL  0x02
#define ADXL345_WATERMARK  0x01
#define ADXL345_OVERRUNY   0x00

#define ADXL345_OK    1 // no error
#define ADXL345_ERROR 0 // indicates error is predent

#define ADXL345_NO_ERROR   0 // initial state
#define ADXL345_READ_ERROR 1 // problem reading accel
#define ADXL345_BAD_ARG    2 // bad method argument

#define ADXL345_MODE_BYPASS 0x00 	//0000
#define ADXL345_MODE_FIFO 0x01 		//0001
#define ADXL345_MODE_STREM 0x02 	//0010
#define ADXL345_MODE_TRIGGER 0x03 	//0011

#include <stdint.h>

class ADXL345 {
  public:
    char* pi2cPath;
	  uint8_t devAddr;
    
	  int pi2cFd;
    bool status;           // set when error occurs
    // see error code for details
    char error_code;       // Initial state
    
    double gains[3];        // counts to Gs

    ADXL345();
    void powerOn();
    void readAccel(int* xyx);
    void readXYZ(int* x, int* y, int* z);
    void getAcceleration(double* xyz);

    void setTapThreshold(int tapThreshold);
    int getTapThreshold();
    void setAxisGains(double* _gains);
    void getAxisGains(double* _gains);
    void setAxisOffset(int x, int y, int z);
    void getAxisOffset(int* x, int* y, int* z);
    void setTapDuration(int tapDuration);
    int getTapDuration();
    void setDoubleTapLatency(int doubleTapLatency);
    int getDoubleTapLatency();
    void setDoubleTapWindow(int doubleTapWindow);
    int getDoubleTapWindow();
    void setActivityThreshold(int activityThreshold);
    int getActivityThreshold();
    void setInactivityThreshold(int inactivityThreshold);
    int getInactivityThreshold();
    void setTimeInactivity(int timeInactivity);
    int getTimeInactivity();
    void setFreeFallThreshold(int freeFallthreshold);
    int getFreeFallThreshold();
    void setFreeFallDuration(int freeFallDuration);
    int getFreeFallDuration();

    bool isActivityXEnabled();
    bool isActivityYEnabled();
    bool isActivityZEnabled();
    bool isInactivityXEnabled();
    bool isInactivityYEnabled();
    bool isInactivityZEnabled();
    bool isActivityAc();
    bool isInactivityAc();
    void setActivityAc(bool state);
    void setInactivityAc(bool state);

    bool getSuppressBit();
    void setSuppressBit(bool state);
    bool isTapDetectionOnX();
    void setTapDetectionOnX(bool state);
    bool isTapDetectionOnY();
    void setTapDetectionOnY(bool state);
    bool isTapDetectionOnZ();
    void setTapDetectionOnZ(bool state);

    void setActivityX(bool state);
    void setActivityY(bool state);
    void setActivityZ(bool state);
    void setInactivityX(bool state);
    void setInactivityY(bool state);
    void setInactivityZ(bool state);

    bool isActivitySourceOnX();
    bool isActivitySourceOnY();
    bool isActivitySourceOnZ();
    bool isTapSourceOnX();
    bool isTapSourceOnY();
    bool isTapSourceOnZ();
    bool isAsleep();

    bool isLowPower();
    void setLowPower(bool state);
    double getRate();
    void setRate(double rate);
    void set_bw(uint8_t bw_code);
    uint8_t get_bw_code();


    bool triggered(uint8_t interrupts, int mask);


    uint8_t getInterruptSource();
    bool getInterruptSource(uint8_t interruptBit);
    bool getInterruptMapping(uint8_t interruptBit);
    void setInterruptMapping(uint8_t interruptBit, bool interruptPin);
    bool isInterruptEnabled(uint8_t interruptBit);
    void setInterrupt(uint8_t interruptBit, bool state);


    void setMode(uint8_t modeBit); // setting operation mode
    uint8_t getMode();
    void setWatermark(uint8_t watermark);
    uint8_t getFifoEntries(void);		// reading number of samples from fifo
    void burstReadXYZ(int* x, int* y, int* z, uint8_t samples);  // burst read function for getting all samples from fifo

    void getRangeSetting(uint8_t* rangeSetting);
    void setRangeSetting(int val);
    bool getSelfTestBit();
    void setSelfTestBit(bool selfTestBit);
    bool getSpiBit();
    void setSpiBit(bool spiBit);
    bool getInterruptLevelBit();
    void setInterruptLevelBit(bool interruptLevelBit);
    bool getFullResBit();
    void setFullResBit(bool fullResBit);
    bool getJustifyBit();
    void setJustifyBit(bool justifyBit);
    void printAllRegister();

  private:
    void writeTo(uint8_t address, uint8_t val);
    void readFrom(uint8_t address, int num, uint8_t buff[]);
    void setRegisterBit(uint8_t regAdress, int bitPos, bool state);
    bool getRegisterBit(uint8_t regAdress, int bitPos);
    uint8_t buffer[6] ;    //6 uint8_ts buffer for saving data read from the device
};
void print_uint8_t(uint8_t val);
#endif