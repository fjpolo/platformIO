#include <Arduino.h>

// ledPin refers to ESP32 GPIO 23
const int ledPin = 23;

// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin ledPin as an output.
    pinMode(ledPin, OUTPUT);
    // Serial
    Serial.begin(115200);
  }

// the loop function runs over and over again forever
void loop() {
  //
  Serial.println("LED on\r\n");
    digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                  // wait for a second
    Serial.println("LED off\r\n");
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                  // wait for a second
    //
}
