from ise_orp import ise_orp

orp = ise_orp(sda=19, scl=23)

orp.measureORP()
print("mV: " + str(orp.mV))
print("Eh: " + str(orp.Eh))
