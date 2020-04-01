/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   In ORP measurements, it's just the mV reading of the probe,
   but another measurement takes into account the potential of the probe
   and simply adds it to the mV reading, known as Eh.
 */

 #include <uFire_ORP.h>

uFire_ORP orp;
long probePotential = 295;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  orp.begin();
  orp.setProbePotential(probePotential);
}

void loop() {
  orp.measureORP();
  Serial.print("mV: "); Serial.println(orp.ORP);
  Serial.print("Eh: "); Serial.println(orp.Eh);
  Serial.println("---");
  delay(1000);
}
