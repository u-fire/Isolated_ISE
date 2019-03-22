/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone
   
   This is compatible with hardware version 2. 

 */

#include <uFire_ISE.h>

ISE_Probe mv;

void setup() {
  Serial.begin(9600);
  Serial.println("Put the probe in the reference solution and wait for the readings to stabilize.");
  Serial.println("Enter what the solution's measurement should be in mV and press enter.");

  mv.reset();
  
  while (Serial.available() == 0) {
    mv.measuremV();
    Serial.print("mV: "); Serial.println(mv.mV);
  }

  mv.calibrateSingle(Serial.readStringUntil('/r').toFloat());
  Serial.print("offset: "); Serial.println(mv.getCalibrateOffset(), 2);
}

void loop() {
  mv.measuremV();
  Serial.print("mV: "); Serial.println(mv.mV);
}
