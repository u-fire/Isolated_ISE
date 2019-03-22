//! # uFire ISE Probe Interface
//!
//! Use any Ion Specific Electrode
//! * measure pH with automatic temperature compensation
//! * measure ORP and Eh
//! * raw mV
//! * temperature in Celsius
//! * library can be easily extended for any probe

use std::f32;
use std::thread;
use std::time::Duration;
use std::mem;

extern crate byteorder;
use byteorder::{ByteOrder, LittleEndian};

extern crate i2cdev;
use self::i2cdev::core::*;
use self::i2cdev::linux::{LinuxI2CDevice, LinuxI2CError};

const ISE_MEASURE_MV: u8 = 80;
const ISE_MEASURE_TEMP: u8 = 40;
const ISE_CALIBRATE_SINGLE: u8 = 20;
const ISE_CALIBRATE_LOW: u8 = 10;
const ISE_CALIBRATE_HIGH: u8 = 8;
const ISE_MEMORY_WRITE: u8 = 4;
const ISE_MEMORY_READ: u8 = 2;
const ISE_I2C: u8 = 1;

const ISE_VERSION_REGISTER: u8 = 0;
const ISE_MV_REGISTER: u8 = 1;
const ISE_TEMP_REGISTER: u8 = 5;
const ISE_CALIBRATE_SINGLE_REGISTER: u8 = 9;
const ISE_CALIBRATE_REFHIGH_REGISTER: u8 = 13;
const ISE_CALIBRATE_REFLOW_REGISTER: u8 = 17;
const ISE_CALIBRATE_READHIGH_REGISTER: u8 = 21;
const ISE_CALIBRATE_READLOW_REGISTER: u8 = 25;
const ISE_SOLUTION_REGISTER: u8 = 29;
const ISE_BUFFER_REGISTER: u8 = 33;
const ISE_CONFIG_REGISTER: u8 = 37;
const ISE_TASK_REGISTER: u8 = 38;

const ISE_DUALPOINT_CONFIG_BIT: u8 = 0;
const ISE_TEMP_COMPENSATION_CONFIG_BIT: u8 = 1;

const ISE_TEMP_MEASURE_TIME: u64 = 750;
const ISE_MV_MEASURE_TIME: u64 = 1750;

const PROBE_MV_TO_PH: f32 = 59.2;
const TEMP_CORRECTION_FACTOR: f32 = 0.03;
const POTENTIAL_REGISTER_ADDRESS: u8 = 100;

pub struct IseProbe {
    dev: Box<LinuxI2CDevice>,
}

impl IseProbe {
    /// Create a new IseProbe object
    ///
    /// Pass the i2c port to use, it must be a software overlay device, and I2C address.
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// ```
    pub fn new(filename: &'static str, address: u16) -> Result<Self, Box<LinuxI2CError>> {
        let dev = LinuxI2CDevice::new(filename, address)?;
        Ok(IseProbe { dev: Box::new(dev) })
    }

    /// Start a probe measurement
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.measure_mv();
    /// ```
    pub fn measure_mv(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        let mut mv: f32;

        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_MEASURE_MV)?;
        thread::sleep(Duration::from_millis(ISE_MV_MEASURE_TIME));
        mv = self._read_register(ISE_MV_REGISTER)?;

        if mv.is_nan() {
            mv = f32::NAN;
        }

        if mv.is_infinite() {
            mv = f32::NAN;
        }

        Ok(mv)
    }

    /// Start a pH measurement
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.measure_ph();
    /// ```
    pub fn measure_ph(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        let mut ph: f32;

        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_MEASURE_MV)?;
        thread::sleep(Duration::from_millis(ISE_MV_MEASURE_TIME));
        ph = self._read_register(ISE_MV_REGISTER)?;

        ph = f32::abs(7.0 - (ph / PROBE_MV_TO_PH));

        if self.using_temperature_compensation()? == 1 {
            let temp = self.measure_temp()?;
            let distance_from_7 = f32::abs(7.0 - f32::round(ph));
            let distance_from_25 = f32::floor(f32::abs(25.0 - f32::round(temp)) / 10.0);
            let mut temp_multiplier = (distance_from_25 * distance_from_7) * TEMP_CORRECTION_FACTOR;

            if ph >= 8.0 && temp >= 35.0 {
                // negative
                temp_multiplier *= -1.0;
            }

            if ph <= 6.0 && temp <= 15.0 {
                // negative
                temp_multiplier *= -1.0;
            }

            ph += temp_multiplier;
        }

        if ph <= 0.0 || ph > 14.0 {
            ph = -1.0;
        }

        if ph.is_nan() {
            ph = -1.0;
        }

        if ph.is_infinite() {
            ph = -1.0;
        }

        Ok(ph)
    }

    /// Converts a pH measurement into mV for use with calibration.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// assert_eq!(0.0, mv.ph_to_mv(7.0).unwrap());
    /// ```
    pub fn ph_to_mv(&mut self, ph: f32) -> Result<(f32), Box<LinuxI2CError>> {
        Ok((7.0 - ph) * PROBE_MV_TO_PH)
    }

    /// Start a temperature measurement
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.measure_temp();
    /// ```
    pub fn measure_temp(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_MEASURE_TEMP)?;
        thread::sleep(Duration::from_millis(ISE_TEMP_MEASURE_TIME));

        Ok(self._read_register(ISE_TEMP_REGISTER)?)
    }

    /// Sets the temperature used by the device.
    ///
    /// # Example
    /// ```
    /// let mut ec = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3c).unwrap();
    /// ec.set_temp(20.2);
    /// ```
    pub fn set_temp(&mut self, temp_c: f32) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_TEMP_REGISTER, temp_c)?;

        Ok(())
    }

    /// Calibrates the probe using a single point using a mV value.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.calibrate_single(500.0);
    /// ```
    pub fn calibrate_single(&mut self, solution_mv: f32) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_SOLUTION_REGISTER, solution_mv)?;
        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_CALIBRATE_SINGLE)?;
        thread::sleep(Duration::from_millis(ISE_MV_MEASURE_TIME));
        Ok(())
    }

    /// Calibrates the dual-point values for the low reading, in mV, and saves them in the
    /// devices's EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.calibrate_probe_low(50.0);
    /// ```
    pub fn calibrate_probe_low(&mut self, solution_mv: f32) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_SOLUTION_REGISTER, solution_mv)?;
        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_CALIBRATE_LOW)?;
        thread::sleep(Duration::from_millis(ISE_MV_MEASURE_TIME));
        Ok(())
    }

    /// Calibrates the dual-point values for the high reading, in mV, and saves them in the
    /// devices's EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.calibrate_probe_low(500.0);
    /// ```
    pub fn calibrate_probe_high(&mut self, solution_mv: f32) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_SOLUTION_REGISTER, solution_mv)?;
        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_CALIBRATE_HIGH)?;
        thread::sleep(Duration::from_millis(ISE_MV_MEASURE_TIME));
        Ok(())
    }

    /// Sets all the values, in mV, for dual point calibration and saves them in the devices's
    /// EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_dual_point_calibration(50.0, 500.0, 34.0, 553.0);
    /// ```
    pub fn set_dual_point_calibration(
        &mut self,
        ref_low: f32,
        ref_high: f32,
        read_low: f32,
        read_high: f32,
    ) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_CALIBRATE_REFLOW_REGISTER, ref_low)?;
        self._write_register(ISE_CALIBRATE_REFHIGH_REGISTER, ref_high)?;
        self._write_register(ISE_CALIBRATE_READLOW_REGISTER, read_low)?;
        self._write_register(ISE_CALIBRATE_READHIGH_REGISTER, read_high)?;

        Ok(())
    }

    /// Returns the single-point offset from the device.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.get_calibrate_offset();
    /// ```
    pub fn get_calibrate_offset(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        Ok(self._read_register(ISE_CALIBRATE_SINGLE_REGISTER)?)
    }

    /// Returns the dual-point calibration high reference value.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_dual_point_calibration(0.0, 500.0, 0.0, 0.0);
    /// ```
    pub fn get_calibrate_high_reference(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        Ok(self._read_register(ISE_CALIBRATE_REFHIGH_REGISTER)?)
    }

    /// Returns the dual-point calibration high reading value.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_dual_point_calibration(0.0, 0.0, 0.0, 553.0);
    /// assert_eq!(553.0, mv.get_calibrate_high_reading().unwrap());
    /// ```
    pub fn get_calibrate_high_reading(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        Ok(self._read_register(ISE_CALIBRATE_READHIGH_REGISTER)?)
    }

    /// Returns the dual-point calibration low reference value.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_dual_point_calibration(50.0, 0.0, 0.0, 0.0);
    /// assert_eq!(50.0, mv.get_calibrate_low_reference().unwrap());
    /// ```
    pub fn get_calibrate_low_reference(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        Ok(self._read_register(ISE_CALIBRATE_REFLOW_REGISTER)?)
    }

    /// Returns the dual-point calibration low reading value.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_dual_point_calibration(0.0, 0.0, 34.0, 0.0);
    /// assert_eq!(34.0, mv.get_calibrate_low_reading().unwrap());
    /// ```
    pub fn get_calibrate_low_reading(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        Ok(self._read_register(ISE_CALIBRATE_READLOW_REGISTER)?)
    }

    /// Configures the device to use temperature compensation or not.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.use_temperature_compensation(true);
    /// assert_eq!(1, mv.using_temperature_compensation().unwrap());
    /// ```
    pub fn use_temperature_compensation(&mut self, b: bool) -> Result<(), Box<LinuxI2CError>> {
        self._change_register(ISE_CONFIG_REGISTER)?;
        let mut config: u8 = self.dev.smbus_read_byte()?;
        thread::sleep(Duration::from_millis(10));
        if b {
            config |= 1 << ISE_TEMP_COMPENSATION_CONFIG_BIT;
        } else {
            config &= !(1 << ISE_TEMP_COMPENSATION_CONFIG_BIT);
        }
        self.dev.smbus_write_byte_data(ISE_CONFIG_REGISTER, config)?;
        thread::sleep(Duration::from_millis(10));
        Ok(())
    }

    /// Configures device to use dual-point calibration.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.use_dual_point(true);
    /// assert_eq!(1, mv.using_dual_point().unwrap());
    /// ```
    pub fn use_dual_point(&mut self, b: bool) -> Result<(), Box<LinuxI2CError>> {
        self._change_register(ISE_CONFIG_REGISTER)?;
        let mut config: u8 = self.dev.smbus_read_byte()?;
        thread::sleep(Duration::from_millis(10));
        if b {
            config |= 1 << ISE_DUALPOINT_CONFIG_BIT;
        } else {
            config &= !(1 << ISE_DUALPOINT_CONFIG_BIT);
        }
        self.dev.smbus_write_byte_data(ISE_CONFIG_REGISTER, config)?;
        thread::sleep(Duration::from_millis(10));
        Ok(())
    }

    /// Returns the firmware version of the device.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// assert_eq!(0x1a, mv.get_version().unwrap());
    /// ```
    pub fn get_version(&mut self) -> Result<(u8), Box<LinuxI2CError>> {
        self._change_register(ISE_VERSION_REGISTER)?;
        //thread::sleep(Duration::from_millis(25));
        Ok(self.dev.smbus_read_byte()?)
    }

    /// Resets all the stored calibration information.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.reset();
    /// assert_eq!(true, mv.get_calibrate_offset().unwrap().is_nan());
    /// assert_eq!(true, mv.get_calibrate_low_reading().unwrap().is_nan());
    /// assert_eq!(true, mv.get_calibrate_high_reading().unwrap().is_nan());
    /// assert_eq!(true, mv.get_calibrate_low_reference().unwrap().is_nan());
    /// assert_eq!(true, mv.get_calibrate_high_reference().unwrap().is_nan());
    /// ```
    pub fn reset(&mut self) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_CALIBRATE_REFLOW_REGISTER, f32::NAN)?;
        self._write_register(ISE_CALIBRATE_REFHIGH_REGISTER, f32::NAN)?;
        self._write_register(ISE_CALIBRATE_READLOW_REGISTER, f32::NAN)?;
        self._write_register(ISE_CALIBRATE_READHIGH_REGISTER, f32::NAN)?;
        self._write_register(ISE_CALIBRATE_SINGLE_REGISTER, f32::NAN)?;
        Ok(())
    }

    /// Configures the device to use temperature compensation.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.use_temperature_compensation(true);
    /// assert_eq!(1, mv.using_temperature_compensation().unwrap());
    /// ```
    pub fn using_temperature_compensation(&mut self) -> Result<(u8), Box<LinuxI2CError>> {
        self._change_register(ISE_CONFIG_REGISTER)?;
        let config: u8 = self.dev.smbus_read_byte()?;
        Ok((config >> ISE_TEMP_COMPENSATION_CONFIG_BIT) & 1)
    }

    /// Configures device to use dual-point calibration.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.use_dual_point(true);
    /// assert_eq!(1, mv.using_dual_point().unwrap());
    /// ```
    pub fn using_dual_point(&mut self) -> Result<(u8), Box<LinuxI2CError>> {
        self._change_register(ISE_CONFIG_REGISTER)?;
        let config: u8 = self.dev.smbus_read_byte()?;
        Ok((config >> ISE_DUALPOINT_CONFIG_BIT) & 1)
    }

    /// Sets the I2C address of the device.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// // mv.set_i2c_address(0x4f);
    /// ```
    pub fn set_i2c_address(&mut self, i2c_address: u8) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_SOLUTION_REGISTER, i2c_address as f32)?;
        self.dev.smbus_write_byte_data(ISE_TASK_REGISTER, ISE_I2C)?;

        Ok(())
    }

    /// Sets all the values, in mV, for dual point calibration and saves them in the devices's
    /// EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.write_eeprom(100, 295.0);
    /// assert_eq!(295.0, mv.read_eeprom(100).unwrap());
    /// ```
    pub fn read_eeprom(&mut self, address: u8) -> Result<(f32), Box<LinuxI2CError>> {
        self._write_register(ISE_SOLUTION_REGISTER, address as f32)?;
        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_MEMORY_READ)?;
        thread::sleep(Duration::from_millis(25));
        Ok(self._read_register(ISE_BUFFER_REGISTER)?)
    }

    /// Sets all the values, in mV, for dual point calibration and saves them in the devices's
    /// EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.write_eeprom(100, 295.0);
    /// assert_eq!(295.0, mv.read_eeprom(100).unwrap());
    /// ```
    pub fn write_eeprom(&mut self, address: u8, value: f32) -> Result<(), Box<LinuxI2CError>> {
        self._write_register(ISE_SOLUTION_REGISTER, address as f32)?;
        self._write_register(ISE_BUFFER_REGISTER, value)?;
        self.dev
            .smbus_write_byte_data(ISE_TASK_REGISTER, ISE_MEMORY_WRITE)?;
        thread::sleep(Duration::from_millis(25));
        Ok(())
    }

    /// Returns the saved probe potential from EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_probe_potential(245.0).unwrap();
    /// assert_eq!(245.0, mv.get_probe_potential().unwrap());
    /// ```
    pub fn get_probe_potential(&mut self) -> Result<(f32), Box<LinuxI2CError>> {
        Ok(self.read_eeprom(POTENTIAL_REGISTER_ADDRESS)?)
    }

    /// Saves probe potential in EEPROM.
    ///
    /// # Example
    /// ```
    /// let mut mv = ufire_ise::IseProbe::new("/dev/i2c-3", 0x3f).unwrap();
    /// mv.set_probe_potential(245.0).unwrap();
    /// assert_eq!(245.0, mv.get_probe_potential().unwrap());
    /// ```
    pub fn set_probe_potential(&mut self, potential: f32) -> Result<(), Box<LinuxI2CError>> {
        self.write_eeprom(POTENTIAL_REGISTER_ADDRESS, potential)?;
        Ok(())
    }

    pub fn _write_register(&mut self, register: u8, f_val: f32) -> Result<(), Box<LinuxI2CError>> {
        unsafe {
            let buf: [u8; 4] = mem::transmute(f_val);
            self._change_register(register)?;
            self.dev.smbus_write_byte_data(register + 0, buf[0])?;
            thread::sleep(Duration::from_millis(10));
            self.dev.smbus_write_byte_data(register + 1, buf[1])?;
            thread::sleep(Duration::from_millis(10));
            self.dev.smbus_write_byte_data(register + 2, buf[2])?;
            thread::sleep(Duration::from_millis(10));
            self.dev.smbus_write_byte_data(register + 3, buf[3])?;
            thread::sleep(Duration::from_millis(10));
            Ok(())
        }
    }

    pub fn _read_register(&mut self, register: u8) -> Result<(f32), Box<LinuxI2CError>> {
        let mut buf: [u8; 4] = [0; 4];
        self._change_register(register)?;
        buf[0] = self.dev.smbus_read_byte()?;
        thread::sleep(Duration::from_millis(10));
        buf[1] = self.dev.smbus_read_byte()?;
        thread::sleep(Duration::from_millis(10));
        buf[2] = self.dev.smbus_read_byte()?;
        thread::sleep(Duration::from_millis(10));
        buf[3] = self.dev.smbus_read_byte()?;
        thread::sleep(Duration::from_millis(10));
        Ok(LittleEndian::read_f32(&buf))
    }

    pub fn _change_register(&mut self, register: u8) -> Result<(), Box<LinuxI2CError>> {
        self.dev.smbus_write_byte(register)?;
        thread::sleep(Duration::from_millis(10));
        Ok(())
    }
}
