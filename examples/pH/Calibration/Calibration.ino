/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone
   
   It shows how to interactively calibrate a probe using dual-points.
 */

#include <ISE_pH.h>

ISE_pH pH;

void calibrateLow() {
  Serial.println("Put the probe in the reference low solution and wait for the readings to stabilize.");
  Serial.println("Enter what the low solution's measurement should be in pH and press enter.");

  while (Serial.available() == 0) {
    pH.measurepH();
    Serial.print("low pH: "); Serial.println(pH.pH);
  }

  float low_pH = Serial.readStringUntil('/r').toFloat();
  pH.calibrateProbeLow(low_pH);
}

void calibrateHigh() {
  Serial.println("Put the probe in the reference high solution and wait for the readings to stabilize.");
  Serial.println("Enter what the high solution's measurement should be in pH and press enter.");

  while (Serial.available() == 0) {
    pH.measurepH();
    Serial.print("high pH: "); Serial.println(pH.pH);
  }

  float high_pH = Serial.readStringUntil('/r').toFloat();
  pH.calibrateProbeHigh(high_pH);
}

void setup() {
  Serial.begin(9600);
  Serial.flush();
  pH.reset();
  calibrateLow();
  calibrateHigh();
}

void loop() {
  pH.measurepH();
  Serial.print("pH: "); Serial.println(pH.pH);
}
