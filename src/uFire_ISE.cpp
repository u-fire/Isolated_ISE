// Copyright (c) 2018 Justin Decker

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

/*!
   \brief Class constructor
 */

uFire_ISE::uFire_ISE(uint8_t i2c_address)
{
  _address = i2c_address;
  #ifndef ARDUINO_SAMD_VARIANT_COMPLIANCE
  Wire.begin();
  #endif // ifndef ARDUINO_SAMD_VARIANT_COMPLIANCE
}

uFire_ISE::uFire_ISE()
{
  _address = ISE_PROBE_I2C;
  #ifndef ARDUINO_SAMD_VARIANT_COMPLIANCE
  Wire.begin();
  #endif // ifndef ARDUINO_SAMD_VARIANT_COMPLIANCE
}

#ifdef ESP32
uFire_ISE::uFire_ISE(uint8_t sda, uint8_t scl, uint8_t i2c_address)
{
  _address = i2c_address;
  Wire.begin(sda, scl, 100000);
}

uFire_ISE::uFire_ISE(uint8_t sda, uint8_t scl)
{
  _address = ISE_PROBE_I2C;
  Wire.begin(sda, scl, 100000);
}

#endif // ifndef ESP32

/*!
   \brief Class destructor
 */
uFire_ISE::~uFire_ISE()
{}

/*!
   \code
    uFire_ISE::measuremV();
   \endcode
   \brief Starts a measurement.
   \return mV
 */
float uFire_ISE::measuremV()
{
  _send_command(ISE_MEASURE_MV);
  delay(ISE_MV_MEASURE_TIME);
  mV = _read_register(ISE_MV_REGISTER);

  if (isinf(mV)) {
    mV = -1;
  }
  if (isnan(mV)) {
    mV = -1;
  }

  return mV;
}

/*!
   \code
    uFire_ISE::measureTemp();
   \endcode
   \brief Starts a temperature measurement
   \post #tempC and #tempF are updated
   \note A value of -127 means the device is not connected.
   \return temperature in C
 */
float uFire_ISE::measureTemp()
{
  _send_command(ISE_MEASURE_TEMP);
  delay(ISE_TEMP_MEASURE_TIME);
  tempC = _read_register(ISE_TEMP_REGISTER);
  if (tempC == -127.0)
  {
    tempF = -127;
  }
  else
  {
    tempF = ((tempC * 9) / 5) + 32;
  }
  return tempC;
}

/*!
    \code
        uFire_ISE::setTemp(20.2);
    \endcode

   \brief Sets the temperature used by the device.
   \post #tempC and #tempF are updated
 */
void uFire_ISE::setTemp(float temp_C)
{
  _write_register(ISE_TEMP_REGISTER, temp_C);
  tempC = temp_C;
  tempF = ((tempC * 9) / 5) + 32;
}

/*!
   \code
    uFire_ISE::calibrateSingle(100);
   \endcode
   \brief Calibrates the probe using a single point.
   \param solutionmV          the mV of the calibration solution
   \post result will be saved in the device's EEPROM and used
   automatically thereafter
 */
void uFire_ISE::calibrateSingle(float solutionmV)
{
  _write_register(ISE_SOLUTION_REGISTER, solutionmV);
  _send_command(ISE_CALIBRATE_SINGLE);
  delay(ISE_MV_MEASURE_TIME);
}

/*!
   \code
    uFire_ISE::calibrateProbeLow(50);
   \endcode
   \brief Calibrates the dual-point values for the low reading and saves them
   in the devices's EEPROM.
   \param solutionmV          the mV of the calibration solution
 */
void uFire_ISE::calibrateProbeLow(float solutionmV)
{
  _write_register(ISE_SOLUTION_REGISTER, solutionmV);
  _send_command(ISE_CALIBRATE_LOW);
  delay(ISE_MV_MEASURE_TIME);
}

/*!
   \code
   uFire_ISE::calibrateProbeHigh(400);
   \endcode
   \brief Calibrates the dual-point values for the high reading and saves them
   in the devices's EEPROM.
   \param solutionmV          the mV of the calibration solution
 */
void uFire_ISE::calibrateProbeHigh(float solutionmV)
{
  _write_register(ISE_SOLUTION_REGISTER, solutionmV);
  _send_command(ISE_CALIBRATE_HIGH);
  delay(ISE_MV_MEASURE_TIME);
}

/*!
   \code
   uFire_ISE::setDualPointCalibration(50, 100, 48, 134);
   \endcode
   \brief Sets all the values for dual point calibration and saves them
   in the devices's EEPROM.
   \param refLow            the reference low point
   \param refHigh           the reference high point
   \param readLow           the measured low point
   \param readHigh          the measured high point
 */
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

/*!
   \code
   float z = uFire_ISE::getCalibrateOffset();
   \endcode
   \brief Retrieves the single-point offset from the device.
   \return   the probe's offset
 */
float uFire_ISE::getCalibrateOffset()
{
  return _read_register(ISE_CALIBRATE_SINGLE_REGISTER);
}

/*!
   \code
   float calHigh = uFire_ISE::getCalibrateHigh();
   \endcode
   \brief Retrieves the reference-high calibration data
   \return   the reference-high
 */
float uFire_ISE::getCalibrateHighReference()
{
  return _read_register(ISE_CALIBRATE_REFHIGH_REGISTER);
}

/*!
   \code
   float calHigh = uFire_ISE::getCalibrateHighReading();
   \endcode
   \brief Retrieves the reading-high calibration data
   \return   the reading-high
 */
float uFire_ISE::getCalibrateHighReading()
{
  return _read_register(ISE_CALIBRATE_READHIGH_REGISTER);
}

/*!
   \code
   float calLow = uFire_ISE::getCalibrateLow();
   \endcode
   \brief Retrieves the reference-low calibration data
   \return   the reference-low
 */
float uFire_ISE::getCalibrateLowReference()
{
  return _read_register(ISE_CALIBRATE_REFLOW_REGISTER);
}

/*!
   \code
   float calLow = uFire_ISE::getCalibrateLowReading();
   \endcode
   \brief Retrieves the reference-low calibration data
   \return   the reading-low
 */
float uFire_ISE::getCalibrateLowReading()
{
  return _read_register(ISE_CALIBRATE_READLOW_REGISTER);
}

/*!
   \code
   uFire_ISE::useTemperatureCompensation(true);
   \endcode
   \brief Configures device to use temperature compensation or not
   \param b   true for false
 */
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

/*!
   \code
   uint8_t version = uFire_ISE::getVersion();
   \endcode
   \brief Retrieves the firmware version of the device
   \return   version of firmware
 */
uint8_t uFire_ISE::getVersion()
{
  return _read_byte(ISE_VERSION_REGISTER);
}

/*!
   \brief Retrieves the firmware version of the device
   \return   version of firmware
 */
uint8_t uFire_ISE::getFirmware()
{
  return _read_byte(ISE_FW_VERSION_REGISTER);
}

/*!
   \brief Resets all the stored calibration information.
 */
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

/*!
   \code
    uFire_ISE::setI2CAddress(0x3d);
   \endcode

   \brief Changes the i2c address of the device.

   If the default address of the device needs to be changed, call this function to
   permanently change the address. If you forget the i2c address, you will need
   to use an i2c scanner to recover it.
 */
void uFire_ISE::setI2CAddress(uint8_t i2cAddress)
{
  _write_register(ISE_SOLUTION_REGISTER, i2cAddress);
  _send_command(ISE_I2C);
  _address = i2cAddress;
}

/*!
   \code
    uFire_ISE::readEEPROM(300);
   \endcode

   \brief Utility function to read data from any location in EEPROM.

 */
float uFire_ISE::readEEPROM(uint8_t address)
{
  _write_register(ISE_SOLUTION_REGISTER, address);
  _send_command(ISE_MEMORY_READ);
  return _read_register(ISE_BUFFER_REGISTER);
}

/*!
   \code
    uFire_ISE::writeEEPROM(300, 100);
   \endcode

   \brief Utility function to write any value into EEPROM.
 */
void uFire_ISE::writeEEPROM(uint8_t address, float value)
{
  _write_register(ISE_SOLUTION_REGISTER, address);
  _write_register(ISE_BUFFER_REGISTER,   value);
  _send_command(ISE_MEMORY_WRITE);
}

/*!
   \code
    uFire_ISE::connected();
   \endcode

   \brief Returns true if connected, false is disconnected

 */
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
