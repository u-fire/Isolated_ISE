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

/*!
   \file ISE_Probe.cpp
   \brief ISE Probe Class Implementation

   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire/ISE_Probe for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone
 */

#ifndef ISE_PH_H
#define ISE_PH_H

#include <math.h>
#include "uFire_ISE.h"

#define PROBE_MV_TO_PH 59.2
#define TEMP_CORRECTION_FACTOR 0.03

class uFire_pH : public uFire_ISE {
public:

  float pH;
  float pOH;
  uFire_pH() {}

  uFire_pH(uint8_t i2c_address) : uFire_ISE(i2c_address) {}

  #ifdef ESP32
  uFire_pH(uint8_t sda,
         uint8_t scl,
         uint8_t i2c_address) : ISE_Probe(sda, scl, i2c_address) {}

  uFire_pH(uint8_t sda,
         uint8_t scl) : ISE_Probe(sda, scl) {}
  #endif // ifndef ESP32
  float measurepH();
  float measurepH(float temp);
  float pHtomV(float pH);
  float mVtopH(float mV);
  void  calibrateSingle(float solutionpH);
  void  calibrateProbeLow(float solutionpH);
  float getCalibrateLowReference();
  float getCalibrateLowReading();
  void  calibrateProbeHigh(float solutionpH);
  float getCalibrateHighReference();
  float getCalibrateHighReading();

private:

  float _measure(float temp);
};

// for older code
class ISE_pH: public uFire_pH{
public:
  ISE_pH() {}
  ISE_pH(uint8_t i2c_address) : uFire_pH(i2c_address) {}

  #ifdef ESP32
  ISE_pH(uint8_t sda,
         uint8_t scl,
         uint8_t i2c_address) : uFire_pH(sda, scl, i2c_address) {}

  ISE_pH(uint8_t sda,
         uint8_t scl) : uFire_pH(sda, scl) {}
  #endif // ifndef ESP32
};


  #endif // ifndef ISEPROBE_H
