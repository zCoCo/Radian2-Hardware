/**
 * Author Teemu Mäntykallio
 * Initializes the library and turns the motor in alternating directions.
*/
  #define EN      2//PIN_D2
  #define STP     9//PIN_B1
  #define DIR     6//PIN_D6
  #define DIAG1   5//PIN_D5
  #define CS_TMC  A6//ADC6/PE2
  
#define EN_PIN    EN  // Nano v3:	16 Mega:	38	//enable (CFG6)
#define DIR_PIN   DIR  //			19			55	//direction
#define STEP_PIN  STP  //			18			54	//step
#define CS_PIN    CS_TMC  //			17			64	//chip select

bool dir = true;

#include <SPI.h>
#include <TMC2130Stepper.h>
TMC2130Stepper driver = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);

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
  driver.irun(CS);
  driver.ihold(CS*multiplier);
} // #int_current

void setup() {
	Serial.begin(9600);
	while(!Serial);
	Serial.println("Start...");
	SPI.begin();
	pinMode(MISO, INPUT_PULLUP);
	driver.begin(); 			// Initiate pins and registeries
  int_current(1080, 0.5, 6.98); // Set Running Motor RMS Coil Current to 1080mA, with 0.5 IHOLD multiplier, using a Reference Resistor of 6k98Ohm
	
	digitalWrite(EN_PIN, LOW);
}

void loop() {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(10);
	uint32_t ms = millis();
	static uint32_t last_time = 0;
	if ((ms - last_time) > 2000) {
		if (dir) {
			Serial.println("Dir -> 0");
			driver.shaft_dir(0);
		} else {
			Serial.println("Dir -> 1");
			driver.shaft_dir(1);
		}
		dir = !dir;
		last_time = ms;
	}
}
