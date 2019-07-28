import cmd

from ise_ph import ise_ph

ise = ise_ph()


class iseshell(cmd.Cmd):
    prompt = '> '

    def do_config(self, a):
        """prints out all the configuration data\nparameters: none"""
        print("config: ")
        print("\toffset: " + str(ise.getCalibrateOffset()))
        print("\tdual point: ")
        print("\t\tlow reference | read: " + str(ise.getCalibrateLowReference()) + " | " + str(ise.getCalibrateLowReading()))
        print("\t\thigh reference | reading: " + str(ise.getCalibrateHighReference()) + " | " + str(ise.getCalibrateHighReading()))
        print("\tversion: " + str(ise.getVersion()) + "." + str(ise.getFirmware()))

    def do_reset(self, a):
        """reset all saved values\nparameters: none"""
        ise.reset()

    def do_temp(self, temp_C):
        """measures the temperature\nparameters: none"""
        if temp_C:
            ise.setTemp(float(temp_C))
        else:
            ise.measureTemp()

        print("C/F: " + str(ise.tempC) + " / " + str(ise.tempF))

    def do_cal(self, solution_pH):
        """calibrates the device\nparameters:\n\tcalibration solution in mS"""
        if solution_pH:
            ise.calibrateSingle(float(solution_pH))

        print("offset: " + str(ise.getCalibrateOffset()))

    def do_mv(self, a):
        """starts a ph measurement\nparameters: none"""
        ise.measuremV()
        print("mV: " + str(ise.mV))

    def do_ph(self, temp):
        if temp:
            ise.measurepH(float(temp))
        else:
            ise.measurepH()

        print("pH: " + str(ise.pH))

    def do_low(self, low_reference_pH):
        """returns or sets the low reference/reading values\nparameters\n\tlow reference solution in mS"""
        if low_reference_pH:
            ise.calibrateProbeLow(float(low_reference_pH))

        print("\tlow reference / read: " + str(ise.getCalibrateLowReference()
                                               ) + " / " + str(ise.getCalibrateLowReading()))

    def do_high(self, high_reference_pH):
        """returns or sets the high referencen/reading values\nparameters\n\thigh reference solution in mS"""
        if high_reference_pH:
            ise.calibrateProbeHigh(float(high_reference_pH))

        print("\thigh reference / reading: " + str(ise.getCalibrateHighReference()
                                                   ) + " / " + str(ise.getCalibrateHighReading()))

    def do_data(self, a):
        """prints all the data registers"""
        print("mV: " + str(ise.mV))

    def do_version(self, a):
        """prints the version register"""
        print("\tversion: " + str(ise.getVersion()) + "." + str(ise.getFirmware()))

    def do_i2c(self, i2cAddress):
        """changes the I2C address"""
        ise.setI2CAddress(i2cAddress)

    def do_EOF(self, line):
        return True


iseshell().cmdloop()
