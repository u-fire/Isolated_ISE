from uFire_ISE import uFire_ISE
import math
POTENTIAL_REGISTER_ADDRESS = 100


class ufire_orp(uFire_ISE):
    ORP = 0
    Eh = 0

    def measureORP(self):
        self.measuremV()
        self.ORP = self.mV
        self.Eh = self.mV + self.getProbePotential()

        if math.isnan(self.ORP):
            self.ORP = -1
            self.Eh = -1
        if math.isinf(self.mV):
            self.ORP = -1
            self.Eh = -1

        return self.mV

    def setProbePotential(self, potential):
        self.writeEEPROM(POTENTIAL_REGISTER_ADDRESS, potential)

    def getProbePotential(self):
        return self.readEEPROM(POTENTIAL_REGISTER_ADDRESS)
