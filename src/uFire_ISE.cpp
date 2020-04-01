// Copyright (c) 2018-2020 Justin Decker

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

#include <math.h>
#include "uFire_ISE.h"

bool uFire_ISE::begin(uint8_t address, TwoWire &wirePort)
{
  _address = address;
  _i2cPort = &wirePort;

  return connected();
}

float uFire_ISE::measuremV()
{
  _send_command(ISE_MEASURE_MV);
  if(_blocking) delay(ISE_MV_MEASURE_TIME);
  _updateRegisters();

  return mV;
}

float uFire_ISE::measureTemp()
{
  _send_command(ISE_MEASURE_TEMP);
  if(_blocking) delay(ISE_TEMP_MEASURE_TIME);
  _updateRegisters();

  return tempC;

}

void uFire_ISE::setTemp(float temp_C)
{
  _write_register(ISE_TEMP_REGISTER, temp_C);
  tempC = temp_C;
  tempF = ((tempC * 9) / 5) + 32;
}

float uFire_ISE::calibrateSingle(float solutionmV)
{
  _write_register(ISE_SOLUTION_REGISTER, solutionmV);
  _send_command(ISE_CALIBRATE_SINGLE);
  if(_blocking) delay(ISE_MV_MEASURE_TIME);

  return getCalibrateOffset();
}

float uFire_ISE::calibrateProbeLow(float solutionmV)
{
  _write_register(ISE_SOLUTION_REGISTER, solutionmV);
  _send_command(ISE_CALIBRATE_LOW);
  if(_blocking) delay(ISE_MV_MEASURE_TIME);

  return getCalibrateLowReading();
}

float uFire_ISE::calibrateProbeHigh(float solutionmV)
{
  _write_register(ISE_SOLUTION_REGISTER, solutionmV);
  _send_command(ISE_CALIBRATE_HIGH);
  if(_blocking) delay(ISE_MV_MEASURE_TIME);

  return getCalibrateHighReading();
}

void uFire_ISE::setDualPointCalibration(float refLow,
                                        float refHigh,
                                        float readLow,
                                        float readHigh)
{
  _write_register(ISE_CALIBRATE_REFLOW_REGISTER,   refLow);
  _write_register(ISE_CALIBRATE_REFHIGH_REGISTER,  refHigh);
  _write_register(ISE_CALIBRATE_READLOW_REGISTER,  readLow);
  _write_register(ISE_CALIBRATE_READHIGH_REGISTER, readHigh);
}

float uFire_ISE::getCalibrateOffset()
{
  return _read_register(ISE_CALIBRATE_SINGLE_REGISTER);
}

float uFire_ISE::getCalibrateHighReference()
{
  return _read_register(ISE_CALIBRATE_REFHIGH_REGISTER);
}

float uFire_ISE::getCalibrateHighReading()
{
  return _read_register(ISE_CALIBRATE_READHIGH_REGISTER);
}

float uFire_ISE::getCalibrateLowReference()
{
  return _read_register(ISE_CALIBRATE_REFLOW_REGISTER);
}

float uFire_ISE::getCalibrateLowReading()
{
  return _read_register(ISE_CALIBRATE_READLOW_REGISTER);
}

void uFire_ISE::useTemperatureCompensation(bool b)
{
  uint8_t retval;
  uint8_t config = _read_byte(ISE_CONFIG_REGISTER);

  if (b)
  {
    retval = bitSet(config, ISE_TEMP_COMPENSATION_CONFIG_BIT);
  }
  else
  {
    retval = bitClear(config, ISE_TEMP_COMPENSATION_CONFIG_BIT);
  }
  _write_byte(ISE_CONFIG_REGISTER, retval);
}

uint8_t uFire_ISE::getVersion()
{
  return _read_byte(ISE_VERSION_REGISTER);
}

uint8_t uFire_ISE::getFirmware()
{
  return _read_byte(ISE_FW_VERSION_REGISTER);
}

void uFire_ISE::reset()
{
  _write_register(ISE_CALIBRATE_SINGLE_REGISTER, NAN);
  delay(10);
  _write_register(ISE_CALIBRATE_REFHIGH_REGISTER, NAN);
  delay(10);
  _write_register(ISE_CALIBRATE_REFLOW_REGISTER, NAN);
  delay(10);
  _write_register(ISE_CALIBRATE_READHIGH_REGISTER, NAN);
  delay(10);
  _write_register(ISE_CALIBRATE_READLOW_REGISTER, NAN);
  delay(10);
  useTemperatureCompensation(false);
}

void uFire_ISE::setI2CAddress(uint8_t i2cAddress)
{
  _write_register(ISE_SOLUTION_REGISTER, i2cAddress);
  _send_command(ISE_I2C);
  _address = i2cAddress;
}

float uFire_ISE::readEEPROM(uint8_t address)
{
  _write_register(ISE_SOLUTION_REGISTER, address);
  _send_command(ISE_MEMORY_READ);
  return _read_register(ISE_BUFFER_REGISTER);
}

void uFire_ISE::writeEEPROM(uint8_t address, float value)
{
  _write_register(ISE_SOLUTION_REGISTER, address);
  _write_register(ISE_BUFFER_REGISTER,   value);
  _send_command(ISE_MEMORY_WRITE);
}

bool uFire_ISE::connected()
{
  uint8_t retval = _read_byte(ISE_VERSION_REGISTER);

  if (retval != 0xFF) {
    return true;
  }
  else {
    return false;
  }
}

void uFire_ISE::setBlocking(bool b)
{
   _blocking = b;
}

bool uFire_ISE::getBlocking()
{
    return _blocking;
}

void uFire_ISE::readData()
{
  _updateRegisters();
  getCalibrateHighReading();
  getCalibrateHighReference();
  getCalibrateLowReading();
  getCalibrateLowReference();
  getCalibrateOffset();
}

void uFire_ISE::_updateRegisters()
{
  mV = _read_register(ISE_MV_REGISTER);
  if (isinf(mV)) {
    mV = -1;
  }
  if (isnan(mV)) {
    mV = -1;
  }

  tempC = _read_register(ISE_TEMP_REGISTER);
  if (tempC == -127.0)
  {
    tempF = -127;
  }
  else
  {
    tempF = ((tempC * 9) / 5) + 32;
  }

}

void uFire_ISE::_change_register(uint8_t r)
{
  Wire.beginTransmission(_address);
  Wire.write(r);
  Wire.endTransmission();
  delay(10);
}

void uFire_ISE::_send_command(uint8_t command)
{
  Wire.beginTransmission(_address);
  Wire.write(ISE_TASK_REGISTER);
  Wire.write(command);
  Wire.endTransmission();
  delay(10);
}

void uFire_ISE::_write_register(uint8_t reg, float f)
{
  uint8_t b[5];
  float   f_val = f;

  b[0] = reg;
  b[1] = *((uint8_t *)&f_val);
  b[2] = *((uint8_t *)&f_val + 1);
  b[3] = *((uint8_t *)&f_val + 2);
  b[4] = *((uint8_t *)&f_val + 3);
  Wire.beginTransmission(_address);
  Wire.write(b, 5);
  Wire.endTransmission();
  delay(10);
}

float uFire_ISE::_read_register(uint8_t reg)
{
  float retval;

  _change_register(reg);

  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 1) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 2) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 3) = Wire.read();
  delay(10);
  return retval;
}

void uFire_ISE::_write_byte(uint8_t reg, uint8_t val)
{
  uint8_t b[5];

  b[0] = reg;
  b[1] = val;
  Wire.beginTransmission(_address);
  Wire.write(b, 2);
  Wire.endTransmission();
  delay(10);
}

uint8_t uFire_ISE::_read_byte(uint8_t reg)
{
  uint8_t retval;

  _change_register(reg);
  Wire.requestFrom(_address, (uint8_t)1);
  retval = Wire.read();
  delay(10);
  return retval;
}
