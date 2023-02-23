/*
 * SSD1331 OLED display driverfile for arduFPGA designs
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


#ifndef SSD1331_H_
#define SSD1331_H_

#include <stdbool.h>
#include <stdint.h>
#if !defined(QT_WIDGETS_LIB)
#include <SPI.h>
#endif
#include <include/global.h>
#include "../device/screen.h"
#include "../include/global.h"

#define SSD1331_CMD_DRAWLINE						0x21
#define SSD1331_CMD_DRAWRECTANGLE					0x22
#define SSD1331_CMD_COPYWINDOW						0x23
#define SSD1331_CMD_DIMWINDOW						0x24
#define SSD1331_CMD_CLEARWINDOW						0x25
#define SSD1331_CMD_FILLWINDOW						0x26
#define SSD1331_DISABLE_FILL						0x00
#define SSD1331_ENABLE_FILL							0x01
#define SSD1331_CMD_CONTINUOUSSCROLLINGSETUP		0x27
#define SSD1331_CMD_DEACTIVESCROLLING				0x2E
#define SSD1331_CMD_ACTIVESCROLLING					0x2F

#define SSD1331_CMD_SETCOLUMNADDRESS				0x15
#define SSD1331_CMD_SETROWADDRESS					0x75
#define SSD1331_CMD_SETCONTRASTA					0x81
#define SSD1331_CMD_SETCONTRASTB					0x82
#define SSD1331_CMD_SETCONTRASTC					0x83
#define SSD1331_CMD_MASTERCURRENTCONTROL			0x87
#define SSD1331_CMD_SETPRECHARGESPEEDA				0x8A
#define SSD1331_CMD_SETPRECHARGESPEEDB				0x8B
#define SSD1331_CMD_SETPRECHARGESPEEDC				0x8C
#define SSD1331_CMD_SETREMAP						0xA0
#define SSD1331_CMD_SETDISPLAYSTARTLINE				0xA1
#define SSD1331_CMD_SETDISPLAYOFFSET				0xA2
#define SSD1331_CMD_NORMALDISPLAY					0xA4
#define SSD1331_CMD_ENTIREDISPLAYON					0xA5
#define SSD1331_CMD_ENTIREDISPLAYOFF				0xA6
#define SSD1331_CMD_INVERSEDISPLAY					0xA7
#define SSD1331_CMD_SETMULTIPLEXRATIO				0xA8
#define SSD1331_CMD_DIMMODESETTING					0xAB
#define SSD1331_CMD_SETMASTERCONFIGURE				0xAD
#define SSD1331_CMD_DIMMODEDISPLAYON				0xAC
#define SSD1331_CMD_DISPLAYOFF						0xAE
#define SSD1331_CMD_DISPLAYON						0xAF
#define SSD1331_CMD_POWERSAVEMODE					0xB0
#define SSD1331_CMD_PHASEPERIODADJUSTMENT			0xB1
#define SSD1331_CMD_DISPLAYCLOCKDIV					0xB3
#define SSD1331_CMD_SETGRAySCALETABLE				0xB8
#define SSD1331_CMD_ENABLELINEARGRAYSCALETABLE		0xB9
#define SSD1331_CMD_SETPRECHARGEVOLTAGE				0xBB
#define SSD1331_CMD_SETVVOLTAGE						0xBE

class ssd1331_spi : public Screen {
	ssd1331_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t vccEnPin, uint8_t pModEnPin);
	~ssd1331_spi();
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

#endif /* SSD1331_H_ */
