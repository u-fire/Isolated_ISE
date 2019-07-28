/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This allows you to run various functions on a command-line like interface.
   For more information and documentation, visit ufire.co

   Display config information
    config

   Basic usage:
    mv
    ph

   Single Point Calibration
    cal 4.0

   Dual Point Calibration
    low 4.0
    high 7.0

   Change the I2C address
    i2c 30

   Reset device configuration
    reset
 */

#include <ISE_pH.h>

#ifdef ESP32
ISE_pH pH(19, 23); // sda, scl
#else // ifdef ESP32
ISE_pH pH;
#endif // ifdef ESP32

String buffer, cmd, p1, p2;

void config()
{
  Serial.print("ISE pH Interface: ");
  Serial.println(pH.connected() ? "connected" : "*disconnected*");
  Serial.print("  offset: ");
  Serial.println(pH.getCalibrateOffset(), 4);
  Serial.print("  dual point: ");
  Serial.print("    low reference | read: ");
  Serial.print(pH.getCalibrateLowReference(), 4);
  Serial.print(" | ");
  Serial.println(pH.getCalibrateLowReading(), 4);
  Serial.print("    high reference | read: ");
  Serial.print(pH.getCalibrateHighReference(), 4);
  Serial.print(" | ");
  Serial.println(pH.getCalibrateHighReading(), 4);
  Serial.print("  temp. compensation: ");
  Serial.print("  version: ");
  Serial.print(pH.getVersion(), HEX);
  Serial.print(".");
  Serial.println(pH.getFirmware(), HEX);
}

void reset_config()
{
  pH.reset();
  config();
}

void temperature()
{
  pH.measureTemp();
  Serial.print("C|F: ");
  Serial.print(pH.tempC);
  Serial.print(" | ");
  Serial.println(pH.tempF);
}

void calibrate()
{
  if (p1.length())
  {
    pH.calibrateSingle(p1.toFloat());
  }

  Serial.print("offset: ");
  Serial.println(pH.getCalibrateOffset(), 5);
}

void mv()
{
  while (Serial.available() == 0)
  {
      pH.measuremV();
      Serial.print("mV: ");
      Serial.println(pH.mV, 4);
      delay(1000);
  }
}

void ph()
{
  while (Serial.available() == 0)
  {
      pH.measurepH();
      Serial.print("pH: ");
      Serial.println(pH.pH, 4);
      delay(1000);
  }
}

void data()
{
  Serial.print("mV: ");
  Serial.println(pH.mV);
  Serial.print("C|F: ");
  Serial.print(pH.tempC);
  Serial.print(" |  ");
  Serial.println(pH.tempF);
}

void low()
{
  if (p1.length())
  {
    pH.calibrateProbeLow(p1.toFloat());
  }

  Serial.print("low reference | read: ");
  Serial.print(pH.getCalibrateLowReference(), 2);
  Serial.print(" | ");
  Serial.println(pH.getCalibrateLowReading(), 2);
}

void high()
{
  if (p1.length())
  {
    pH.calibrateProbeHigh(p1.toFloat());
  }

  Serial.print("high reference | read: ");
  Serial.print(pH.getCalibrateHighReference(), 2);
  Serial.print(" | ");
  Serial.println(pH.getCalibrateHighReading(), 2);
}

void i2c()
{
  if (p1.length())
  {
    pH.setI2CAddress(strtoul(p1.c_str(), 0, 16));
  }
}

void read()
{
  if (p1.length())
  {
    Serial.println(pH.readEEPROM(p1.toFloat()));
  }
}

void write()
{
  if (p1.length())
  {
    pH.writeEEPROM(p1.toInt(), p2.toFloat());
  }
}

void cmd_run()
{
  if ((cmd == "conf") || (cmd == "config") || (cmd == "c"))
    config();
  if ((cmd == "reset") || (cmd == "r"))
    reset_config();
  if ((cmd == "temp") || (cmd == "t"))
    temperature();
  if ((cmd == "calibrate") || (cmd == "cal"))
    calibrate();
  if (cmd == "mv")
    mv();
  if (cmd == "ph")
    ph();
  if ((cmd == "data") || (cmd == "d"))
    data();
  if (cmd == "low")
    low();
  if (cmd == "high")
    high();
  if (cmd == "i2c")
    i2c();
  if (cmd == "read")
    read();
  if (cmd == "write")
    write();
}

void cli_process()
{
  while (Serial.available())
  {
    char c = Serial.read();

    switch (c)
    {
    case '\n': // new line
      Serial.println();
      cmd = buffer.substring(0, buffer.indexOf(" ", 0));
      cmd.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p1 = buffer.substring(0, buffer.indexOf(" ", 0));
      p1.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p2 = buffer.substring(0, buffer.indexOf(" ", 0));
      p2.trim();
      cmd_run();

      Serial.print("> ");
      buffer = "";
      break;

    case '\b': // backspace
      buffer.remove(buffer.length() - 1);

      Serial.print("\b \b");
      break;

    default: // everything else
      buffer += c;

      Serial.print(c);
    }
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  config();
  Serial.print("> ");
}

void loop()
{
  cli_process();
}
