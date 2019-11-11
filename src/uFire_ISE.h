// Copyright (c) 2017 Justin Decker

//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef ISEPROBE_H
#define ISEPROBE_H

#include <math.h>

#if defined(PARTICLE)
# include "application.h"
# define bitRead(value, bit) (((value) >> (bit)) & 0x01)
# define bitSet(value, bit) ((value) |= (1UL << (bit)))
# define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
# define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#else // if defined(PARTICLE)
# include <Arduino.h>
# include <Wire.h>
#endif // if defined(PARTICLE)

#define ISE_PROBE_I2C 0x3F
#define ISE_MEASURE_MV 80
#define ISE_MEASURE_TEMP 40
#define ISE_CALIBRATE_SINGLE 20
#define ISE_CALIBRATE_LOW 10
#define ISE_CALIBRATE_HIGH 8
#define ISE_MEMORY_WRITE 4
#define ISE_MEMORY_READ 2
#define ISE_I2C 1

#define ISE_VERSION_REGISTER 0             /*!< version */
#define ISE_MV_REGISTER 1                  /*!< mV */
#define ISE_TEMP_REGISTER 5                /*!< temperature in C */
#define ISE_CALIBRATE_SINGLE_REGISTER 9    /*!< calibration offset */
#define ISE_CALIBRATE_REFHIGH_REGISTER 13  /*!< reference high calibration */
#define ISE_CALIBRATE_REFLOW_REGISTER 17   /*!< reference low calibration */
#define ISE_CALIBRATE_READHIGH_REGISTER 21 /*!< reading high calibration */
#define ISE_CALIBRATE_READLOW_REGISTER 25  /*!< reading low calibration */
#define ISE_SOLUTION_REGISTER  29          /*!< reference ISE solution */
#define ISE_BUFFER_REGISTER 33             /*!< buffer */
#define ISE_FW_VERSION_REGISTER 37
#define ISE_CONFIG_REGISTER 38             /*!< config register */
#define ISE_TASK_REGISTER 39               /*!< task register */

#define ISE_TEMP_MEASURE_TIME 750
#define ISE_MV_MEASURE_TIME 250

#define ISE_DUALPOINT_CONFIG_BIT 0         /*!< dual point config bit */
#define ISE_TEMP_COMPENSATION_CONFIG_BIT 1 /*!< temperature compensation config bit */

class uFire_ISE                            /*! ISE Class */
{
public:

  uint8_t _address;
  float tempC; /*!< Temperature in C */
  float tempF; /*!< Temperature in F */
  float mV;    /*!< mV of probe */
  uFire_ISE(uint8_t i2c_address);
  uFire_ISE();
  #ifdef ESP32
  uFire_ISE(uint8_t sda,
            uint8_t scl,
            uint8_t i2c_address);
  uFire_ISE(uint8_t sda,
            uint8_t scl);
  #endif // ifndef ESP32
  ~uFire_ISE();
  float measuremV();
  float measureTemp();
  float  calibrateSingle(float solutionmV);
  float  calibrateProbeLow(float solutionmV);
  float  calibrateProbeHigh(float solutionmV);
  void  setDualPointCalibration(float refLow,
                                float refHigh,
                                float readLow,
                                float readHigh);
  float   getCalibrateOffset();
  void    useTemperatureCompensation(bool b);
  void    setTemp(float temp_C);
  float   getCalibrateHighReference();
  float   getCalibrateLowReference();
  float   getCalibrateHighReading();
  float   getCalibrateLowReading();
  uint8_t getVersion();
  uint8_t getFirmware();
  void    reset();
  void    setI2CAddress(uint8_t i2cAddress);
  void    writeEEPROM(uint8_t address,
                      float   value);
  float   readEEPROM(uint8_t address);
  bool    connected();

private:

  void    _change_register(uint8_t register);
  void    _send_command(uint8_t command);
  void    _write_register(uint8_t reg,
                          float   f);
  void    _write_byte(uint8_t reg,
                      uint8_t val);
  float   _read_register(uint8_t reg);
  uint8_t _read_byte(uint8_t reg);
};

class ISE_Probe : public uFire_ISE {};
#endif // ifndef ISEPROBE_H
