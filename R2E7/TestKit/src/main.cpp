// Compile using PlatformIO. Latest version tested: 3.6.3

#include <Arduino.h>
#undef min // ETL Support
#undef max

#include "SPI.h" // Platformio LDF has trouble knowing core libraries like this exist unless included here.

#include <HAL.h>

//#include <Units.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while(!Serial); // Wait for UART connection
  Serial.println("Start . . . ");

  init_HAL();
  init_stepper();

	Serial.print("DRV_STATUS=0b");
	Serial.println(driver.DRV_STATUS(), BIN);
}

void blink(uint32_t t){
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  delay(t/2);
  digitalWrite(LED_BUILTIN, LOW);
  delay(t/2);
}

bool dir = true;
void loop2() {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(10);
	uint32_t ms = millis();
	static uint32_t last_time = 0;
	if ((ms - last_time) > 2000) {
		if (dir) {
			Serial.println("Dir -> 0");
			driver.shaft(0);
		} else {
			Serial.println("Dir -> 1");
			driver.shaft(1);
		}
		dir = !dir;
		last_time = ms;
	}
}

unsigned long long last_time = 0;
void loop(){
  if (millis() - last_time > 250){
    Serial.println(stepper.distanceToGo());
    last_time = millis();
  }
  if(stepper.distanceToGo() == 0){
    Serial.println("Blorp");
    delay(100);
    stepper.moveTo(180*STEPS_PER_DEG*dir); // Move 180deg
		dir = !dir;
  }
  stepper.run();
} // #loop

/* Scheduler Notes: *look into migrating to FreeRTOS instead of finishing the scheduler*
// !!If you keep the scheduler, look into using ETL pool for action "creation"/"deletion"
Sch.run() called as frequently as possible.
Sch.concurrent = [void (*fcn)(void)] to be called on every #run event
Contains RingBuffer of (timeToBeExecuted, (*fcn)(void)) events.
Sch::run:
  -find nearest non-concurrent event
  -call each item in concurrent array
    - after each call, ensure next non-concurrent even hasn't passed.
      - if it has passed or is about to, call it and find next nearest n-c event.
  - loop through non-concurrent event schedule buffer and call any events whose
    time has come.
*/
