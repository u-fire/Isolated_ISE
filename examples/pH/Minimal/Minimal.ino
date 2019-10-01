/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone
 */

 #include <uFire_pH.h>

uFire_pH ph;

void setup() {
  Serial.begin(9600);
}

void loop() {
  ph.measurepH();
  Serial.print("pH: "); Serial.println(ph.pH);
  Serial.print("pOH: "); Serial.println(ph.pOH);
  Serial.print("mV: "); Serial.println(ph.mV);
  Serial.println();
}
