import math
import struct
import time
import smbus  # pylint: disable=E0401

global i2c

ISE_PROBE = 0x3F

ISE_MEASURE_MV = 80
ISE_MEASURE_TEMP = 40
ISE_CALIBRATE_SINGLE = 20
ISE_CALIBRATE_LOW = 10
ISE_CALIBRATE_HIGH = 8
ISE_MEMORY_WRITE = 4
ISE_MEMORY_READ = 2
ISE_I2C = 1

ISE_VERSION_REGISTER = 0             # hardware version */
ISE_MV_REGISTER = 1                  # mV */
ISE_TEMP_REGISTER = 5                # temperature in C */
ISE_CALIBRATE_SINGLE_REGISTER = 9    # calibration offset */
ISE_CALIBRATE_REFHIGH_REGISTER = 13  # reference high calibration */
ISE_CALIBRATE_REFLOW_REGISTER = 17   # reference low calibration */
ISE_CALIBRATE_READHIGH_REGISTER = 21  # reading high calibration */
ISE_CALIBRATE_READLOW_REGISTER = 25  # reading low calibration */
ISE_SOLUTION_REGISTER = 29          # reference ISE solution */
ISE_BUFFER_REGISTER = 33             # buffer */
ISE_FW_VERSION_REGISTER = 37         # firmware version
ISE_CONFIG_REGISTER = 38             # config register */
ISE_TASK_REGISTER = 39               # task register */

ISE_TEMP_MEASURE_TIME = 750
ISE_MV_MEASURE_TIME = 250

ISE_DUALPOINT_CONFIG_BIT = 0
ISE_TEMP_COMPENSATION_CONFIG_BIT = 1


class uFire_ISE(object):
    mV = 0
    tempC = 0
    tempF = 0
    address = ISE_PROBE

    def __init__(self, address=ISE_PROBE, i2c_bus=3, **kwargs):
        global i2c
        self.address = address
        i2c = smbus.SMBus(i2c_bus)

    def measuremV(self):
        self._send_command(ISE_MEASURE_MV)
        time.sleep(ISE_MV_MEASURE_TIME / 1000.0)
        self.mV = self._read_register(ISE_MV_REGISTER)
        self.mV = round(self.mV * 100.0) / 100.0

        if math.isnan(self.mV):
            self.mV = -1
        if math.isinf(self.mV):
            self.mV = -1

        return self.mV

    def measureTemp(self):
        self._send_command(ISE_MEASURE_TEMP)
        time.sleep(ISE_TEMP_MEASURE_TIME / 1000.0)
        self.tempC = self._read_register(ISE_TEMP_REGISTER)

        if self.tempC == -127.0:
            self.tempF = -127.0
        else:
            self.tempF = ((self.tempC * 9) / 5) + 32

        return self.tempC

    def setTemp(self, temp_C):
        self._write_register(ISE_TEMP_REGISTER, temp_C)
        self.tempC = temp_C
        self.tempF = ((self.tempC * 9) / 5) + 32

    def calibrateSingle(self, solutionmV):
        self._write_register(ISE_SOLUTION_REGISTER, solutionmV)
        self._send_command(ISE_CALIBRATE_SINGLE)
        time.sleep(ISE_MV_MEASURE_TIME / 1000.0)

    def calibrateProbeLow(self, solutionmV):
        self._write_register(ISE_SOLUTION_REGISTER, solutionmV)
        self._send_command(ISE_CALIBRATE_LOW)
        time.sleep(ISE_MV_MEASURE_TIME / 1000.0)

    def calibrateProbeHigh(self, solutionmV):
        self._write_register(ISE_SOLUTION_REGISTER, solutionmV)
        self._send_command(ISE_CALIBRATE_HIGH)
        time.sleep(ISE_MV_MEASURE_TIME / 1000.0)

    def getVersion(self):
        return self._read_byte(ISE_VERSION_REGISTER)

    def getCalibrateOffset(self):
        return self._read_register(ISE_CALIBRATE_SINGLE_REGISTER)

    def getCalibrateHighReference(self):
        return self._read_register(ISE_CALIBRATE_REFHIGH_REGISTER)

    def getCalibrateLowReference(self):
        return self._read_register(ISE_CALIBRATE_REFLOW_REGISTER)

    def getCalibrateHighReading(self):
        return self._read_register(ISE_CALIBRATE_READHIGH_REGISTER)

    def getCalibrateLowReading(self):
        return self._read_register(ISE_CALIBRATE_READLOW_REGISTER)

    def reset(self):
        n = float('nan')
        self._write_register(ISE_CALIBRATE_SINGLE_REGISTER, n)
        self._write_register(ISE_CALIBRATE_REFHIGH_REGISTER, n)
        self._write_register(ISE_CALIBRATE_REFLOW_REGISTER, n)
        self._write_register(ISE_CALIBRATE_READHIGH_REGISTER, n)
        self._write_register(ISE_CALIBRATE_READLOW_REGISTER, n)

    def setDualPointCalibration(self, refLow, refHigh, readLow, readHigh):
        self._write_register(ISE_CALIBRATE_REFLOW_REGISTER, refLow)
        self._write_register(ISE_CALIBRATE_REFHIGH_REGISTER, refHigh)
        self._write_register(ISE_CALIBRATE_READLOW_REGISTER, readLow)
        self._write_register(ISE_CALIBRATE_READHIGH_REGISTER, readHigh)

    def setI2CAddress(self, i2cAddress):
        if i2cAddress >= 1 and i2cAddress <= 127:
            self._write_register(ISE_SOLUTION_REGISTER, float(i2cAddress))
            self._send_command(ISE_I2C)
            self.address = int(i2cAddress)

    def useTemperatureCompensation(self, b):
        retval = self._read_byte(ISE_CONFIG_REGISTER)

        retval = self._bit_set(retval, ISE_TEMP_COMPENSATION_CONFIG_BIT, b)
        self._write_byte(ISE_CONFIG_REGISTER, retval)

    def readEEPROM(self, address):
        self._write_register(ISE_SOLUTION_REGISTER, int(address))
        self._send_command(ISE_MEMORY_READ)
        return self._read_register(ISE_BUFFER_REGISTER)

    def writeEEPROM(self, address, value):
        self._write_register(ISE_SOLUTION_REGISTER, int(address))
        self._write_register(ISE_BUFFER_REGISTER, float(value))
        self._send_command(ISE_MEMORY_WRITE)

    def getFirmware(self):
        return self._read_byte(ISE_FW_VERSION_REGISTER)

    def _bit_set(self, v, index, x):
        mask = 1 << index
        v &= ~mask
        if x:
            v |= mask
        return v

    def _change_register(self, r):
        global i2c
        i2c.write_byte(self.address, r)
        time.sleep(10 / 1000.0)

    def _send_command(self, command):
        global i2c
        i2c.write_byte_data(self.address, ISE_TASK_REGISTER, command)
        time.sleep(10 / 1000.0)

    def _write_register(self, reg, f):
        global i2c
        n = self.round_total_digits(f)
        fd = bytearray(struct.pack("f", n))
        data = [0, 0, 0, 0]
        data[0] = fd[0]
        data[1] = fd[1]
        data[2] = fd[2]
        data[3] = fd[3]
        self._change_register(reg)
        i2c.write_i2c_block_data(self.address, reg, data)
        time.sleep(10 / 1000.0)

    def _read_register(self, reg):
        global i2c
        data = [0, 0, 0, 0]
        self._change_register(reg)
        data[0] = i2c.read_byte(self.address)
        data[1] = i2c.read_byte(self.address)
        data[2] = i2c.read_byte(self.address)
        data[3] = i2c.read_byte(self.address)
        ba = bytearray(data)
        f = struct.unpack('f', ba)[0]
        return self.round_total_digits(f)

    def _write_byte(self, reg, val):
        global i2c
        i2c.write_byte_data(self.address, reg, val)
        time.sleep(10 / 1000.0)

    def _read_byte(self, reg):
        global i2c
        self._change_register(reg)
        time.sleep(10 / 1000.0)
        return i2c.read_byte(self.address)

    def magnitude(self, x):
        if math.isnan(x):
            return 0
        return 0 if x == 0 else int(math.floor(math.log10(abs(x)))) + 1

    def round_total_digits(self, x, digits=7):
        return round(x, digits - self.magnitude(x))
