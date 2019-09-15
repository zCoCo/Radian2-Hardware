/*
 * Unit tests to verify most basic functionality of motor driver hardware. That
 * is: if any of these tests fail, there is almost certainly a hardware failure.
 *
 * Author: Connor W. Colombo, Ottia
 * Last Update: 8/28/2019, Colombo
 */

#ifdef UNIT_TEST

#include <Arduino.h>
#include <unity.h>
#include <Streaming.h>

#include <HAL.h>

#if defined(R2E7v1) || defined(M1C1v2) // Tests listed in order of increasing complexity:
//Perform any setup neccessary for testing that can't be guaranteed outside it.
void init_testing_env(void){
  // Ensure DIAG1 only reports on motor stall:
  pinMode(DIAG1, INPUT_PULLUP); // rated as weak pull-up at 30-50kOhm, DIAG1 is spec'd for 47kOhm max.
  driver.diag1_pushpull(0); // Driver will pull up to 5V if enabled and DIAG1 is not on 5V tolerant pin
  driver.diag1_stall(1);
  driver.diag1_index(0);
  driver.diag1_onstate(0);
}

/*
 * Passes if the motor driver is connected, can be accessed over SPI, and its
 * registers can be configured and read over SPI.
 */
void test_driver_connection(void){
  Serial.println("Testing motor-driver connection . . .");
  // Check to see if GCONF contains data:
  uint32_t gconf = driver.GCONF();
  uint32_t chop = driver.CHOPCONF();
  uint32_t stat = driver.DRV_STATUS();
  TEST_ASSERT_NOT_EQUAL_MESSAGE(gconf | chop | stat, 0b0,
    "All major config. and status registers shouldn't be 0. Check motor driver "
    "SPI connection."
  );
  // Check to see if a register bits can be set an recalled:
  int resetTarget = MICROSTEPS >= 128 ? MICROSTEPS/2 : MICROSTEPS*2;
  driver.microsteps(resetTarget);
  TEST_ASSERT_EQUAL_INT_MESSAGE(resetTarget, driver.microsteps(),
    "Microsteps sector of CHOPCONF register can't be changed and read. Check "
    "motor driver SPI connection."
  );
  driver.microsteps(MICROSTEPS);
  TEST_ASSERT_EQUAL_INT_MESSAGE(MICROSTEPS, driver.microsteps(),
    "Microsteps sector of CHOPCONF register can't be changed and read. Check "
    "motor driver SPI connection. NOTE: Failure occured on second attempt; "
    "first attempt passed."
  );
}

/*
 * Passes if no flags have been raised for abnormal motor driver conditions.
 * This test should be run after / as a part of all tests in which the motor is
 * engaged.
 * NOTE: This only performs checks which require no drive conditions (eg.
 * motor_open requires motor to be being driven in low-speed).
 */
void test_stable_driver_conditions(void){
  TEST_ASSERT_FALSE_MESSAGE(motor_stalled(),
    "Motor driver is reporting a motor stall. Check that there are no excessive "
    "loads on the motor, that it is free to spin, and that the max. RMS motor "
    "coil current isn't set too low. "
  );
  if(motor_shorted()){
    Serial << "Current Shorting State: Coil A - " << (driver.s2ga() ? "shorted":"no") << ", Coil B - " << (driver.s2gb() ? "shorted":"no") << endl;
  }
  TEST_ASSERT_FALSE_MESSAGE(motor_shorted(),
    "Motor driver is reporting a short to ground between one or both of the "
    "coil circuits. Power off and check motor wiring and motor driver integrity."
  );
  TEST_ASSERT_FALSE_MESSAGE(driver_warm(),
    "Motor driver is at a high temperature and near a thermal shutdown. Adjust "
    "motor max-speed, current, and acceleration settings accordingly. Consider "
    "powering off and making sure driver has solid thermal connection to cooling "
    "features and that there are no short circuits in motor wiring or motor "
    "driver."
  );
  TEST_ASSERT_FALSE_MESSAGE(driver_overloaded(),
    "Motor driver is at a very high temperature and has shutdown to protect "
    "itself. Adjust motor max-speed, current, and acceleration settings "
    "accordingly. Power off and make sure driver has solid thermal connection to "
    "cooling features and that there are no short circuits in motor wiring or "
    "motor driver."
  );
}

/*
 * Passes if there is a motor connected; it can be driven (can sink current);
 * and the current draw can be read from driver registers over SPI, which should
 * be true if #test_driver_connection passes.
 */
void test_motor_connection(void){
  Serial.println("Testing motor connection . . .");
  static constexpr int current_threshold = 240; // arbitrary
  static constexpr int step_threshold = 360 * STEPS_PER_DEG; // num. of attempted steps before test considered a failure

  digitalWrite(ENA, LOW); // Enable stepper

  bool stall_init = motor_stalled(); // Motor stall state before test

  // Whether both motor coils have simultaneously reported being not open for
  // even a single cycle after current thresh. was reached (this should always
  // true normal operating conditions):
  bool not_open = false;
  bool curr_passed = false; // Whether the current threshold has been passed
  int count = 0;
  while((count<step_threshold && !curr_passed) || (curr_passed && !not_open)){
    // Request a step:
    digitalWrite(STEP_PIN, HIGH);
    digitalWrite(STEP_PIN, LOW);

    // Update motor and driver state:
    curr_passed = driver.cur_a() > current_threshold;
    if(curr_passed && !motor_still()){
      not_open |= !motor_open();
    }

    delay(3); // Open-loop testing must be done at low speeds
    count++;
  }

  TEST_ASSERT_TRUE_MESSAGE(curr_passed,
    "Driver unable to source sufficient current. Power off and check motor "
    "connection to motor driver, ensure proper coil wiring, and motor driver "
    "circuit integrity."
  );

  TEST_ASSERT_TRUE_MESSAGE(not_open,
    "Motor driver is reporting that one or both motor coils is open "
    "(broken-circuit) during a low speed test. Power off and check motor wiring. "
    "Note: False positives also be caused by low motor voltage, motor short, "
    "excessive driver temperature, and high motor velocity (not in this test)."
  );

  // Ensure no DIAG1 flags were raised during test (it's fine it was already raised):
  TEST_ASSERT_TRUE_MESSAGE(stall_init | !motor_stalled(),
    "DIAG1 or stallGuard flag was raised during motor connection test. Motor "
    "likely stalled during test. Ensure that there are no excessive loads on the "
    "motor and that the max. RMS motor coil current isn't set too low. "
    "Alternatively, consider adjusting the driver stall detection threshold, "
    "SGT, in the COOLCONF register."
  );

  RUN_TEST(test_stable_driver_conditions);
}

/*
 * Passes if:
 *  - The motor can be run continuously at given current (mA) and target speed
      (stp/sec) for given number of steps, run_dist.
 *  - This run takes roughly (w/in 5%) the expected amount of time.
 *  - The motor driver can still be communicated with after the run.
 *  - The motor driver reports no abnormal conditions after the run.
 */
void motor_speed_test(const float targ_speed, const uint32_t run_dist, const uint32_t current){
  // Set max. params for test:
  stepper.setMaxSpeed(targ_speed);
  stepper.setSpeed(targ_speed);
  driver.rms_current(current); // mA
  // Get up to speed in 1 rev. (should be easily attainable acceleration for
  // reasonable target speeds):
  const float accel = targ_speed*targ_speed / 2 / STEPS_PER_REV;
  stepper.setAcceleration(accel);

  // Set expectations for test:
  float expected_time = 1000 * (run_dist / targ_speed + targ_speed / accel);
  if((targ_speed*targ_speed / accel)/1000.0 > run_dist){ // not enough time to get up to speed and back down before reaching run_dist
    expected_time = 2000.0 * sqrtf(run_dist / accel);
  }

  bool stall_init = motor_stalled(); // motor stall state before test

  // Setup Move:
  stepper.move(run_dist);
  delay(100);

  // Run Move:
  uint32_t start_time = millis();
  while(stepper.distanceToGo() > 0){
    stepper.run();
  }
  uint32_t run_time = millis() - start_time;

  // Perform quick register fetch, change, and fetch to verify that motor driver
  // is alive and communicating:
  bool shaftSetting = driver.shaft();
  driver.shaft(!shaftSetting);
  TEST_ASSERT_EQUAL_MESSAGE(driver.shaft(), !shaftSetting,
    "Unable to change shaft setting in GCONF register and then read that change "
    "back successfully after motor driving test. Check motor driver SPI "
    "connection. Motor driver could have browned out or entered a fault state "
    "during test. Verify stability of power supply (VM and VIO) to the motor "
    "driver."
  );
  driver.shaft(shaftSetting); // Return setting to prev. value

  // Ensure no DIAG1 flags were raised during test (it's fine it was already raised):
  TEST_ASSERT_TRUE_MESSAGE(stall_init | !motor_stalled(),
    "DIAG1 or stallGuard flag was raised during motor driving test. Motor likely "
    "stalled during test. Ensure that there are no excessive loads on the motor "
    "and that the max. RMS motor coil current isn't set too low and acceration "
    "target isn't set too high. Alternatively, consider adjusting the driver "
    "stall detection threshold, SGT, in the COOLCONF register."
  );

  // Verify test performance:
  TEST_ASSERT_TRUE_MESSAGE(1.05f*expected_time > run_time,
    "Motor driving test run took longer than expected. Target speed was not attained. "
    "Verify motor wiring and then adjust max speed or microstepping accordingly."
  );

  TEST_ASSERT_TRUE_MESSAGE(0.95f*expected_time < run_time,
    "Motor driving test run completed too quickly. Target speed was exceeded. "
    "Consider adjusting max. RMS motor coil current and verify connection of "
    "indexing signal (STP, DIR) to the motor driver."
  );
}

/*
 * Passes if:
 *  - The motor can be run continuously at a quarter of max speed and
 *    max current draw for 2 revolutions.
 *  - This run takes roughly (w/in 5%) the expected amount of time.
 *  - The motor driver can still be communicated with after the run.
 *  - The motor driver reports no abnormal conditions after the run.
 */
 void test_motor_run(){
   Serial.println("Testing motor run . . .");
   motor_speed_test(MAX_STEP_SPEED/4, 2*STEPS_PER_REV, MAX_RMS_CURRENT);
 }

 /*
  * Passes if:
  *  - The motor can be run continuously at a max speed and high current draw for
       10 revolutions.
  *  - This run takes roughly (w/in 5%) the expected amount of time.
  *  - The motor driver can still be communicated with after the run.
  *  - The motor driver reports no abnormal conditions after the run.
  */
  void test_motor_fast_run(){
    Serial.println("Testing fast motor run . . .");
    motor_speed_test(MAX_STEP_SPEED, 10*STEPS_PER_REV, MAX_RMS_CURRENT);
  }

  /*
   * Passes if:
   *  - The motor can be run continuously at half of max speed and high current
        draw for 3 minutes.
   *  - This run takes roughly (w/in 5%) the expected amount of time.
   *  - The motor driver can still be communicated with after the run.
   *  - The motor driver reports no abnormal conditions after the run.
   */
   void test_motor_endurance(){
     Serial.println("Testing motor endurance . . .");
     motor_speed_test(MAX_STEP_SPEED/2, 3*60*MAX_STEP_SPEED/2, MAX_RMS_CURRENT);
   }

   /*
    * Runs the given motor test followed by an abnormalities check. Can't embed
    * the second RUN_TEST in the first because platformio only reports the name
    * on the inner most test on success.
    * ALSO, this has to be a macro because if it was a function with a function
    * pointer as an argument, platformio just reports the name of that argument
    * and not the function it points to.
    */
    #define exec_motor_test(test) do{\
        RUN_TEST(test);\
        RUN_TEST(test_stable_driver_conditions);\
    } while(0)

#endif

void setup() {
  delay(2100); // wait for Serial connection to be established
  UNITY_BEGIN();

  // Setup hardware:
  init_HAL();
  init_stepper();

  init_testing_env();

  Serial.println("Starting Tests . . .");

  RUN_TEST(test_driver_connection);
  TEST_ASSERT_TRUE_MESSAGE(!motor_stalled(),
    "Motor should not be reporting a stall before running. Check driver "
    "connection and diagnostic pull-up/down resistors."
  );
  RUN_TEST(test_motor_connection);

  // Ensure motor and driver are not reporting abnormal conditions before
  // attempting to run motor.
  RUN_TEST(test_stable_driver_conditions);

  exec_motor_test(test_motor_run);
  exec_motor_test(test_motor_fast_run);
  exec_motor_test(test_motor_endurance);

  UNITY_END();
}

void loop(){
  // Do nothing.
}

#endif // UNIT_TEST
