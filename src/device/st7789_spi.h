/*
 * ST7789 TFT LCD display driver file for arduFPGA designs.
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


#ifndef ST7789_H_
#define ST7789_H_

#if !defined(QT_WIDGETS_LIB)
#include "Arduino.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "../device/screen.h"
#include "../include/global.h"


#define ST7789_TFTWIDTH 	240
#define ST7789_TFTHEIGHT 	240

#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 0

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD


#if defined(QT_WIDGETS_LIB)
typedef char SPIClass;
#endif

class st7789_spi : public Screen {
public:
	st7789_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
	st7789_spi(SPIClass *Spi, uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
	~st7789_spi();

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
	void WrData(uint8_t* buff, size_t buff_size);
	void WrCmd(uint8_t* buff, size_t buff_size);

private:
	inline void select();
	inline void unselect();
	inline void reset();
	void writeData(uint8_t* buff, size_t buff_size);
	void executeCommandList(const uint8_t *addr);
	void setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void setOrientation(uint16_t rotation);
};


#endif /* ST7789_H_ */
