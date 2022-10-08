/*
 * SSD1306 SPI driver.
 *
 * Copyright (C) 2020  Iulian Gheorghiu (morgoth@devboard.tech)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __SSD1306_SPI__
#define __SSD1306_SPI__

#include "Arduino.h"

#include <stdbool.h>
#include <stdint.h>
#include "../device/screen.h"
#include "../include/global.h"


#define SSD1306_BLACK               0    ///< Draw 'off' pixels
#define SSD1306_WHITE               1    ///< Draw 'on' pixels
#define SSD1306_INVERSE             2    ///< Invert pixels

#define SSD1306_MEMORYMODE          0x20 ///< See datasheet
#define SSD1306_COLUMNADDR          0x21 ///< See datasheet
#define SSD1306_PAGEADDR            0x22 ///< See datasheet
#define SSD1306_SETCONTRAST         0x81 ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8D ///< See datasheet
#define SSD1306_SEGREMAP            0xA0 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5 ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6 ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7 ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8 ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< See datasheet
#define SSD1306_DISPLAYON           0xAF ///< See datasheet
#define SSD1306_COMSCANINC          0xC0 ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range

class ssd1306_spi : public Screen {
public:
	ssd1306_spi(SPIClass *spi, uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
	ssd1306_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
	ssd1306_spi(SPIClass *spi, int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
	ssd1306_spi(int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
#ifndef SSD1306_USE_NO_BUF
	ssd1306_spi(SPIClass *spi, void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
	ssd1306_spi(void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
#endif
	~ssd1306_spi();
	void deriverInit();
	static Screen *Init(void *driverHandlerPtr);
	static void Idle(void *driverHandlerPtr);
	static void TriggerRefresh(void *driverHandlerPtr);
	static void TriggerUpdate(void *driverHandlerPtr);
	static int GetX(void *driverHandlerPtr);
	static int GetY(void *driverHandlerPtr);
	static Screen *DrvRefresh(void *driverHandlerPtr);
	static Screen *DrvOn(void *driverHandlerPtr, bool state);
	static Screen *DrvSetContrast(void *driverHandlerPtr, byte cont);
	static Screen *DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color);
	static Screen *DrvDrawPixelClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int color);
	static Screen *DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color);
	static Screen *DrvDrawRectangleClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color);
	static Screen *DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color);
	static Screen *DrvDrawHLineClip(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color);
	static Screen *DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color);
	static Screen *DrvDrawVLineClip(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color);
	static Screen *DrvClear(void *driverHandlerPtr, int color);
	static Screen *DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor);
	static Screen *DrvDrawStringClip(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor);

	void WrCmd(byte cmd);
	void WrData(byte data);


};

#endif
