/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone


   This show show to use temperature compensation while taking a pH reading.
   If useTemperatureCompensation is true, a temperature reading is also taken, and an
   adjustment formula is applied. The adjustment is typically slight and not applied within
   certain temperature ranges and pH ranges.

   Keep in mind that temperature compensation in regards to pH adjusts for the probe's 
   resistance change, not the changes that take place in the solution.
 */

 #include <ISE_pH.h>

ISE_pH ph;

void setup() {
  Serial.begin(9600);
}

void loop() {
  ph.measurepH(ph.measureTemp());
  Serial.print("pH: "); Serial.println(ph.pH);
  Serial.println("---");
}
