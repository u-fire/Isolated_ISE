from uFire_ISE import uFire_ISE

mv = uFire_ISE()

mv.measuremV()
print("mV: " + str(mv.mV))
