//Arduino nano board.

#include <arduFPGA-app-common-arduino.h>

const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED

sTimer timer = sTimer(500);
void setup() {
  pinMode(ledPin, OUTPUT);
  timer.Start();
}

void loop() {
  if (timer.Tick()) {
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}
