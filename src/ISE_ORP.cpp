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

#include "ISE_ORP.h"


/*!
   \brief Class destructor
 */
ISE_ORP::~ISE_ORP()
{}

/*!
   \code
    ISE_ORP::measureORP();
   \endcode
   \brief Starts an ORP measurement.
   \return ORP
 */
float ISE_ORP::measureORP()
{
  measuremV();
  ORP = mV;
  Eh  = mV + getProbePotential();

  if (isinf(ORP)) {
    mV  = -1;
    ORP = -1;
    Eh  = -1;
  }
  if (isnan(ORP)) {
    mV  = -1;
    ORP = -1;
    Eh  = -1;
  }

  return mV;
}

void ISE_ORP::setProbePotential(uint32_t potential)
{
  writeEEPROM(POTENTIAL_REGISTER_ADDRESS, potential);
}

uint32_t ISE_ORP::getProbePotential()
{
  return readEEPROM(POTENTIAL_REGISTER_ADDRESS);
}
