from uFire_ORP import uFire_ORP

# the passed parameter 3 indicates the third (/dev/i2c-3) I2C device
orp = uFire_ORP()

orp.measureORP()
print("mV: " + str(orp.mV))
print("Eh: " + str(orp.Eh))
