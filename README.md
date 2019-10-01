### Isolated ISE Probe Interface

> Use any Ion Specific Electrode
- Measure pH with temperature compensation
- Measure ORP and Eh
- Raw mV
- Temperature in Celsius/Fahrenheit
- Electrically Isolated

#### What it is
An I2C slave device that can interface with any voltage based sensor such as a pH or ORP probe. There are pin headers available to connect a DS18B20 temperature sensor for automatic temperature compensation. The firmware on the device provides two calibration options, single or dual point.

#### Using it
There is extensive [documentation](http://ufire.co/docs/uFire_ISE/) on the use and setup of the device. The library to use it is in the Arduino IDE, Particle.io IDE and a python implementation for Raspberry Pi and MicroPython is also available.

~~~
#include "uFire_ISE.h"
uFire_ISE mv;
mv.measuremV();
~~~
~~~
#include "uFire_pH.h"
uFire_pH ph;
ph.measurepH();
~~~
~~~
#include "uFire_ORP.h"
uFire_ORP orp;
orp.measureORP();
~~~

##### Isolation

When different probes are connected to the same controlling device, they can cause interference. The environment also causes interference due to ground-loops or other electrical noise like pumps. Electrically isolating the probe from the controlling device can help to prevent it.

All μFire probe interfaces come with electrical isolation hardware which provides a stable, isolated power supply and I2C bus between the interface and your controlling device.