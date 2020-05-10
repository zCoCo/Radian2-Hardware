#include <TMCStepper.h>

  // Physical Specs:
  #define MICROSTEPS 2

  // Motor Specs:
  constexpr float FULL_STEPS_PER_REV = 200.0f; // ignoring microstepping
  #define MAX_RPM 90 // [rev/min] Empirical
  #define ACCEL 500 // [deg/s^2]
  #define MAX_RMS_CURRENT 1000 // [mA] - Max. RMS motor coil current

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
  #define EN_PIN   ENA // Enableic
  #define DIR_PIN  DIR // Direction
  #define STEP_PIN STP // Step
  #define CS_PIN   _CS_DRV // Chip select

  #define R_SENSE 0.11 // SilentStepStick
  TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE); // all critical driver functions should get wrapped by HAL

constexpr uint32_t steps_per_deg = STEPS_PER_DEG;

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

char data[100];

void setup() {
    SPI.begin();
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Start...");
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    driver.begin();             // Initiate pins and registeries
    driver.rms_current(MAX_RMS_CURRENT);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
    //driver.en_pwm_mode(0);      // Enable extremely quiet stepping
    //driver.pwm_autoscale(1);
    driver.microsteps(MICROSTEPS);

    stepper.setMaxSpeed(MAX_STEP_SPEED);
    stepper.setAcceleration(ACCEL * STEPS_PER_DEG);
    stepper.setEnablePin(EN_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();

    sprintf(data, "STEPS_PER_REV: %0.5f", STEPS_PER_REV);
    Serial.println(data);
    sprintf(data, "STEPS_PER_DEG: %0.5f", STEPS_PER_DEG);
    Serial.println(data);
    sprintf(data, "MAX_STEP_SPEED: %0.5f", MAX_STEP_SPEED);
    Serial.println(data);
}

bool shaft = 1;

void loop() {
    if (stepper.distanceToGo() == 0) {
        stepper.disableOutputs();
        delay(500);
        shaft = !shaft;
        driver.shaft(shaft);
        Serial.println(shaft ? "Move CW" : "Move CCW");
        stepper.move(360*steps_per_deg);
        stepper.enableOutputs();
    }
    stepper.run();
}
