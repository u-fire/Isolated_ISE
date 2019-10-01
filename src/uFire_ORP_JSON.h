#pragma once

#include <uFire_ORP.h>

class uFire_ORP_JSON
{
public:
  float value;
  String emptyPlaceholder;
  uFire_ORP_JSON(){}
  void begin(ISE_ORP *orp);
  String processJSON(String json);
private:
  ISE_ORP *orp;
  String orp_reset();
  String orp_connected();
  String orp_measure();
  String orp_offset(String);
  String orp_potential(String);
  String orp_temp();
};

