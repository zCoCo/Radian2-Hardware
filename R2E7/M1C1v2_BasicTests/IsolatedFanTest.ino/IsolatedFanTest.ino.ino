#define FAN     PB0   // Fan Control Pin, PWM

void setup() {
    pinMode(FAN, PWM);
}

void loop() {
    // Fade in from min to max:
    for (int fadeValue = 0; fadeValue <= 100; fadeValue += 1) {
        set_fan(fadeValue);
        delay(30);
    }

    delay(2000); // hover at max for a bit
    
    // Fade out from max to min:
    for (int fadeValue = 100 ; fadeValue >= 0; fadeValue -= 1) {
        set_fan(fadeValue);
        delay(30);
    }
}

// Sets the Fan Power Level to the Given Value between 0 and 100;
void set_fan(uint8_t pwr){
  static constexpr uint16_t scaling = 32768 / 100;
  pwmWrite(FAN, (uint16_t) pwr * scaling);
}
