/**
 * Author Teemu Mäntykallio
 * Initializes the library and turns the motor in alternating directions.
*/

#define DEV // Essentially a TMC2130 SilentStepStick Stuck to an Arduino Nano

int dir = 1;

#include <TMC2130Stepper.h>

#ifndef DEV
  #define EN      2//PIN_D2
  #define STP     9//PIN_B1
  #define DIR     6//PIN_D6
  #define DIAG1   5//PIN_D5
  #define CS_TMC  4//25//ADC6/PE2
    
  #define EN_PIN    EN  // Nano v3: 16 Mega:  38  //enable (CFG6)
  #define DIR_PIN   DIR  //     19      55  //direction
  #define STEP_PIN  STP  //     18      54  //step
  #define CS_PIN    CS_TMC  //      17      64  //chip select
  TMC2130Stepper driver = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);
#else
  #define EN      9
  #define STP     3
  #define DIR     2
  #define CS_TMC  6
  #define SDI     8
  #define SDO     5
  #define SCK     7
      
  #define EN_PIN    EN  // Nano v3: 16 Mega:  38  //enable (CFG6)
  #define DIR_PIN   DIR  //     19      55  //direction
  #define STEP_PIN  STP  //     18      54  //step
  #define CS_PIN    CS_TMC  //      17      64  //chip select
  #define MOSI_PIN  SDI
  #define MISO_PIN  SDO
  #define SCK_PIN   SCK
  TMC2130Stepper driver = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN, MOSI_PIN, MISO_PIN, SCK_PIN);
#endif // DEV?

/* 
  Set Supply RMS Current in mA Using Internal RDSon for Current Sensing and Rref in kOhms
  to provide a Current Reference, Iref.
  Calculations:
  Iref, mA = 5/(1+Rref)
  Ipeak, vsense=0, mA = 3000*Iref
  Ipeak, vsense=1, mA = 0.55*Ipeak, vsense=0
  
  Irms = Ipeak / sqrt(2)
  Isup, mA = (CS+1)/32 * Irms
  
  Isup, vsense=0, mA = (CS+1)/32 * 3000 * 5 / (1+Rref) /sqrt(2)
  Isup, vsense=1, mA = (CS+1)/32 * 0.55 * 3000 * 5 / (1+Rref) / sqrt(2)

  ->CS, vsense=0 = 4 * sqrt(2) * (Rref+1) * Isup / 1875 - 1
  ->CS, vsense=1 = 4 * sqrt(2) * (Rref+1) * Isup / 1031.25 - 1

  N.B.: Standard IHOLD multiplier is 0.5
*/  
void int_current(uint16_t Isup, float multiplier, float Rref) {
  driver.internal_Rsense(1); // Init.
  uint8_t CS = 4.0 * 1.41421 * (Rref+1.0) * Isup / 1875.0 - 1;
  // If Current Scale is too low, turn on high sensitivity R_sense and calculate again
  if (CS < 16) {
    driver.vsense(true);
    CS = 4.0 * 1.41421 * (Rref+1.0) * Isup / 1031.25 - 1;
  } else if(driver.vsense()) { // If CS >= 16, turn off high_sense_r if it's currently ON
    driver.vsense(false);
  }
  Serial.println(CS);
  driver.irun(CS);
  driver.ihold(CS*multiplier);
} // #int_current

#define CURRENT 800

void setup() {
  Serial.begin(9600);
  Serial.println("Start...");
  delay(650); // Wait for Circuit to Initialize
  #ifndef DEV
    SPI.begin();
    Serial.println("1");
    pinMode(MISO, INPUT_PULLUP);
    Serial.println("2");
  	driver.begin(); 			// Initiate pins and registeries
    Serial.println("3");
    int_current(CURRENT, 0.5, 6.98); // Set Running Motor RMS Coil Current to 1080mA, with 0.5 IHOLD multiplier, using a Reference Resistor of 6k98Ohm
  #else
    Serial.println("1");
    Serial.println("DEV");
    Serial.println("2");
    driver.begin();
    Serial.println("3");
    driver.SilentStepStick2130(CURRENT); // eq. to #rms_current
  #endif // DEV?
  Serial.println("4");
  digitalWrite(EN_PIN, LOW);
  Serial.println("Setup ready");
  Serial.print("DRV_STATUS=0b");
  Serial.println(driver.DRV_STATUS(), BIN);
}

void loop() {
  digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(10);
	uint32_t ms = millis();
	static uint32_t last_time = 0;
	if ((ms - last_time) > 2000) {
    Serial.print("Dir -> "); Serial.println(dir);
    driver.shaft_dir(dir);
		dir = !dir;
		last_time = ms;
	}
}
