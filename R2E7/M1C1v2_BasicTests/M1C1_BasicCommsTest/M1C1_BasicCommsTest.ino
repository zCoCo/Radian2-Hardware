// Tests if the RS485 Comm Lines work using the Simplified Hefty Information Tranmission protocol by responding to a basic set of commands.

/* Programming Settings:
Board: Generic STM32F103C series
Variant: STM32F103CB (20k RAM, 128k Flash) <- switch to C8 according to what's on board
Upload method: Serial
CPU Speed: 72MHz (Normal)
Optimize: Smallest
*/

#include "shitp_comms.h"
#include <TMCStepper.h>
#include <FastLED.h> // Using STM32 Fork for Platformio

// NB: al_ indicates active low

  // Debugging:
  void dbg_print(char* c){
    #ifdef M1_DEBUG
      Serial.print(c);
    #endif
  }
  void dbg_println(char* c){
    #ifdef M1_DEBUG
      Serial.println(c);
    #endif
  }

  // Physical Specs:
  #define MICROSTEPS 2

  // Motor Specs:
  constexpr float GEAR_RATIO = 1.323529f; // Gear Ratio (output revolutions to motor revolutions)
  constexpr float FULL_STEPS_PER_MOTOR_REV = 200.0f; // ignoring microstepping
  constexpr float MAX_RPM = 70.0f / GEAR_RATIO; // [rev/min] Empirical
  #define ACCEL 500 // [deg/s^2]
  #define MAX_RMS_CURRENT 1500 // [mA] - Max. RMS motor coil current

  constexpr float STEPS_PER_REV = MICROSTEPS * FULL_STEPS_PER_MOTOR_REV * GEAR_RATIO;
  constexpr float STEPS_PER_DEG = STEPS_PER_REV / 360.0f;

  constexpr float MAX_STEP_SPEED = MAX_RPM * 360.0f * STEPS_PER_DEG / 60.0f; // [stp/sec]

  // Primary Communications:
  #define DEV_ID  PA1   // Device ID Line (analog)

  #define TTX     PA9   // RS485 translator write pin for actuator write line
  #define TXD     TTX   // Diagnostic device write (out) pin
  #define TDE     PA8   // RS485 translator write enable for actuator write line

  #define RRX     PA10  // RS485 translator read pin for actuator read line
  #define RXD     RRX   // Diagnostic device read (in) pin
  #define al_RRE  PB15  // RS485 translator read driver enable for actuator read line, active low

  #define MOSI    PA7   // SPI Master-Out, Slave-In (M=MCU, S=Driver)
  #define MISO    PA6   // SPI Master-In, Slave-Out
  #define SCK     PA5   // SPI Clock Line
  #define al_CS_DRV PA4   // SPI Slave Chip-Select for Driver, active low

  // Auxiliary Communications:
  #define RTX     PB10  // RS485 translator write pin for actuator read line
  #define RDE     PB1   // RS485 translator write enable for actuator read line

  #define TRX     PB11  // RS485 translator read pin for actuator write line
  #define al_TRE  PB14  // RS485 translator read driver enable for actuator write line, active low

  // Board State Sensing and Control:
  #define al_PIB  PB7   // Plugged-In-Backwards Detection (upstream device plugged in output and downstream device plugged in input), active low
  #define TERM    PB6   // Whether RS-485 Line is (automatically) Terminated
  #define PROG    PB9   // Whether Device is Currently in Programming Mode

  // Programming Mode Toggle: Keep HIGH normally. When brought LOW, external
  // circuit triggers MCU reset for 750ms, during which time TPROG pin on MCU
  // goes High-Z, causing TPROG line to be brought HIGH by external pull-up. The
  // rise of the TPROG line causes a flipflop to latch the PROG line HIGH which
  // sets BOOT0 and TDE HIGH through a series resistor.
  // Bringing TPROG low again while PROG is high performs these tasks again but
  // ending with PROG LOW.
  #define TPROG   PB8   // Pulse HIGH then LOW to Force MCU into Programming Mode and Trigger Reset, keep high under normal operation

  // System Sensing:
  #define VM_SENS PA0   // ADC pin for to Voltage Divider on Main Power Line, VM
  #define SENSA   PB4   // Left Encoder Sensor 1
  #define SENSB   PB5   // Left Encoder Sensor 2
  #define SENSC   PA15  // Right Encoder Sensor 1
  #define SENSD   PB3   // Right Encoder Sensor 2

  // Control Signals:
  #define DIAG1   PB13  // Stepper Driver (TMC2130) Diagnostic 1 Pin
  #define STP     PA3   // Indexer Step Pin
  #define DIR     PA2   // Indexer Direction Pin
  #define ENA     PC13  // Indexer Enable Pin
  #define SLP     -1    // Stepper Driver Sleep Pin (NC)

  #define SERV    PB2   // Also BOOT1 (DON'T output on this line while in programming mode, BOOT0=H or PROG=H).
  #define FAN     PB0   // Fan Control Pin, PWM

  #define LEDS_DO PB12  // Data Output for WS2811 RGB LED Control

  // SOFTWARE SETUP:

  // Macros expected by TMC Library:
  #define EN_PIN   ENA // Enable
  #define DIR_PIN  DIR // Direction
  #define STEP_PIN STP // Step
  #define CS_PIN   al_CS_DRV // Chip select

  #define R_SENSE  0.075 //TMC5160 SilentStepStick
  //#define R_SENSE 0.11 // TMC2130 SilentStepStick

  TMC5160Stepper driver = TMC5160Stepper(CS_PIN, R_SENSE); // all critical driver functions should get wrapped by HAL
  //TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE, MOSI, MISO, SCK); // bit-bangs SW-SPI

constexpr float steps_per_deg = STEPS_PER_DEG;

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

#define NUM_LEDS 6
CRGB leds[NUM_LEDS];

char data[100];

volatile bool sensorState[4];

void update_encoder(){
  sensorState[0] = digitalRead(SENSA);
  sensorState[1] = digitalRead(SENSB);
  sensorState[2] = digitalRead(SENSC);
  sensorState[3] = digitalRead(SENSD);
}

void ISR_A(){
  if(digitalRead(SENSA)){
    dbg_println("^A"); // RISING
  } else{
    dbg_println(".A"); // FALLING
  }
  update_encoder();
}

void ISR_B(){
  if(digitalRead(SENSB)){
    dbg_println("^B"); // RISING
  } else{
    dbg_println(".B"); // FALLING
  }
  update_encoder();
}

void ISR_C(){
  if(digitalRead(SENSC)){
    dbg_println("^C"); // RISING
  } else{
    dbg_println(".C"); // FALLING
  }
  update_encoder();
}

void ISR_D(){
  if(digitalRead(SENSD)){
    dbg_println("^D"); // RISING
  } else{
    dbg_println(".D"); // FALLING
  }
  update_encoder();
}

// Set LEDs to the specified color will set all LEDs unless given a start and
// end index.
void set_color(color_t color, uint8_t fromLED = 0, uint8_t toLED = NUM_LEDS){
  static uint8_t led;
  // Update LEDs:
  FastLED.clear();
  for(led=fromLED; led<=toLED; led++){
    leds[led] = CRGB(color);
  }
  FastLED.show();
}

// -- COMMS --

// Communication Lines:
long baud = 115200;
address_t fake_device_id = 0x01; // "fake" as in not driven by DEV_ID (yet).
SHITP_CommsLine PrimaryComms(&Serial, &init_serial, &fake_device_id, al_RRE, TDE, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainA.

// Interprets / acts upon the given command with the associated data payload.
void command_interpreter(Command cmd, uint8_t* data){
  static ByteUnion<float> floatunion; // Helper destructuring union
  static ByteUnion<color_t> colorunion; // Helper destructuring union
  static ByteUnion<uint16_t> u16union; // Helper destructuring union
  static ByteUnion<address_t> addrunion; // Helper destructuring union
  static SHITP_Message msg; // response message being built
  static char buff[Commands::debug.len]; // buffer for debug messages

  static bool wait_for_move_completion = false; // whether waiting for the current move to complete before accepting a new setpoint
  static unsigned long setpoint_hold_expiration = 0; // ms time when the current waypoint hold ends.

  switch(cmd.id){
    case Commands::move_to.id:
      memcpy(floatunion.data, data, sizeof(float)); // Convert the data
      if(
        !(wait_for_move_completion && stepper.distanceToGo() > 0) // there isn't a hold until setpoint arrival
        && millis() > setpoint_hold_expiration // there isn't a hold until setpoint arrival
      ){
        stepper.moveTo(floatunion.val*steps_per_deg);
        wait_for_move_completion = false; // reset
      }
      if(stepper.distanceToGo() != 0){ // putting this after moveTo implictly also checks for if new setpoint == current pos
        set_color(COLOR::active);
      }
    break;

    case Commands::set_speed.id:
      memcpy(u16union.data, data, sizeof(uint16_t)); // Convert the data
      stepper.setMaxSpeed(u16union.val*steps_per_deg);
    break;

    case Commands::enable.id:
      if(data[0] == 1){
        stepper.enableOutputs();
      } else if(data[0] == 0){
        stepper.disableOutputs();
      }
    break;

    case Commands::complete_move.id:
      if(data[0] == 1){
        wait_for_move_completion = true;
      }
    break;

    case Commands::hold_setpoint.id:
      memcpy(u16union.data, data, sizeof(uint16_t)); // Convert the data
      setpoint_hold_expiration = millis() + u16union.val;
    break;

    case Commands::led_color.id:
      memcpy(colorunion.data, data, sizeof(color_t)); // Convert the data
      set_color(colorunion.val);
    break;

    case Commands::ping.id:
      msg.header.data.address = 0xFF; // basestation
      msg.header.data.command = Commands::debug;
      memset(buff, ' ', sizeof(buff));
      sprintf(buff, "< Device %d Ping: %d >", fake_device_id, data[0]);
      memcpy(msg.data, buff, Commands::debug.len); // Copy the data back over
      PrimaryComms.write(msg);
    break;

    case Commands::prog.id:
      memcpy(addrunion.data, data, sizeof(address_t)); // Convert the data
      if(addrunion.val == fake_device_id){ // extra check to prevent off-chance coincidental triggering
        msg.header.data.address = 0xFF; // basestation
        msg.header.data.command = Commands::debug;
        memset(buff, ' ', sizeof(buff));
        sprintf(buff, "< Device %d entering PROG >", fake_device_id);
        memcpy(msg.data, buff, Commands::debug.len);
        PrimaryComms.write(msg);

        enter_programming_mode();
      }
    break;
  }
}

void init_serial(long baud){
  Serial.begin(baud);
}

void setup() {
    PrimaryComms.begin();
    init_core();

    set_fan(100);
    FastLED.addLeds<WS2811, LEDS_DO, RGB>(leds, NUM_LEDS);
    // FastLED.setBrightness(127);
    set_color(COLOR::white);

    stepper.setMaxSpeed(MAX_STEP_SPEED);
    stepper.setAcceleration(ACCEL * STEPS_PER_DEG);
    stepper.setEnablePin(EN_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();

    set_color(COLOR::idle);
}

bool fault_state = 0; // if there's a fault (ex. stall or overheat)
long last_fault_check = 0; // time of last fault check
long last_fault_blink = 0; // time of last fault blink
bool fault_blink_state = 0; //if fault it will blink error color

bool anom_state = 0; // if there's an anomaly (ex. warm driver)
long last_anom_check = 0; // time of last anomaly check
long last_anom_blink = 0; // time of last anomaly blink
bool anom_blink_state = 0; //if anomaly it will blink warning color

long last_blink = 0; // time of last heartbeat blink
bool blink_state = 0;
bool move_done = true;
void loop() {
  // Update Comms:
  PrimaryComms.update();

  // Update LED Bar:
  if (stepper.distanceToGo() == 0){
    if(!move_done){ // Run only once when a move has been completed
      move_done = true;
      set_color(COLOR::idle);
    }
  } else{
    move_done = false;
  }

  // Heartbeat:
  if((millis() - last_blink) >= 1000){
    blink_state = !blink_state;
    leds[0] = blink_state ? CRGB::FloralWhite : leds[1];
    FastLED.show();
    last_blink = millis();
  }

  // Check Anomaly State every 1.5s (semi-expensive):
  /*if((millis() - last_anom_check) >= 1500){
    bool prev_anom_state = anom_state;
    anom_state = driver_warm();
    if(!anom_state && prev_anom_state){ // If exiting fault, set color back to normal
      set_color(move_done ? COLOR::idle : COLOR::active);
    }
    last_anom_check = millis();
  }
  // Blink every 1000ms on anomaly:
  if(anom_state && (millis() - last_anom_blink) >= 1000){
    anom_blink_state = !anom_blink_state;
    set_color(anom_blink_state ? COLOR::warning : COLOR::off);
    last_anom_blink = millis();
  }

  // Check Fault State every 1s (semi-expensive):
  if((millis() - last_fault_check) >= 1000){
    bool prev_fault_state = fault_state;
    fault_state = driver_overloaded() || motor_stalled() || motor_open();
    if(!fault_state && prev_fault_state){ // If exiting fault, set color back to normal
      set_color(move_done ? COLOR::idle : COLOR::active);
    }
    last_fault_check = millis();
  }
  // Blink every 250ms on fault:
  if(fault_state && (millis() - last_fault_blink) >= 250){
    fault_blink_state = !fault_blink_state;
    set_color(fault_blink_state ? COLOR::error : COLOR::off);
    last_fault_blink = millis();
  }*/

  // Update Stepper:
  stepper.run(); // TODO: put in a timer interrupt

}

  // CONTROL SIGNALS:
  void enter_programming_mode(){
    stepper.disableOutputs(); // Unlock the stepper
    delay(200); // make sure the above happens
    PrimaryComms.stream->flush(); // Ensure all communications have been finished
    digitalWrite(TPROG, LOW); // Trigger
    delay(750); // Block and wait for external circuit to trigger RST (should be ~instant, will cut this delay off)
  }

  // Sets the Fan Power Level to the Given Value between 0 and 100;
  void set_fan(uint8_t pwr){
    static constexpr uint16_t scaling = 32768 / 100;
    pwmWrite(FAN, (uint16_t) pwr * scaling);
  }

  // DIAGNOTICS:
  void printStatus(){
    static char str[100];
    dbg_println("\t--------");
    dbg_println("STATUS:");
    sprintf(str, "\tVoltage: %0.1f", power_voltage()/10.0f);
    dbg_println(str);

    sprintf(str, "\tMotor: still: %d,\t stalled: %d,\t shorted: %d,\t open: %d", motor_still(), motor_stalled(), motor_shorted(), motor_open());
    dbg_println(str);

    sprintf(str, "\tDriver: warm: %d,\t overload: %d", driver_warm(), driver_overloaded());
    dbg_println(str);

    sprintf(str, "\tSENS1: A:%d B%d, \t SENS2: C:%d, D:%d", digitalRead(SENSA), digitalRead(SENSB), digitalRead(SENSC), digitalRead(SENSD));
    dbg_println(str);

    dbg_println("\t--------");
  }

  // Returns the Voltage of the Board and Actuator's Power Supply, VM, in
  // decivolts (eg. 11.9V = 119)
  uint8_t power_voltage(){
    static constexpr float scaling = 10.0f / 4095.0f * 3.3f * 7.4f / 1.8f; // 12bit ADC
    return analogRead(VM_SENS) * scaling;
  }

  // Whether the motor is currently in stand-still:
  bool motor_still(){
    return driver.stst(); // TODO: Add encoder feedback checking
  }

  // Detect if a motor stall event has been reported on DIAG1 (which is active low).
  bool motor_stalled(){
    // Check both in case one diagnostic path becomes broken:
    return !digitalRead(DIAG1) || driver.stallguard();
  }

  // Detect if either motor coil is shorted to ground:
  bool motor_shorted(){
    // Check both in case one diagnostic path becomes broken:
    return driver.s2ga() | driver.s2gb();
  }

  /*
   * Detect if either motor coil circuit is reporting an open loop (broken).
   * NOTE: False postives can be triggered by:
      - Low motor voltage
      - High motor velocities
      - No motor velocity (stand-still)
      - Motor short
      - High driver temperatures
    * Therefore, only perform this test while running at low velocity.
    */
  bool motor_open(){
    // Either coil A or B reports open loop AND driver has not been sitting in standstill (for 2^20 clocks):
    return (driver.ola() | driver.olb()) & !motor_still();
  }

  // Whether the motor driver is near an over-temperature shutdown.
  bool driver_warm(){
    return driver.otpw(); // over-temperature pre-warning flag
  }

  /*
   * Whether the driver has shutdown due to thermal overload.
   * NOTE: Once this threshold has been crossed, it will remain in shutdown
   * driver temperature drops below optw.
   */
  bool driver_overloaded(){
    return driver.ot(); // over-temperature flag
  }

  // Initialize Hardware Components:
  void init_core(){
    dbg_println("Initializing HAL");

    // State Sensing:
    pinMode(DEV_ID, INPUT_ANALOG);
    long deviceID = analogRead(DEV_ID);
    sprintf(data, "Device ID: %d", deviceID);
    dbg_println(data);
    pinMode(VM_SENS, INPUT_ANALOG);
    sprintf(data, "Power Level:  %ddV", power_voltage());
    dbg_println(data);

    pinMode(al_PIB, INPUT);
    pinMode(TERM, INPUT);

    // Programming State:
    digitalWrite(TPROG, HIGH); // Establish initial value before enabling output
    pinMode(TPROG, OUTPUT);
    pinMode(PROG, INPUT);

    if(digitalRead(PROG)){
      dbg_println(
        "MCU programming mode detected. \n"
        "Switching back to normal mode and restarting MCU."
      );
      delay(250);
      digitalWrite(TPROG, LOW);
      delay(750); // Wait for external circuit to trigger RST (should be ~instant).
    } else{
      dbg_println("MCU is in normal operating mode.");
    }

    // Encoder Inputs:

    // Must Disable JTAG in AFIO for SENSD, SENSA, and SENSC(?) to work as GPIO:
    afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY); // AFIO_DEBUG_NONE?
    pinMode(SENSA, INPUT);
    pinMode(SENSB, INPUT);
    pinMode(SENSC, INPUT);
    pinMode(SENSD, INPUT);
    attachInterrupt(digitalPinToInterrupt(SENSA), ISR_A, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSB), ISR_B, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSC), ISR_C, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SENSD), ISR_D, CHANGE);

    // Control:
    pinMode(FAN, PWM);
    // pinMode(SERV, OUTPUT); // PB2/BOOT1 (shared by SERV) might not have a timer. Proceed with caution.
    pinMode(LEDS_DO, OUTPUT);

    pinMode(DIAG1, INPUT);
    pinMode(STP, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(ENA, OUTPUT);
    digitalWrite(ENA, LOW);

    pinMode(al_CS_DRV, OUTPUT);
    digitalWrite(al_CS_DRV, HIGH);
    SPI.begin();
    pinMode(MISO, INPUT_PULLUP);

    driver.begin();             // Initiate pins and registeries
    driver.rms_current(MAX_RMS_CURRENT);    // The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
    //driver.stealthChop(1);      // Enable extremely quiet stepping
    //driver.stealth_autoscale(1);
    driver.microsteps(MICROSTEPS);
    driver.diag1_stall(1);
    driver.diag1_pushpull(1);

    driver.push(); // Push shadow registers
      driver.tbl(1);
      driver.TPOWERDOWN(255);
    driver.toff(4);

    // coolStep Threshold:
    driver.TCOOLTHRS(0xFFFFF); // 20bit max
    driver.THIGH(0);

    // stallGuard2:
    driver.semin(6);
    driver.semax(3);
    driver.sedn(0b01);
    driver.seup(0b11);
    driver.sgt(20); // STALL_VALUE:[-64..63] - higher->less sensitive

    // Effective hysteresis = 0
    driver.hstrt(0);
    driver.hend(2);

    // stealthChop PWM:
    /*driver.en_pwm_mode(true);
    driver.pwm_freq(1);
    driver.pwm_autoscale(true);
    driver.pwm_ampl(180);
    driver.pwm_grad(1);*/

    // Ensure DIAG1 only reports on motor stall:
    pinMode(DIAG1, INPUT_PULLUP); // rated as weak pull-up at 30-50kOhm, DIAG1 is spec'd for 47kOhm max.
    driver.diag1_pushpull(0); // Driver will pull up to 5V if enabled and DIAG1 is not on 5V tolerant pin
    driver.diag1_stall(1);
    driver.diag1_index(0);
    driver.diag1_onstate(0);

    digitalWrite(EN_PIN, LOW); // Re-nable
  } // #init_core
