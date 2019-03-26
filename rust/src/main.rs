extern crate ufire_iso_ise;
extern crate shrust;
use shrust::{Shell, ShellIO};
use std::io::prelude::*;
use ufire_iso_ise::*;

fn main() {
    println!("Isolated ISE Probe Interface shell");
	println!("type `help` for a list of commands");
	let v = vec![0x3f];
	let mut shell = Shell::new(v);

	shell.new_command_noargs("config", "Prints configuration data", move |io, v| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		try!(writeln!(io, "Config:"));
		try!(writeln!(io, "	offset: {}", mv.get_calibrate_offset().unwrap()));
		try!(writeln!(io, "	dual point:"));
		try!(writeln!(io, "		low reference / read: {} / {}",
		mv.get_calibrate_low_reference().unwrap(),
		mv.get_calibrate_low_reading().unwrap()));
		try!(writeln!(io, "		high reference / read: {} / {}",
		mv.get_calibrate_high_reference().unwrap(),
		mv.get_calibrate_high_reading().unwrap()));
		try!(writeln!(io, "	version hw.fw: {}.{}", mv.get_version().unwrap(), mv.get_firmware().unwrap()));
		Ok(())
	});

	shell.new_command_noargs("reset", "Resets all configuration data", move |_, v| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		mv.reset().unwrap();
		Ok(())
	});

	shell.new_command_noargs("temp", "Measures temperature", move |io, v| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		let temp_c = mv.measure_temp().unwrap();
		try!(writeln!(io, "C: {}", temp_c));
		Ok(())
	});

	shell.new_command_noargs("mv", "`mv <temp C>` : Measures EC", move |io, v| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		let result = mv.measure_mv().unwrap();
		try!(writeln!(io, "mV: {}", result));
		Ok(())
	});

	shell.new_command("ph", "`ph <temp C>` : Measures pH", 1, move |io, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		let ph = mv.measure_ph(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "pH: {}", ph));
		Ok(())
	});

	shell.new_command("cal", "`cal <pH>` : Calibrate the probe using a single point", 1, move |io, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		mv.calibrate_single(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	offset: {}", mv.get_calibrate_offset().unwrap()));
		Ok(())
	});

	shell.new_command("low", "`low <pH>` : Dual point low calibration", 1, move |io, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		mv.calibrate_probe_low(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	low reference / read: {} / {}",
		mv.get_calibrate_low_reference().unwrap(),
		mv.get_calibrate_low_reading().unwrap()));
		Ok(())
	});

	shell.new_command("high", "`high <pH>` : Dual point high calibration", 1, move |io, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		mv.calibrate_probe_high(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	high reference / read: {} / {}",
		mv.get_calibrate_high_reference().unwrap(),
		mv.get_calibrate_high_reading().unwrap()));
		Ok(())
	});

	shell.new_command_noargs("ver", "Hardware/Firmware of device", move |io, v| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		try!(writeln!(io, "	version: {}.{}", mv.get_version().unwrap(),mv.get_firmware().unwrap()));
		Ok(())
	});

	shell.new_command("i2c", "`i2c <decimal I2C address>` : Changes the I2C address", 1, move |io, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		v[0] = s[0].parse().unwrap();
		mv.set_i2c_address(v[0]).unwrap();
		try!(writeln!(io, "	I2C Address: {}", v[0]));
		Ok(())
	});

	shell.new_command("read", "`read <address>` : Reads an address in EEPROM", 1, move |io, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		let eeprom = mv.read_eeprom(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "{}", eeprom));
		Ok(())
	});

	shell.new_command("write", "`read ,address> <value>` : Changes the I2C address", 1, move |_, v, s| {
		let mut mv = IseProbe::new("/dev/i2c-3", v[0]).unwrap();
		mv.write_eeprom(s[0].parse().unwrap(), s[1].parse().unwrap()).unwrap();
		Ok(())
	});

    shell.run_loop(&mut ShellIO::default());
}
