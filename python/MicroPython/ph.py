from ise_ph import ise_ph

ph = ise_ph(sda=19, scl=23)

ph.measurepH()
print("mV: " + str(ph.mV))
print("pH: " + str(ph.pH))
print("pOH: " + str(ph.pOH))
