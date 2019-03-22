from ise_ph import ise_ph

# the passed parameter 3 indicates the third (/dev/i2c-3) I2C device
ph = ise_ph()

ph.measurepH()

print("mV: " + str(ph.mV))
print("pH: " + str(ph.pH))
print("pOH: " + str(ph.pOH))
