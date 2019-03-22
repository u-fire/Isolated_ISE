import unittest
import math
from iseprobe import iseprobe

mv = iseprobe(0x3f, 3)


class TestMethods(unittest.TestCase):

    def test_version(self):
        self.assertEqual(mv.getVersion(), 0x1a)

    def test_use_temp_comp_true(self):
        mv.useTemperatureCompensation(True)
        self.assertEqual(mv.usingTemperatureCompensation(), True)

    def test_use_temp_comp_false(self):
        mv.useTemperatureCompensation(False)
        self.assertEqual(mv.usingTemperatureCompensation(), False)

    def test_use_dual_point_true(self):
        mv.useDualPoint(True)
        self.assertEqual(mv.usingDualPoint(), True)

    def test_use_dual_point_false(self):
        mv.useDualPoint(False)
        self.assertEqual(mv.usingDualPoint(), False)

    def test_reference_low(self):
        mv.setDualPointCalibration(1.01, 0, 0, 0)
        self.assertEqual(mv.getCalibrateLowReference(), 1.01)

    def test_reference_high(self):
        mv.setDualPointCalibration(0, 1.01, 0, 0)
        self.assertEqual(mv.getCalibrateHighReference(), 1.01)

    def test_reading_low(self):
        mv.setDualPointCalibration(0, 0, 1.01, 0)
        self.assertEqual(mv.getCalibrateLowReading(), 1.01)

    def test_reading_high(self):
        mv.setDualPointCalibration(0, 0, 0, 1.01)
        self.assertEqual(mv.getCalibrateHighReading(), 1.01)

    def test_reset(self):
        n = float('nan')
        mv.reset()
        self.assertTrue(math.isnan(mv.getCalibrateLowReference()))
        self.assertTrue(math.isnan(mv.getCalibrateHighReference()))
        self.assertTrue(math.isnan(mv.getCalibrateLowReading()))
        self.assertTrue(math.isnan(mv.getCalibrateHighReading()))

    def test_temperature(self):
        self.assertNotEqual(mv.measureTemp(), -127)


if __name__ == '__main__':
    unittest.main()
