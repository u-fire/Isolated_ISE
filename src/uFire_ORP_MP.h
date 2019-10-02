#pragma once

#include <uFire_ORP.h>

class uFire_ORP_MP
{
public:
  float value;
  String emptyPlaceholder;
  uFire_ORP_MP(){}
  void begin(uFire_ORP *orp);
  String processMP(String json);
private:
  uFire_ORP *orp;
  String orp_reset();
  String orp_connected();
  String orp_measure();
  String orp_offset(String);
  String orp_potential(String);
  String orp_temp();
};

