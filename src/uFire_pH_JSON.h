#pragma once

#include <uFire_pH.h>

class uFire_pH_JSON
{
public:
  float value;
  String emptyPlaceholder;
  uFire_pH_JSON(){}
  void begin(uFire_pH *ph);
  String processJSON(String json);
private:
  uFire_pH *ph;
  String ph_reset();
  String ph_connected();
  String ph_single(String);
  String ph_high_ref(String);
  String ph_high_read();
  String ph_low_ref(String);
  String ph_low_read();
  String ph_measure(String);
  String ph_temp();
};

