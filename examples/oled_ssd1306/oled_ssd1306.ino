#include <arduFPGA-app-common-arduino.h>


#include <SPI.h>

#if defined(__AVR_ATmega328P__)
#define DISPLAY_CS_PIN						10
#define DISPLAY_DC_PIN						9
#define DISPLAY_RST_PIN						8
#define DISPLAY_BUSY_PIN					12
#elif defined(MCU_STM32F103C6) || defined(MCU_STM32F103C8)
#define DISPLAY_CS_PIN						28
#define DISPLAY_DC_PIN						21
#define DISPLAY_RST_PIN						8
#define DISPLAY_BUSY_PIN					12
#else
#define DISPLAY_CS_PIN						10
#define DISPLAY_DC_PIN						9
#define DISPLAY_RST_PIN						8
#define DISPLAY_BUSY_PIN					12
#endif

const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED

sTimer timer = sTimer(500);

#if defined(__AVR_ATmega328P__)
ssd1306_spi screen = ssd1306_spi(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN);
//st7789_spi screen = st7789_spi(-1, DISPLAY_DC_PIN, DISPLAY_RST_PIN);
#else
SPIClass spiDisplay = SPIClass();
//SPIClass spiDisplay = SPIClass(2);
//wpdW21_spi screen = wpdW21_spi(&SPI_2, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_BUSY_PIN);
//ssd1306_spi screen = ssd1306_spi(&SPI_2, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN);
//st7735_spi screen = st7735_spi(&SPI_2, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN);
st7789_spi screen = st7789_spi(&spiDisplay, -1, DISPLAY_DC_PIN, DISPLAY_RST_PIN);
#endif

void setup() {
  // Set SPI for display
  SPI.begin();
  screen.spi->setClockDivider(SPI_CLOCK_DIV4);
  screen.spi->begin();
  screen.ScreenOrientation = screenOrientation::LANDSCAPE;
  screen.init();
  screen.drvClear(0);
  gfxString str = gfxString();
  str.setForeColor(1)->setInkColor(1)->setMaxLineLen(16)->setMaxLen(128)->setTabSpaces(4);
  str.drawStringWindowed4x6("Some string", 8, 8, 0, 0);
  screen.drvRefresh();
  delay(1000);
  screen.drvClear(0);
  screen.drawCircle(40, 32, 32, true, 0xFFFFFFFF);
  screen.drvRefresh();
  delay(1000);
  screen.drvClear(0);
  screen.drvDrawString("Some string", 8, 8, 0, 0xFFFFFFFF);
  screen.drvRefresh();
  delay(1000);
  screen.drvClear(0);
  screen.drawTriangle(40, 8, 10, 56, 70, 56, true, 0xFFFFFFFF);
  screen.drvRefresh();
  delay(1000);
  screen.drvClear(0);
  screen.drvDrawString("Another string", 8, 8, 0, 0xFFFFFFFF);
  screen.drvRefresh();
  timer.Start();
}

void loop() {
}
