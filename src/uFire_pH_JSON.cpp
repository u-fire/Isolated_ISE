#if __has_include("ArduinoJson.h")
#include "uFire_pH_JSON.h"
#include <ArduinoJson.h>

void uFire_pH_JSON::begin(uFire_pH *p_ph)
{
  ph = p_ph;
  emptyPlaceholder = "-";
}

String uFire_pH_JSON::processJSON(String rx_string)
{
  String cmd = rx_string.substring(0, rx_string.indexOf(" ", 0));
  cmd.trim();
  rx_string.remove(0, rx_string.indexOf(" ", 0));
  rx_string.trim();
  String parameter = rx_string.substring(0, rx_string.indexOf(" ", 0));
  parameter.trim();

  String value = "";
  if (cmd == "ph")            value = ph_measure(parameter);
  if (cmd == "phrf")          value = ph_high_ref(parameter);
  if (cmd == "phr")           value = ph_high_read();
  if (cmd == "plrf")          value = ph_low_ref(parameter);
  if (cmd == "plr")           value = ph_low_read();
  if (cmd == "ps")            value = ph_single(parameter);
  if (cmd == "pc")            value = ph_connected();
  if (cmd == "pr")            value = ph_reset();
  if (cmd == "pt")            value = ph_temp();

  if (value != "")
  {
    this->value = value.toFloat();
    return value;
  } else
  {
    this->value = -1;
    return "";
  }
}

String uFire_pH_JSON::ph_reset()
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + 20;
  DynamicJsonDocument doc(bufferSize);
  doc["pr"] = "pr";
  ph->reset();
  serializeJson(doc, output);
  return output;
}

String uFire_pH_JSON::ph_connected()
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + 20;
  DynamicJsonDocument doc(bufferSize);
  doc["pc"] = ph->connected();
  serializeJson(doc, output);
  return output;
}

String uFire_pH_JSON::ph_single(String parameter)
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 20;
  DynamicJsonDocument doc(bufferSize);
  if (parameter.length())
  {
    ph->calibrateSingle(parameter.toFloat());
  }

  float offset = ph->getCalibrateOffset();
  if (isnan(offset)) {
    doc["ps"]  = emptyPlaceholder;
  }
  else {
    doc["ps"] = offset;
  }
  serializeJson(doc, output);
  return output;
}

String uFire_pH_JSON::ph_measure(String parameter)
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 20;
  DynamicJsonDocument doc(bufferSize);
  if (parameter)
  {
    doc["ph"] = floor(ph->measurepH(parameter.toFloat()) * 100.0 + 0.5) / 100.0;
  }
  else
  {
    doc["ph"] = floor(ph->measurepH() * 100.0 + 0.5) / 100.0;

  }
  serializeJson(doc, output);
  return output;
}

String uFire_pH_JSON::ph_low_ref(String parameter)
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 20;
  DynamicJsonDocument doc(bufferSize);

  if (parameter.length())
  {
    ph->calibrateProbeLow(parameter.toFloat());
  }

  float lowReference = ph->getCalibrateLowReference();
  if (isnan(lowReference)) {
    doc["plrf"]  = emptyPlaceholder;
  }
  else {
    doc["plrf"]  = lowReference;
  }
  serializeJson(doc, output);
  return output;

}

String uFire_pH_JSON::ph_low_read()
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 20;
  DynamicJsonDocument doc(bufferSize);

  float lowRead = ph->getCalibrateLowReading();
  if (isnan(lowRead)) {
    doc["plr"]  = emptyPlaceholder;
  }
  else {
    doc["plr"] = lowRead;
  }
  serializeJson(doc, output);
  return output;

}

String uFire_pH_JSON::ph_high_ref(String parameter)
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 20;
  DynamicJsonDocument doc(bufferSize);
  if (parameter.length())
  {
    ph->calibrateProbeHigh(parameter.toFloat());
  }

  float highReference = ph->getCalibrateHighReference();
  if (isnan(highReference)) {
    doc["phrf"]  = emptyPlaceholder;
  }
  else {
    doc["phrf"] = highReference;
  }
  serializeJson(doc, output);
  return output;
}

String uFire_pH_JSON::ph_high_read()
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 20;
  DynamicJsonDocument doc(bufferSize);

  float highReference = ph->getCalibrateHighReading();
  if (isnan(highReference)) {
    doc["phr"]  = emptyPlaceholder;
  }
  else {
    doc["phr"] = highReference;
  }
  serializeJson(doc, output);
  return output;
}

String uFire_pH_JSON::ph_temp()
{
  String output;
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + 20;
  DynamicJsonDocument doc(bufferSize);
  doc["pt"] = ph->measureTemp();
  serializeJson(doc, output);
  return output;
}
#endif

