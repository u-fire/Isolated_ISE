from uFire_pH import uFire_pH

# the passed parameter 3 indicates the third (/dev/i2c-3) I2C device
ph = uFire_pH()

ph.measurepH()

print("mV: " + str(ph.mV))
print("pH: " + str(ph.pH))
print("pOH: " + str(ph.pOH))
