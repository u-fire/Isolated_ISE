from uFire_ISE import uFire_ISE
import math

PROBE_MV_TO_PH = 59.2
TEMP_CORRECTION_FACTOR = 0.03


class uFire_pH(uFire_ISE):
    pH = 0
    pOH = 0

    def measurepH(self, temp=None):
        self.measuremV()
        if self.mV == -1:
            self.pH = -1
            self.pOH = -1
            return -1

        self.pH = abs(7.0 - (self.mV / PROBE_MV_TO_PH))
        if temp is not None:
            distance_from_7 = abs(7 - round(self.pH))
            distance_from_25 = math.floor(abs(25 - round(temp)) / 10)
            temp_multiplier = (distance_from_25 * distance_from_7) * TEMP_CORRECTION_FACTOR
            if (self.pH >= 8.0) and (temp >= 35):
                temp_multiplier *= -1
            if (self.pH <= 6.0) and (temp <= 15):
                temp_multiplier *= -1
            self.pH += temp_multiplier

        self.pOH = abs(self.pH - 14)

        if self.pH <= 0.0 or self.pH >= 14.0:
            self.pH = -1
            self.pOH = -1
        if math.isnan(self.pH):
            self.pH = -1
            self.pOH = -1
        if math.isinf(self.mV):
            self.pH = -1
            self.pOH = -1
        return self.pH

    def calibrateSingle(self, solutionpH):
        super().calibrateSingle(self.pHtomV(solutionpH))

    def calibrateProbeHigh(self, solutionpH):
        super().calibrateProbeHigh(self.pHtomV(solutionpH))

    def getCalibrateHighReference(self):
        return self.mVtopH(super().getCalibrateHighReference())

    def getCalibrateHighReading(self):
        return self.mVtopH(super().getCalibrateHighReading())

    def calibrateProbeLow(self, solutionpH):
        super().calibrateProbeLow(self.pHtomV(solutionpH))

    def getCalibrateLowReference(self):
        return self.mVtopH(super().getCalibrateLowReference())

    def getCalibrateLowReading(self):
        return self.mVtopH(super().getCalibrateLowReading())

    def pHtomV(self, pH):
        return (7 - pH) * PROBE_MV_TO_PH

    def mVtopH(self, mV):
        return abs(7.0 - (mV / PROBE_MV_TO_PH))
