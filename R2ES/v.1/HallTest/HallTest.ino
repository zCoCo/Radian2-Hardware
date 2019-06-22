//Use with Arduino Pro Mini

//#include <AccelStepper.h>
#include <microsmooth.h>

//Control
#define STEP  6
#define DIR   5

//Sense
#define X1    A1
#define X2    A2

uint16_t *history = ms_init(SGA);
 int processed_value;

void setup(){
  Serial.begin(9600);
  pinMode(X1, INPUT);
  pinMode(X2, INPUT);
  /*pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(500);*/
}

void loop(){
  processed_value = sga_filter(analogRead(X1), history);
  if(0 == 0){
    Serial.println(analogRead(X1));
  }
  delay(1);        // delay in between reads for stability
}
