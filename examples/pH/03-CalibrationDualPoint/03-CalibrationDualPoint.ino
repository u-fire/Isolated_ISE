/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   For hardware version 2, firmware 3
 */

#include <uFire_pH.h>
uFire_pH ph;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // start the ISE board
  //https://ufire.co/docs/uFire_ISE/api.html#begin
  ph.begin();
  
  // reset the calibration data
  //https://ufire.co/docs/uFire_ISE/api.html#reset
  ph.reset();
  
  // Go over the steps here: https://ufire.co/docs/uFire_ISE/#dual-point
  // To follow the above steps as simply as possible, this example will be
  // compiled and ran twice. Once for the low point, then again
  // after commenting/uncommenting the lines below, for the high point

  // First, calibrate the low point
  // https://ufire.co/docs/uFire_ISE/api.html#calibrateprobelow

  ph.calibrateProbeLow(4.0);
  // ^ comment this line after the low calibration

  // Second, calibrate the high point
  // https://ufire.co/docs/uFire_ISE/api.html#calibrateprobehigh

  // ph.calibrateProbeHigh(7.0);
  // ^ uncomment this line after low calibration
}

void loop()
{
  // https://ufire.co/docs/uFire_ISE/api.html#measureph
  Serial.println((String)"pH: " + ph.measurepH());
  delay(1000);
}