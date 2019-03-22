from ise_orp import ise_orp

# the passed parameter 3 indicates the third (/dev/i2c-3) I2C device
orp = ise_orp()

orp.measureORP()
print("mV: " + str(orp.mV))
print("Eh: " + str(orp.Eh))
