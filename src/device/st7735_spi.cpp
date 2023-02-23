/*
 * ST7735 TFT LCD display driver file for arduFPGA designs.
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

#include <stdbool.h>
#include <stdio.h>
#include "st7735_spi.h"
#if !defined(QT_WIDGETS_LIB)
#include <SPI.h>
#else
#define LOW         0
#define HIGH        1
#endif
#include "font6x8.h"

#define DELAY 0x80

#if __AVR_MEGA__
#include <avr/pgmspace.h>
static const unsigned char ST7735_BIT_MASK_TABLE [] PROGMEM = {
	0b00000001,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01000000,
	0b10000000
};
#endif

// based on Adafruit ST7735 library for Arduino
static const uint8_t
init_cmds1[] 
#if __AVR_MEGA__
 PROGMEM
#endif
= {            // Init for 7735R, part 1 (red or green tab)
	15,                       // 15 commands in list:
	ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
	150,                    //     150 ms delay
	ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
	255,                    //     500 ms delay
	ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
	0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
	ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
	0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
	ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
	0x01, 0x2C, 0x2D,       //     Dot inversion mode
	0x01, 0x2C, 0x2D,       //     Line inversion mode
	ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
	0x07,                   //     No inversion
	ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
	0xA2,
	0x02,                   //     -4.6V
	0x84,                   //     AUTO mode
	ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
	0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
	ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
	0x0A,                   //     Opamp current small
	0x00,                   //     Boost frequency
	ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
	0x8A,                   //     BCLK/2, Opamp current small & Medium low
	0x2A,
	ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
	0x8A, 0xEE,
	ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
	0x0E,
	ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
	ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
	ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
	ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
	0x05 },                 //     16-bit color

#ifdef ST7735_128x128
init_cmds2[] 
#if __AVR_MEGA__
 PROGMEM
#endif
= {            // Init for 7735R, part 2 (1.44" display)
	2,                        //  2 commands in list:
	ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F,             //     XEND = 127
	ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifndef ST7735_128x128
init_cmds2[] 
#if __AVR_MEGA__
 PROGMEM
#endif
= {            // Init for 7735S, part 2 (160x80 display)
	3,                        //  3 commands in list:
	ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x4F,             //     XEND = 79
	ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x9F ,            //     XEND = 159
	ST7735_INVON, 0 },        //  3: Invert colors
#endif

init_cmds3[] 
#if __AVR_MEGA__
 PROGMEM
#endif
= {            // Init for 7735R, part 3 (red or green tab)
	4,                        //  4 commands in list:
	ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
	0x02, 0x1c, 0x07, 0x12,
	0x37, 0x32, 0x29, 0x2d,
	0x29, 0x25, 0x2B, 0x39,
	0x00, 0x01, 0x03, 0x10,
	ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
	0x03, 0x1d, 0x07, 0x06,
	0x2E, 0x2C, 0x29, 0x2D,
	0x2E, 0x2E, 0x37, 0x3F,
	0x00, 0x00, 0x02, 0x10,
	ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
	10,                     //     10 ms delay
	ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
	100 };                  //     100 ms delay


void st7735_spi::select() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(CsPin, LOW);
#endif
}

void st7735_spi::unselect() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(CsPin, HIGH);
#endif
}

void st7735_spi::reset() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(RstPin, LOW);
	delay(5);
	digitalWrite(RstPin, HIGH);
#endif
}

void st7735_spi::WrCmd(uint8_t cmd) {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(DcPin, LOW);
	spi->transfer(cmd);
#endif
}

void st7735_spi::writeData(uint8_t* buff, size_t buff_size) {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(DcPin, HIGH);
	//spi_wrd_buf(inst, buff, buff, buff_size);
	for (int cnt = 0; cnt < buff_size; cnt++) {
		spi->transfer(*buff++);
	}
#endif
}

st7735_spi::st7735_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
#if !defined(QT_WIDGETS_LIB)
    spi = &SPI;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
#endif
	setDefault();
	deriverInit();
}

st7735_spi::st7735_spi(SPIClass *Spi, uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
#if !defined(QT_WIDGETS_LIB)
    spi = Spi;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
#endif
	setDefault();
	deriverInit();
}

st7735_spi::~st7735_spi() {

}

void st7735_spi::deriverInit() {
	DriverPtr = this;
	IdlePtr = &st7735_spi::Idle;
	TriggerRefreshPtr = &st7735_spi::TriggerRefresh;
	TriggerUpdatePtr = &st7735_spi::TriggerUpdate;
	InitPtr = &st7735_spi::Init;
	GetXPtr = &st7735_spi::GetX;
	GetYPtr = &st7735_spi::GetY;
	DrvRefreshPtr = &st7735_spi::DrvRefresh;
	DrvOnPtr = &st7735_spi::DrvOn;
	DrvSetContrastPtr = &st7735_spi::DrvSetContrast;
	DrvDrawPixelPtr = &st7735_spi::DrvDrawPixel;
	DrvDrawPixelClipPtr = &st7735_spi::DrvDrawPixelClip;
	DrvDrawRectanglePtr = &st7735_spi::DrvDrawRectangle;
	DrvDrawRectangleClipPtr = &st7735_spi::DrvDrawRectangleClip;
	DrvDrawHLinePtr = &st7735_spi::DrvDrawHLine;
	DrvDrawHLineClipPtr = &st7735_spi::DrvDrawHLineClip;
	DrvDrawVLinePtr = &st7735_spi::DrvDrawVLine;
	DrvDrawVLineClipPtr = &st7735_spi::DrvDrawVLineClip;
	DrvClearPtr = &st7735_spi::DrvClear;
	DrvDrawStringPtr = &st7735_spi::DrvDrawString;
	DrvDrawStringClipPtr = &st7735_spi::DrvDrawStringClip;
}

void st7735_spi::executeCommandList(const uint8_t *addr) {
	uint8_t numCommands, numArgs;
	uint16_t ms;

#ifdef __AVR_MEGA__
	numCommands = pgm_read_byte(addr++);
#else
	numCommands = *addr++;
#endif
	while(numCommands--) {
#ifdef __AVR_MEGA__
		uint8_t cmd = pgm_read_byte(addr++);
#else
		uint8_t cmd = *addr++;
#endif
		WrCmd(cmd);

#ifdef __AVR_MEGA__
		numArgs = pgm_read_byte(addr++);
#else
		numArgs = *addr++;
#endif
		// If high bit set, delay follows args
		ms = numArgs & DELAY;
		numArgs &= ~DELAY;
		if(numArgs) {
#ifdef __AVR_MEGA__
			uint8_t tmp_buf[16];
			memcpy_P(tmp_buf, addr, numArgs);
			writeData(tmp_buf, numArgs);
#else
			writeData((uint8_t*)addr, numArgs);
#endif
			addr += numArgs;
		}

		if(ms) {
#ifdef __AVR_MEGA__
			ms = pgm_read_byte(addr++);
#else
			ms = *addr++;
#endif
			//ms = *addr++;
			if(ms == 255) 
				ms = 500;
#if !defined(QT_WIDGETS_LIB)
            delay(ms);
#endif
		}
	}
}

void st7735_spi::setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
#if !defined(QT_WIDGETS_LIB)
    // column address set
	WrCmd(ST7735_CASET);
	uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
	writeData(data, sizeof(data));

	// row address set
	WrCmd(ST7735_RASET);
	data[1] = y0 + ST7735_YSTART;
	data[3] = y1 + ST7735_YSTART;
	writeData(data, sizeof(data));

	// write to RAM
	WrCmd(ST7735_RAMWR);
#endif
}

void st7735_spi::Idle(void *driverHandlerPtr) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
}

void st7735_spi::TriggerRefresh(void *driverHandlerPtr) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
}

void st7735_spi::TriggerUpdate(void *driverHandlerPtr) {
	DrvRefresh(driverHandlerPtr);
}

Screen *st7735_spi::Init(void *driverHandlerPtr) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	drv->reset();
    drv->select();
    drv->executeCommandList(init_cmds1);
    drv->executeCommandList(init_cmds2);
    drv->executeCommandList(init_cmds3);
    drv->unselect();
    return (Screen *)driverHandlerPtr;
}

int st7735_spi::GetX(void *driverHandlerPtr) {
#ifdef ST7735_128x128
	return 128;
#else
	return 160;
#endif
}

int st7735_spi::GetY(void *driverHandlerPtr) {
#ifdef ST7735_128x128
	return 128;
#else
	return 80;
#endif
}

Screen *st7735_spi::DrvRefresh(void *driverHandlerPtr) {
	DrvOn(driverHandlerPtr, true);
	return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvOn(void *driverHandlerPtr, bool state) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	drv->select();
	//st7735_writeCommand(inst, state ? ST7735_NORON : ST7735_NOROFF);
	drv->WrCmd(state ? ST7735_DISPON : ST7735_DISPOFF);
	drv->unselect();
    return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvSetContrast(void *driverHandlerPtr, byte cont) {
	//spi_wrd_byte(inst, 0x81);
	//spi_wrd_byte(inst, cont);
	//SPI_ST7735_CS_DEASSERT();
    return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	DrvDrawPixelClip(driverHandlerPtr, drv->box, x, y, color);
	return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvDrawPixelClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int color) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	/* Check if outside the display */
    if(x < 0 || x > GetX(driverHandlerPtr) - 1 || y < 0 || y > GetY(driverHandlerPtr) - 1)
		return (Screen *)driverHandlerPtr;
	/* Check if outside the window */
	if(box) {
		if(x < box->x_min ||
		x >= box->x_max ||
		y < box->y_min ||
		y >= box->y_max)
			return (Screen *)driverHandlerPtr;
	}
	drv->select();

    drv->setAddressWindow(x, y, x+1, y+1);
#if !defined(QT_WIDGETS_LIB)
#ifdef ST7735_BW_MODE
	uint8_t data[2];
	if(color) {
		data[0] = 0xFF;
		data[1] = 0xFF;
	} else {
		data[0] = 0x00;
		data[1] = 0x00;
	}
#else
    uint8_t data[] = { color >> 8, color };
#endif
    drv->writeData(data, sizeof(data));
#endif
    drv->unselect();
    return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	return DrvDrawRectangleClip(driverHandlerPtr, drv->box, x, y, x_size, y_size, fill, color);
}

Screen *st7735_spi::DrvDrawRectangleClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	box_t box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = GetX(driverHandlerPtr);
		box__.y_min = 0;
		box__.y_max = GetY(driverHandlerPtr);
	}
	int16_t x_end = x + x_size ,y_end = y + y_size;
	if(x >= box__.x_max ||
		y >= box__.y_max ||
			x_end < box__.x_min ||
				y_end < box__.y_min)
		return (Screen *)driverHandlerPtr;
    int16_t LineCnt = y;

#if !defined(QT_WIDGETS_LIB)
#ifdef ST7735_BW_MODE
	uint8_t data[2];
	if(color) {
		data[0] = 0xFF;
		data[1] = 0xFF;
	} else {
		data[0] = 0x00;
		data[1] = 0x00;
	}
#else
	uint8_t data[] = { color >> 8, color };
#endif
#endif

	if(fill) {
		if(LineCnt < box__.y_min)
			LineCnt = box__.y_min;
		int16_t _x_start = x;
		if(_x_start < box__.x_min)
			_x_start = box__.x_min;
		int16_t _x_end = x_end;
		if(_x_end > box__.x_max)
			_x_end = box__.x_max;
		int16_t width_to_refresh = (_x_end - _x_start);
		if((width_to_refresh + _x_start) > box__.x_max)
			width_to_refresh = (box__.x_max - _x_start);
		
		drv->select();
		drv->setAddressWindow(x, LineCnt, _x_end, y_end);

		uint16_t pix_nr = (LineCnt - y_end) * (x - _x_end);
#if !defined(QT_WIDGETS_LIB)
        digitalWrite(drv->DcPin, HIGH);
		for (;pix_nr > 0; pix_nr--) {
			drv->spi->transfer(data[0]);
			drv->spi->transfer(data[1]);
		}
#endif
		drv->unselect();
		return (Screen *)driverHandlerPtr;
	}
	DrvDrawHLineClip(driverHandlerPtr, box, x, x + x_size, y, 1, color);
	DrvDrawHLineClip(driverHandlerPtr, box, x, x + x_size, y + y_size, 1, color);
	DrvDrawVLineClip(driverHandlerPtr, box, y, y + y_size, x, 1, color);
	DrvDrawVLineClip(driverHandlerPtr, box, y, y + y_size, x + x_size, 1, color);
	return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	return DrvDrawHLineClip(driverHandlerPtr, drv->box, x1, x2, y, width, color);
}

Screen *st7735_spi::DrvDrawHLineClip(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color) {
	int Half_width1 = width - (width>>1);
	int Half_width2 = width - Half_width1;
	DrvDrawRectangleClip(driverHandlerPtr, box, x1, y - Half_width2, x2, y + Half_width1, true, color);
	return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
	return DrvDrawVLineClip(driverHandlerPtr, drv->box, y1, y2, x, width, color);
}

Screen *st7735_spi::DrvDrawVLineClip(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color) {
	int Half_width1 = width - (width>>1);
	int Half_width2 = width - Half_width1;
	DrvDrawRectangleClip(driverHandlerPtr, box, x - Half_width2, y1, x + Half_width1, y2, true, color);
	return (Screen *)driverHandlerPtr;
}

Screen *st7735_spi::DrvClear(void *driverHandlerPtr, int color) {
	DrvDrawRectangleClip(driverHandlerPtr, NULL, 0, 0, GetX(driverHandlerPtr), GetY(driverHandlerPtr), true, color);
	return (Screen *)driverHandlerPtr;
}

/*#####################################################*/
Screen *st7735_spi::DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor) {
	st7735_spi *drv = (st7735_spi *)driverHandlerPtr;
  return DrvDrawStringClip(driverHandlerPtr, drv->box, string, x, y, drv->terminalMode, drv->wordWrap, foreColor, inkColor);
}

Screen *st7735_spi::DrvDrawStringClip(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor) {
	box_t box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = GetX(driverHandlerPtr);
		box__.y_min = 0;
		box__.y_max = GetY(driverHandlerPtr);
	}
	char *pcString = string;
	bool WordWrap = wordWrap;
	//s32 _SelStart = properties->_SelStart;
	//s32 _SelLen = properties->_SelLen;

	int8_t chWidth = 0;
	int8_t chHeight = 0;
	int16_t CharPtr;
	int8_t Tmp = 0;
	int16_t Cursor_X = x;
	int16_t Cursor_Y = y;
	bool ulVisible = true;
	int16_t CharCnt = 0;
	bool ulOpaque = false;
#ifdef __AVR_MEGA__
	chWidth = pgm_read_byte(&fontTable6x8[2]);
	chHeight = pgm_read_byte(&fontTable6x8[3]);
#else
	chWidth = fontTable6x8[2];
	chHeight = fontTable6x8[3];
#endif
	do {
		int8_t Char = *pcString;
		if (Char == 0) {
			return (Screen *)driverHandlerPtr;
		}
#ifdef __AVR_MEGA__
		CharPtr = ((Char - pgm_read_byte(&fontTable6x8[4])) * chWidth) + pgm_read_byte(&fontTable6x8[0]);
		if(Char < pgm_read_byte(&fontTable6x8[4]) || Char > pgm_read_byte(&fontTable6x8[5]))
#else
		CharPtr = ((Char - fontTable6x8[4]) * chWidth) + fontTable6x8[0];
		if (Char < fontTable6x8[4] || Char > fontTable6x8[5])
#endif
		{
			//chWidth_Tmp = chWidth;
			chWidth = 0;
			} else {
			int8_t Temp;
			/* if CompactWriting is true search the character for free cols from right to left and clear them */
			if (!terminalMode) {
				for (Tmp = 1; Tmp < chWidth; Tmp++) {
#ifdef __AVR_MEGA__
					Temp = pgm_read_byte(&fontTable6x8[Tmp + CharPtr]);
#else
					Temp = fontTable6x8[Tmp + CharPtr];
#endif
					if (Temp == 0)
						break;
				}
				Tmp++;
			}
			else {
				Tmp = chWidth;
			}
			if (Cursor_X + Tmp >= box__.x_min
			&& Cursor_X < box__.x_max + Tmp
			&& Cursor_Y + chHeight >= box__.y_min
			&& Cursor_Y < box__.y_max + chHeight) {
				if (ulVisible) {
					int16_t XX = 0;
					int16_t YY = 0;
					for (XX = 0; XX < Tmp; XX++) {
#ifdef __AVR_MEGA__
						Temp = pgm_read_byte(&fontTable6x8[XX + CharPtr]);
#else
						Temp = fontTable6x8[XX + CharPtr];
#endif
						for (YY = 0; YY < chHeight; YY++) {
							if (Temp & 0x1) {
								DrvDrawPixelClip(driverHandlerPtr, &box__,
								XX + Cursor_X, YY + Cursor_Y, inkColor);
							}
							else {
								if (ulOpaque)
									DrvDrawPixelClip(driverHandlerPtr, &box__,
								XX + Cursor_X, YY + Cursor_Y, foreColor);
							}
							Temp = Temp >> 1;
						}
					}
				}
			}
		}
		switch (Char) {
			case '\r':
				Cursor_X = x;
				pcString++;
				break;
			case '\n':
				Cursor_Y += chHeight;
				pcString++;
				break;
			default:
				Cursor_X += Tmp;
				if ((Cursor_X + chWidth > box__.x_max)
				&& WordWrap == true) {
					Cursor_Y += chHeight;
					Cursor_X = x;
				}
				pcString++;
		}
		CharCnt++;
	} while (1);
}
//#######################################################################################
