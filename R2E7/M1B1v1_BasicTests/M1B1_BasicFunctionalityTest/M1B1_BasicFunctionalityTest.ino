#define FAN 23
#define LED 2
#define WIFI_ENABLE 13

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0; // ESP32 PWM Channel
const int resolution = 10; //Resolution 8, 10, 12, 15

void setup() {
  Serial.begin(115200);
  Serial.println("Begin. . .");
  
  pinMode(LED, OUTPUT);
  pinMode(WIFI_ENABLE, INPUT);

  digitalWrite(LED, digitalRead(WIFI_ENABLE));

  // configure "LED" (fan) PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FAN, ledChannel);
}

void loop() {
  // Check to see if should enter programming mode:
  while(Serial.available() > 0){
    int fan_lvl = Serial.parseInt();
    Serial.print(fan_lvl);
    Serial.println(" % PWM");
    ledcWrite(ledChannel, constrain(fan_lvl*10,0,1024)); //PWM Value has 10b resolution
  }
  digitalWrite(LED, digitalRead(WIFI_ENABLE));
}
