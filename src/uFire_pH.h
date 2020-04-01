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
  float measurepH(float temp=25);
  float pHtomV(float pH);
  float mVtopH(float mV);
  float calibrateSingle(float solutionpH);
  float calibrateProbeLow(float solutionpH);
  float getCalibrateLowReference();
  float getCalibrateLowReading();
  float calibrateProbeHigh(float solutionpH);
  float getCalibrateHighReference();
  float getCalibrateHighReading();
  void  readData();

private:

  float _measure(float temp=25);
  void  _updateRegisters();
};

// for older code
class ISE_pH: public uFire_pH{
public:

};


  #endif // ifndef ISEPROBE_H
