from iseprobe import iseprobe

mv = iseprobe(sda=19, scl=23)

mv.measuremV()
print("mV: " + str(mv.mV))
