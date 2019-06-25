/**
 * Author Teemu Mäntykallio
 * Initializes the library and runs the stepper motor.
 * Modified by: Connor W. Colombo, Ottia
 */

#include <HAL.h>

float Rsense = 0.11;
float hold_x = 0.5;

void serialTuple(String cmd, int arg) {
	Serial.print("Received command: ");
	Serial.print(cmd);
	Serial.print("(");
	Serial.print(arg);
	Serial.println(")");
}

// Microstepping:
uint16_t ms_size = 0;
void setMS(uint16_t mss){
	driver.microsteps(mss);
	ms_size = mss;
}

void init_LiveTuner() {
	while(Serial.readStringUntil('\n') != "INIT"){ ; } // Wait for init.

	setMS(MICROSTEPS);

	while (driver.cur_a() < 240) { // Use cur_b if measuring from B coil
		digitalWrite(STEP_PIN, HIGH);
		digitalWrite(STEP_PIN, LOW);
		delay(3);
	}
	Serial.print("cur_a = ");
	Serial.println(driver.cur_a(), DEC);
	Serial.print("cur_b = ");
	Serial.println(driver.cur_b(), DEC);
}

void run_LiveTuner() {
	// Update Stall Guard Plot:
	static uint32_t last_time = 0;
	static bool plotStallGuard = false;
	uint32_t now = millis();
	if(plotStallGuard && (now-last_time) > 100) { //run every 0.1s
	  last_time = now;
	  uint32_t drv_status = driver.DRV_STATUS();
	  Serial.print("0 ");
	  Serial.print((drv_status & SG_RESULT_bm)>>SG_RESULT_bp , DEC);
	  Serial.print(" ");
	  Serial.println(driver.rms_current(), DEC);
  }
	if(stepper.distanceToGo() == 0){
		plotStallGuard = false;
	}

	// Process Commands:
	if (Serial.available() > 0) {
		String cmd = Serial.readStringUntil(' ');
		String strArg = Serial.readStringUntil('\n');

		int arg = strArg.toInt();

		if (cmd == "setAccel") {
			serialTuple("setAccel", arg);
			stepper.setAcceleration(arg * ms_size*STEPS_PER_DEG/MICROSTEPS);
		}
		else if (cmd == "setSpeed") {
			serialTuple("setSpeed", arg);
	    stepper.setMaxSpeed(arg * ms_size*STEPS_PER_DEG/MICROSTEPS);
			delay(100);
		}
		else if (cmd == "setMicros") { // Set Microstepping
			serialTuple("setMicros", arg);
			setMS(arg);
		}
		else if (cmd == "rampTo") {
			serialTuple("rampTo", arg);
			stepper.moveTo(arg * ms_size*STEPS_PER_DEG/MICROSTEPS);
			delay(100);
		}
		else if (cmd == "stallGuardTo") { // TODO: FIXME
			serialTuple("stallGuardTo", arg);
			stepper.moveTo(arg * ms_size*STEPS_PER_DEG/MICROSTEPS);
			delay(100);
			plotStallGuard = true;
		}
		else if (cmd == "rms_current") {
			serialTuple("rms_current", arg);
			driver.rms_current(arg, hold_x);
		}
		else if (cmd == "Rsense") {
			Serial.print("Setting R sense value to: ");
			Serial.println(arg);
			Rsense = arg;
		}
		else if (cmd == "hold_multiplier") {
			Serial.print("Setting hold multiplier to: ");
			Serial.println(arg);
			hold_x = arg;
		}
		else if (cmd == "GCONF") {
			Serial.print("GCONF: 0b");
			Serial.println(driver.GCONF(), BIN);
		}
		else if (cmd == "I_scale_analog") {
			serialTuple("I_scale_analog", arg);
			driver.I_scale_analog(arg);
		}
		else if (cmd == "internal_Rsense") {
			serialTuple("internal_Rsense", arg);
			driver.internal_Rsense(arg);
		}
		else if (cmd == "en_pwm_mode") {
			serialTuple("en_pwm_mode", arg);
			driver.en_pwm_mode(arg);
		}
		else if (cmd == "enc_commutation") {
			serialTuple("enc_commutation", arg);
			driver.enc_commutation(arg);
		}
		else if (cmd == "shaft") {
			serialTuple("shaft", arg);
			driver.shaft(arg);
		}
		else if (cmd == "diag0_error") {
			serialTuple("diag0_error", arg);
			driver.diag0_error(arg);
		}
		else if (cmd == "diag0_otpw") {
			serialTuple("diag0_otpw", arg);
			driver.diag0_otpw(arg);
		}
		else if (cmd == "diag0_stall") {
			serialTuple("diag0_stall", arg);
			driver.diag0_stall(arg);
		}
		else if (cmd == "diag1_stall") {
			serialTuple("diag1_stall", arg);
			driver.diag1_stall(arg);
		}
		else if (cmd == "diag1_index") {
			serialTuple("diag1_index", arg);
			driver.diag1_index(arg);
		}
		else if (cmd == "diag1_onstate") {
			serialTuple("diag1_onstate", arg);
			driver.diag1_onstate(arg);
		}
		else if (cmd == "diag0_int_pushpull") {
			serialTuple("diag0_int_pushpull", arg);
			driver.diag0_int_pushpull(arg);
		}
		else if (cmd == "diag1_pushpull") {
			serialTuple("diag1_pushpull", arg);
			driver.diag1_pushpull(arg);
		}
		else if (cmd == "small_hysteresis") {
			serialTuple("small_hysteresis", arg);
			driver.small_hysteresis(arg);
		}
		else if (cmd == "stop_enable") {
			serialTuple("stop_enable", arg);
			driver.stop_enable(arg);
		}
		else if (cmd == "direct_mode") {
			serialTuple("direct_mode", arg);
			driver.direct_mode(arg);
		}
		// IHOLD_IRUN
		else if (cmd == "ihold") {
			serialTuple("ihold", arg);
			driver.ihold(arg);
		}
		else if (cmd == "irun") {
			serialTuple("irun", arg);
			driver.irun(arg);
		}
		else if (cmd == "iholddelay") {
			serialTuple("iholddelay", arg);
			driver.iholddelay(arg);
		}

		else if (cmd == "TSTEP") {
			Serial.print("TSTEP: ");
			Serial.println(driver.TSTEP());
		}

		else if (cmd == "TPWMTHRS") {
			serialTuple("TPWMTHRS", arg);
			driver.TPWMTHRS(arg);
		}
		else if (cmd == "TCOOLTHRS") {
			serialTuple("TCOOLTHRS", arg);
			driver.TCOOLTHRS(arg);
		}
		else if (cmd == "THIGH") {
			serialTuple("THIGH", arg);
			driver.THIGH(arg);
		}
		// XDIRECT
		else if (cmd == "coil_A") {
			serialTuple("coil_A", arg);
			driver.coil_A(arg);
		}
		else if (cmd == "coil_B") {
			serialTuple("coil_B", arg);
			driver.coil_B(arg);
		}

		else if (cmd == "VDCMIN") {
			serialTuple("VDCMIN", arg);
			driver.VDCMIN(arg);
		}

		else if (cmd == "CHOPCONF") {
			Serial.print("CHOPCONF: 0b");
			Serial.println(driver.CHOPCONF(), BIN);
		}
		else if (cmd == "toff") {
			serialTuple("toff", arg);
			driver.toff(arg);
		}
		else if (cmd == "hstrt") {
			serialTuple("hstrt", arg);
			driver.hstrt(arg);
		}
		else if (cmd == "hend") {
			serialTuple("hend", arg);
			driver.hend(arg);
		}
		/*
		else if (cmd == "fd") {
			serialTuple("fd", arg);
			driver.fd(arg);
		}
		*/
		else if (cmd == "disfdcc") {
			serialTuple("disfdcc", arg);
			driver.disfdcc(arg);
		}
		else if (cmd == "rndtf") {
			serialTuple("rndtf", arg);
			driver.rndtf(arg);
		}
		else if (cmd == "chm") {
			serialTuple("chm", arg);
			driver.chm(arg);
		}
		else if (cmd == "tbl") {
			serialTuple("tbl", arg);
			driver.tbl(arg);
		}
		else if (cmd == "vsense") {
			serialTuple("vsense", arg);
			driver.vsense(arg);
		}
		else if (cmd == "vhighfs") {
			serialTuple("vhighfs", arg);
			driver.vhighfs(arg);
		}
		else if (cmd == "vhighchm") {
			serialTuple("vhighchm", arg);
			driver.vhighchm(arg);
		}
		else if (cmd == "sync") {
			serialTuple("sync", arg);
			driver.sync(arg);
		}
		else if (cmd == "mres") {
			serialTuple("mres", arg);
			driver.mres(arg);
		}
		else if (cmd == "intpol") {
			serialTuple("intpol", arg);
			driver.intpol(arg);
		}
		else if (cmd == "dedge") {
			serialTuple("dedge", arg);
			driver.dedge(arg);
		}
		else if (cmd == "diss2g") {
			serialTuple("diss2g", arg);
			driver.diss2g(arg);
		}
		// COOLCONF
		else if (cmd == "semin") {
			serialTuple("semin", arg);
			driver.semin(arg);
		}
		else if (cmd == "seup") {
			serialTuple("seup", arg);
			driver.seup(arg);
		}
		else if (cmd == "semax") {
			serialTuple("semax", arg);
			driver.semax(arg);
		}
		else if (cmd == "sedn") {
			serialTuple("sedn", arg);
			driver.sedn(arg);
		}
		else if (cmd == "seimin") {
			serialTuple("seimin", arg);
			driver.seimin(arg);
		}
		else if (cmd == "sgt") {
			serialTuple("sgt", arg);
			driver.sgt(arg);
		}
		else if (cmd == "sfilt") {
			serialTuple("sfilt", arg);
			driver.sfilt(arg);
		}
		// PWMCONF
		else if (cmd == "pwm_ampl") {
			serialTuple("pwm_ampl", arg);
			driver.pwm_ampl(arg);
		}
		else if (cmd == "pwm_grad") {
			serialTuple("pwm_grad", arg);
			driver.pwm_grad(arg);
		}
		else if (cmd == "pwm_freq") {
			serialTuple("pwm_freq", arg);
			driver.pwm_freq(arg);
		}
		else if (cmd == "pwm_autoscale") {
			serialTuple("pwm_autoscale", arg);
			driver.pwm_autoscale(arg);
		}
		else if (cmd == "pwm_symmetric") {
			serialTuple("pwm_symmetric", arg);
			driver.pwm_symmetric(arg);
		}
		else if (cmd == "freewheel") {
			serialTuple("freewheel", arg);
			driver.freewheel(arg);
		}
		// ENCM_CTRL
		else if (cmd == "inv") {
			serialTuple("inv", arg);
			driver.inv(arg);
		}

		else if (cmd == "DRVSTATUS") {
			Serial.print("DRVSTATUS: 0b");
			Serial.println(driver.DRV_STATUS(), BIN);
		}
		else if (cmd == "PWM_SCALE") {
			Serial.print("PWM_SCALE: 0b");
			Serial.println(driver.PWM_SCALE(), DEC);
		}
		else if (cmd == "LOST_STEPS") {
			Serial.print("LOST_STEPS: 0b");
			Serial.println(driver.LOST_STEPS(), DEC);
		}
		else {
			Serial.println("Invalid command!");
		}
	}
}
