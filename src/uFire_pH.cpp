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

#include "uFire_pH.h"

float uFire_pH::_measure(float temp)
{
  // Turn mV into pH
  float mv = measuremV();

  if (mv == -1)
  {
    pH  = -1;
    pOH = -1;
    return -1;
  }

  pH = fabs(7.0 - (mv / PROBE_MV_TO_PH));

  // Determine the temperature correction
  if (temp != -127)
  {
    uint8_t distance_from_7  = abs(7 - round(pH));
    uint8_t distance_from_25 = floor(abs(25 - round(temp)) / 10);
    float   temp_multiplier  = (distance_from_25 * distance_from_7) * TEMP_CORRECTION_FACTOR;
    if ((pH >= 8.0) && (temp >= 35))
    {
      // negative
      temp_multiplier *= -1;
    }
    if ((pH <= 6.0) && (temp <= 15))
    {
      // negative
      temp_multiplier *= -1;
    }
    pH += temp_multiplier;
  }

  pOH = fabs(pH - 14);

  if ((pH <= 0.0) || (pH > 14.0)) {
    pH  = -1;
    pOH = -1;
  }
  if (isinf(pH)) {
    pH  = -1;
    pOH = -1;
  }
  if (isnan(pH)) {
    pH  = -1;
    pOH = -1;
  }

  return pH;
}

float uFire_pH::measurepH(float temp)
{
  useTemperatureCompensation(true);
  return _measure(temp);
}

float uFire_pH::measurepH()
{
  return _measure(-127);
}

float uFire_pH::pHtomV(float pH)
{
  return (7 - pH) * PROBE_MV_TO_PH;
}

float uFire_pH::mVtopH(float mV)
{
  return fabs(7.0 - (mV / PROBE_MV_TO_PH));
}

float uFire_pH::calibrateSingle(float solutionpH)
{
  uFire_ISE::calibrateSingle(pHtomV(solutionpH));

  return uFire_ISE::getCalibrateOffset();
}

float uFire_pH::calibrateProbeLow(float solutionpH)
{
  uFire_ISE::calibrateProbeLow(pHtomV(solutionpH));

  return mVtopH(uFire_ISE::getCalibrateLowReading());
}

float uFire_pH::getCalibrateLowReference()
{
  return mVtopH(uFire_ISE::getCalibrateLowReference());
}

float uFire_pH::getCalibrateLowReading()
{
  return mVtopH(uFire_ISE::getCalibrateLowReading());
}

float uFire_pH::calibrateProbeHigh(float solutionpH)
{
  uFire_ISE::calibrateProbeHigh(pHtomV(solutionpH));

  return mVtopH(uFire_ISE::getCalibrateHighReading());
}

float uFire_pH::getCalibrateHighReference()
{
  return mVtopH(uFire_ISE::getCalibrateHighReference());
}

float uFire_pH::getCalibrateHighReading()
{
  return mVtopH(uFire_ISE::getCalibrateHighReading());
}
