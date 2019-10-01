/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   It shows how to interactively calibrate a probe using dual-points.
 */

#include <uFire_ISE.h>

uFire_ISE mv;

void calibrateLow() {
  Serial.println("Put the probe in the reference low solution and wait for the readings to stabilize.");
  Serial.println("Enter what the low solution's measurement should be in mV and press enter.");

  while (Serial.available() == 0) {
    mv.measuremV();
    Serial.print("low mV: "); Serial.println(mv.mV);
  }

  float low_mV = Serial.readStringUntil('/r').toFloat();
  mv.calibrateProbeLow(low_mV);
}

void calibrateHigh() {
  Serial.println("Put the probe in the reference high solution and wait for the readings to stabilize.");
  Serial.println("Enter what the high solution's measurement should be in mV and press enter.");

  while (Serial.available() == 0) {
    mv.measuremV();
    Serial.print("high mV: "); Serial.println(mv.mV);
  }

  float high_mV = Serial.readStringUntil('/r').toFloat();
  mv.calibrateProbeHigh(high_mV);
}

void setup() {
  Serial.begin(9600);
  Serial.flush();
  mv.reset();
  calibrateLow();
  calibrateHigh();
}

void loop() {
  mv.measuremV();
  Serial.print("mV: "); Serial.println(mv.mV);
}
