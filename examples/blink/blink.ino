#include <arduFPGA-app-common-arduino.h>

#include <SPI.h>

#define SSD1306_CS_PIN						28
#define SSD1306_DC_PIN						21
#define SSD1306_RST_PIN						8

const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED

sTimer timer = sTimer(500);
ssd1306_spi screen = ssd1306_spi(1024, SSD1306_CS_PIN, SSD1306_DC_PIN, SSD1306_RST_PIN);

void setup() {
  pinMode(ledPin, OUTPUT);
  // Set SPI for display
  //Serial.begin(9600);
  SPI.begin();
  screen.spi->setClockDivider(SPI_CLOCK_DIV4);
  screen.spi->begin();
  screen.init();
  screen.drvDrawString("Some string", 8, 16, 0, 1);
  screen.drvRefresh();
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
