#include <Streaming.h>
#include <TMCStepper.h>

  // Physical Specs:
  #define MICROSTEPS 2

  // Motor Specs:
  constexpr float FULL_STEPS_PER_REV = 200.0f; // ignoring microstepping
  #define MAX_RPM 90 // [rev/min] Empirical
  #define ACCEL 500 // [deg/s^2]
  #define MAX_RMS_CURRENT 2500 // [mA] - Max. RMS motor coil current

  constexpr float STEPS_PER_REV = MICROSTEPS * FULL_STEPS_PER_REV;
  constexpr float STEPS_PER_DEG = STEPS_PER_REV / 360.0f;

  constexpr float MAX_STEP_SPEED = MAX_RPM * 360.0f * STEPS_PER_DEG / 60.0f; // [stp/sec]

  // Primary Communications:
  #define DEV_ID  PA1   // Device ID Line (analog)

  #define TTX     PA9   // RS485 translator write pin for actuator write line
  #define TXD     TTX   // Diagnostic device write (out) pin
  #define TDE     PA8   // RS485 translator write enable for actuator write line

  #define RRX     PA10  // RS485 translator read pin for actuator read line
  #define RXD     RRX   // Diagnostic device read (in) pin
  #define _RRE    PB15  // RS485 translator read driver enable for actuator read line, active low

  #define MOSI    PA7   // SPI Master-Out, Slave-In (M=MCU, S=Driver)
  #define MISO    PA6   // SPI Master-In, Slave-Out
  #define SCK     PA5   // SPI Clock Line
  #define _CS_DRV PA4   // SPI Slave Chip-Select for Driver, active low

  // Auxiliary Communications:
  #define RTX     PB10  // RS485 translator write pin for actuator read line
  #define RDE     PB1   // RS485 translator write enable for actuator read line

  #define TRX     PB11  // RS485 translator read pin for actuator write line
  #define _TRE    PB14  // RS485 translator read driver enable for actuator write line, active low

  // Board State Sensing and Control:
  #define _PIB    PB7   // Plugged-In-Backwards Detection (upstream device plugged in output and downstream device plugged in input), active low
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
  #define CS_PIN   _CS_DRV // Chip select

  #define R_SENSE   0.075 //TMC5160 SilentStepStick
  //#define R_SENSE 0.11 // TMC2130 SilentStepStick
  
  TMC5160Stepper driver = TMC5160Stepper(CS_PIN, R_SENSE); // all critical driver functions should get wrapped by HAL
  //TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE, MOSI, MISO, SCK); // bit-bangs SW-SPI

constexpr float steps_per_deg = STEPS_PER_DEG;

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

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
    Serial.println("^A"); // RISING
  } else{
    Serial.println(".A"); // FALLING
  }
  update_encoder();
}

void ISR_B(){
  if(digitalRead(SENSB)){
    Serial.println("^B"); // RISING
  } else{
    Serial.println(".B"); // FALLING
  }
  update_encoder();
}

void ISR_C(){
  if(digitalRead(SENSC)){
    Serial.println("^C"); // RISING
  } else{
    Serial.println(".C"); // FALLING
  }
  update_encoder();
}

void ISR_D(){
  if(digitalRead(SENSD)){
    Serial.println("^D"); // RISING
  } else{
    Serial.println(".D"); // FALLING
  }
  update_encoder();
}

void setup() {
    Serial.begin(9600);
    init_core();

    set_fan(100);
    Serial.println(analogRead(VM_SENS));
    printStatus();
}

bool shaft = 1;

void loop() {
    // Check to see if should enter programming mode:
    while(Serial.available() > 0){
      String str = Serial.readString();
      if(str.indexOf("PROG") > -1){
        Serial.println("\t\t - ENTERING PROGRAMMING MODE -");
        Serial.flush();
        digitalWrite(TPROG, LOW);
        delay(750); // Wait for external circuit to trigger RST (should be ~instant).
      }
    }

    delay(5000);
    printStatus();
}

  // CONTROL SIGNALS:

  void printStatus(){
    static char str[100];
    Serial.println("\t--------");
    Serial.println("STATUS:");
    sprintf(str, "\tVoltage: %0.1f", power_voltage()/10.0f);
    Serial.println(str);

    sprintf(str, "\tMotor: still: %d,\t stalled: %d,\t shorted: %d,\t open: %d", motor_still(), motor_stalled(), motor_shorted(), motor_open());
    Serial.println(str);

    sprintf(str, "\tDriver: warm: %d,\t overload: %d", driver_warm(), driver_overloaded());
    Serial.println(str);

    sprintf(str, "\tSENS1: A:%d B%d, \t SENS2: C:%d, D:%d", digitalRead(SENSA), digitalRead(SENSB), digitalRead(SENSC), digitalRead(SENSD));
    Serial.println(str);
    
    Serial.println("\t--------");
  }

  // Sets the Fan Power Level to the Given Value between 0 and 100;
  void set_fan(uint8_t pwr){
    static constexpr uint16_t scaling = 32768 / 100;
    pwmWrite(FAN, (uint16_t) pwr * scaling);
  }

  // DIAGNOTICS:

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
    Serial.println("Initializing HAL");

    // State Sensing:
    pinMode(DEV_ID, INPUT_ANALOG);
    long deviceID = analogRead(DEV_ID);
    Serial << "Device ID: " << deviceID << endl;
    pinMode(VM_SENS, INPUT_ANALOG);
    Serial << "Power Level: " << power_voltage() << "dV" << endl;

    pinMode(_PIB, INPUT);
    pinMode(TERM, INPUT);

    // Programming State:
    digitalWrite(TPROG, HIGH); // Establish initial value before enabling output
    pinMode(TPROG, OUTPUT);
    pinMode(PROG, INPUT);

    if(digitalRead(PROG)){
      Serial.println(
        "MCU programming mode detected. \n"
        "Switching back to normal mode and restarting MCU."
      );
      Serial.flush();
      digitalWrite(TPROG, LOW);
      delay(750); // Wait for external circuit to trigger RST (should be ~instant).
    } else{
      Serial.println("MCU is in normal operating mode.");
    }

    // Encoder Inputs:

    // Must Disable JTAG in AFIO for SENSD, SENSA, and SENSC(?) to work as GPIO:
    afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);
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

    pinMode(_CS_DRV, OUTPUT);
    digitalWrite(_CS_DRV, HIGH);
    SPI.begin();
    pinMode(MISO, INPUT_PULLUP);

    driver.begin();             // Initiate pins and registeries
    driver.rms_current(MAX_RMS_CURRENT);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
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
    driver.sgt(25); // STALL_VALUE:[-64..63] - higher->less sensitive

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
