int ledPin = PC13;
void setup() {
    pinMode(ledPin, PWM);  // setup the pin as PWM
}

void loop()  {
    // Fade in from min to max in increments of 1280 points:
    for (int fadeValue = 0; fadeValue <= 65535; fadeValue += 1280) {
        // Sets the value (range from 0 to 65535):
        pwmWrite(ledPin, fadeValue);
        // Wait for 30 milliseconds to see the dimming effect:
        delay(30);
    }

    // Fade out from max to min in increments of 1280 points:
    for (int fadeValue = 65535 ; fadeValue >= 0; fadeValue -= 1280) {
        // Sets the value (range from 0 to 1280):
        pwmWrite(ledPin, fadeValue);
        // Wait for 30 milliseconds to see the dimming effect:
        delay(30);
    }
}
