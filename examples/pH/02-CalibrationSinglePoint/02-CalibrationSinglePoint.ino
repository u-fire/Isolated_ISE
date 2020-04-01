/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   For hardware version 2, firmware 2
 */

 #include <uFire_pH.h>

uFire_pH ph;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // https://ufire.co/docs/uFire_ISE/api.html#begin
  ph.begin();

  // reset the calibration data
  //https://ufire.co/docs/uFire_ISE/api.html#reset
  ph.reset();
  
  // pH value of the calibration solution.
  // https://ufire.co/docs/uFire_ISE/ph.html#calibratesingle
  ph.calibrateSingle(7.0);
}

void loop() {
  // https://ufire.co/docs/uFire_ISE/ph.html#measureph
  ph.measurepH();
  
  Serial.println((String) "pH: " + ph.pH);
  delay(1000);
}