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


#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "ssd1331_spi.h"
#include "spi.h"
#include "font6x8.h"
#include "SPI.h"

/*#if __AVR_MEGA__
#include <avr/pgmspace.h>
static const unsigned char SSD1306_BIT_MASK_TABLE [] PROGMEM = {
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
*/
ssd1331_spi::ssd1331_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t vccEnPin, uint8_t pModEnPin) {
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	VccEnPin = vccEnPin;
	PModEnPin = pModEnPin;
	spi = &SPI;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	digitalWrite(vccEnPin, HIGH);
	digitalWrite(pModEnPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(vccEnPin, OUTPUT);
	pinMode(pModEnPin, OUTPUT);
	setDefault();
	deriverInit();
}

ssd1331_spi::~ssd1331_spi() {

}

void ssd1331_spi::deriverInit() {
	DriverPtr = this;
	IdlePtr = &ssd1331_spi::Idle;
	TriggerRefreshPtr = &ssd1331_spi::TriggerRefresh;
	TriggerUpdatePtr = &ssd1331_spi::TriggerUpdate;
	InitPtr = &ssd1331_spi::Init;
	GetXPtr = &ssd1331_spi::GetX;
	GetYPtr = &ssd1331_spi::GetY;
	DrvRefreshPtr = &ssd1331_spi::DrvRefresh;
	DrvOnPtr = &ssd1331_spi::DrvOn;
	DrvSetContrastPtr = &ssd1331_spi::DrvSetContrast;
	DrvDrawPixelPtr = &ssd1331_spi::DrvDrawPixel;
	DrvDrawPixelClipPtr = &ssd1331_spi::DrvDrawPixelClip;
	DrvDrawRectanglePtr = &ssd1331_spi::DrvDrawRectangle;
	DrvDrawRectangleClipPtr = &ssd1331_spi::DrvDrawRectangleClip;
	DrvDrawHLinePtr = &ssd1331_spi::DrvDrawHLine;
	DrvDrawHLineClipPtr = &ssd1331_spi::DrvDrawHLineClip;
	DrvDrawVLinePtr = &ssd1331_spi::DrvDrawVLine;
	DrvDrawVLineClipPtr = &ssd1331_spi::DrvDrawVLineClip;
	DrvClearPtr = &ssd1331_spi::DrvClear;
	DrvDrawStringPtr = &ssd1331_spi::DrvDrawString;
	DrvDrawStringClipPtr = &ssd1331_spi::DrvDrawStringClip;
}

void ssd1331_spi::Idle(void *driverHandlerPtr) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
}

void ssd1331_spi::TriggerRefresh(void *driverHandlerPtr) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
}

void ssd1331_spi::TriggerUpdate(void *driverHandlerPtr) {
	DrvRefresh(driverHandlerPtr);
}

Screen *ssd1331_spi::Init(void *driverHandlerPtr) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;

	delay(5);
	digitalWrite(drv->RstPin, LOW);
	delay(2);
	digitalWrite(drv->RstPin, HIGH);
	digitalWrite(drv->VccEnPin, LOW);
	digitalWrite(drv->PModEnPin, HIGH);
	delay(20);
	digitalWrite(drv->RstPin, LOW);
	delay(2);
	digitalWrite(drv->RstPin, HIGH);
	delay(2);
	
	uint8_t cmds[5];
	/* The (un)lock commands register and the byte value to set the register to unlock command mode */
	drv->WrCmd(0xFD);
	drv->WrCmd(0x12);
	//cmds[0] = 0xFD;
	//cmds[1] = 0x12;
	//digitalWrite(drv->CsPin, LOW);
	//drv->spi->transfer(cmds, 2);
	/* Turn off the display with a one byte command */
	drv->WrCmd(SSD1331_CMD_DISPLAYOFF);
	//cmds[0] = SSD1331_CMD_DISPLAYOFF;
	//drv->spi->transfer(cmds, 1);
	/* The driver remap and color depth command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETREMAP);
	drv->WrCmd(0x72);
	//cmds[0] = SSD1331_CMD_SETREMAP;
	//cmds[1] = 0x72;//RGB = 0x72, BGR = 0x76
	//drv->spi->transfer(cmds, 2);
	/* The set display start line command and the single byte value for the top line */
	drv->WrCmd(SSD1331_CMD_SETDISPLAYSTARTLINE);
	drv->WrCmd(0x00);
	//cmds[0] = SSD1331_CMD_SETDISPLAYSTARTLINE;
	//cmds[1] = 0x00;
	//drv->spi->transfer(cmds, 2);
	/* The  command and the single byte value for no vertical offset */
	drv->WrCmd(SSD1331_CMD_SETDISPLAYOFFSET);
	drv->WrCmd(0x00);
	//cmds[0] = SSD1331_CMD_SETDISPLAYOFFSET;
	//cmds[1] = 0x00;
	//drv->spi->transfer(cmds, 2);
	/* A single byte value for a normal display */
	drv->WrCmd(SSD1331_CMD_NORMALDISPLAY);
	//cmds[0] = SSD1331_CMD_NORMALDISPLAY;
	//drv->spi->transfer(cmds, 1);
	/* The multiplex ratio command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETMULTIPLEXRATIO);
	drv->WrCmd(0x3F);
	//cmds[0] = SSD1331_CMD_SETMULTIPLEXRATIO;
	//cmds[1] = 0x3F;
	//drv->spi->transfer(cmds, 2);
	/* The master configuration command and the required single byte value of 0x8E */
	drv->WrCmd(SSD1331_CMD_SETMASTERCONFIGURE);
	drv->WrCmd(0x8E);
	//cmds[0] = SSD1331_CMD_SETMASTERCONFIGURE;
	//cmds[1] = 0x8E;
	//drv->spi->transfer(cmds, 2);
	/* The power saving mode command and the single byte value */
	drv->WrCmd(SSD1331_CMD_POWERSAVEMODE);
	drv->WrCmd(0x0B);
	//cmds[0] = SSD1331_CMD_POWERSAVEMODE;
	//cmds[1] = 0x0B;
	//drv->spi->transfer(cmds, 2);
	/* The set phase length command and the single byte value */
	drv->WrCmd(SSD1331_CMD_PHASEPERIODADJUSTMENT);
	drv->WrCmd(0x31);
	//cmds[0] = SSD1331_CMD_PHASEPERIODADJUSTMENT;
	//cmds[1] = 0x31;
	//drv->spi->transfer(cmds, 2);
	/* The Clock ratio and oscillator frequency command and the single byte value */
	drv->WrCmd(SSD1331_CMD_DISPLAYCLOCKDIV);
	drv->WrCmd(0xF0);
	//cmds[0] = SSD1331_CMD_DISPLAYCLOCKDIV;
	//cmds[1] = 0xF0;
	//drv->spi->transfer(cmds, 2);
	/* The color A 2nd precharge speed command and the single byte value*/
	drv->WrCmd(SSD1331_CMD_SETPRECHARGESPEEDA);
	drv->WrCmd(0x64);
	//cmds[0] = SSD1331_CMD_SETPRECHARGESPEEDA;
	//cmds[1] = 0x64;
	//drv->spi->transfer(cmds, 2);
	/* The color B 2nd precharge speed command and the single byte value*/
	drv->WrCmd(SSD1331_CMD_SETPRECHARGESPEEDB);
	drv->WrCmd(0x78);
	//cmds[0] = SSD1331_CMD_SETPRECHARGESPEEDB;
	//cmds[1] = 0x78;
	//drv->spi->transfer(cmds, 2);
	/* The color C 2nd precharge speed command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETPRECHARGESPEEDC);
	drv->WrCmd(0x64);
	//cmds[0] = SSD1331_CMD_SETPRECHARGESPEEDC;
	//cmds[1] = 0x64;
	//drv->spi->transfer(cmds, 2);
	/* The set precharge voltage command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETPRECHARGEVOLTAGE);
	drv->WrCmd(0x3A);
	//cmds[0] = SSD1331_CMD_SETPRECHARGEVOLTAGE;
	//cmds[1] = 0x3A;
	//drv->spi->transfer(cmds, 2);
	/* The VCOMH Deselect level command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETVVOLTAGE);
	drv->WrCmd(0x3E);
	//cmds[0] = SSD1331_CMD_SETVVOLTAGE;
	//cmds[1] = 0x3E;
	//drv->spi->transfer(cmds, 2);
	/* The set master current attenuation factor command and the single byte value */
	drv->WrCmd(SSD1331_CMD_MASTERCURRENTCONTROL);
	drv->WrCmd(0x06);
	//cmds[0] = SSD1331_CMD_MASTERCURRENTCONTROL;
	//cmds[1] = 0x06;
	//drv->spi->transfer(cmds, 2);
	/* The Color A contrast command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETCONTRASTA);
	drv->WrCmd(0x91);
	//cmds[0] = SSD1331_CMD_SETCONTRASTA;
	//cmds[1] = 0x91;
	//drv->spi->transfer(cmds, 2);
	/* The Color B contrast command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETCONTRASTB);
	drv->WrCmd(0x50);
	//cmds[0] = SSD1331_CMD_SETCONTRASTB;
	//cmds[1] = 0x50;
	//drv->spi->transfer(cmds, 2);
	/* The Color C contrast command and the single byte value */
	drv->WrCmd(SSD1331_CMD_SETCONTRASTC);
	drv->WrCmd(0x7D);
	//cmds[0] = SSD1331_CMD_SETCONTRASTC;
	//cmds[1] = 0x7D;
	//drv->spi->transfer(cmds, 2);
	/* The disable scrolling command */
	//drv->WrData(SSD1331_CMD_CLEARWINDOW);
	//cmds[0] = SSD1331_CMD_CLEARWINDOW;
	//drv->spi->transfer(cmds, 1);
	delay(2);
	drv->WrCmd(SSD1331_CMD_CLEARWINDOW);
	drv->WrCmd(0x00);
	drv->WrCmd(0x00);
	drv->WrCmd(95);
	drv->WrCmd(63);
	//cmds[0] = SSD1331_CMD_CLEARWINDOW;
	//cmds[1] = 0x00;
	//cmds[2] = 0x00;
	//cmds[3] = 95;
	//cmds[4] = 63;
	//drv->spi->transfer(cmds, 5);
	digitalWrite(drv->VccEnPin, HIGH);
	delay(100);
	/* Turn on the display with a one byte command */
	//cmds[0] = SSD1331_CMD_DISPLAYON;
	//drv->spi->transfer((uint8_t*)cmds, 1);
	drv->WrCmd(SSD1331_CMD_DISPLAYON);
	delay(2);
	/* The clear command and the five bytes representing the area to clear */
	drv->WrCmd(SSD1331_CMD_CLEARWINDOW);
	drv->WrCmd(0x00);
	drv->WrCmd(0x00);
	drv->WrCmd(95);
	drv->WrCmd(63);
	//cmds[0] = SSD1331_CMD_CLEARWINDOW; 		// Enter the �clear mode�
	//cmds[1] = 0x00;					// Set the starting column coordinates
	//cmds[2] = 0x00;					// Set the starting row coordinates
	//cmds[3] = 95;	// Set the finishing column coordinates;
	//cmds[4] = 63;	// Set the finishing row coordinates;
	//drv->spi->transfer(cmds, 5);
	digitalWrite(drv->CsPin, HIGH);
	delay(5);
	DrvSetContrast(driverHandlerPtr, 32);
	DrvClear(driverHandlerPtr, false);

	return drv;
}

void ssd1331_spi::WrCmd(byte cmd) {
	digitalWrite(DcPin, LOW);
	digitalWrite(CsPin, LOW);
	spi->transfer(cmd);
	digitalWrite(CsPin, HIGH);
}

void ssd1331_spi::WrData(byte data) {
	digitalWrite(DcPin, HIGH);
	digitalWrite(CsPin, LOW);
	spi->transfer(data);
	digitalWrite(CsPin, HIGH);
}

Screen *ssd1331_spi::DrvOn(void *driverHandlerPtr, bool state) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	digitalWrite(drv->CsPin, LOW);
	drv->WrCmd(state ? 0xAF : 0xAE);
	digitalWrite(drv->CsPin, HIGH);
	return drv;
}

Screen *ssd1331_spi::DrvSetContrast(void *driverHandlerPtr, byte cont) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	digitalWrite(drv->CsPin, LOW);
	drv->WrCmd(SSD1331_CMD_SETCONTRASTA);
	drv->WrCmd(cont);
	digitalWrite(drv->CsPin, HIGH);
	digitalWrite(drv->CsPin, LOW);
	drv->WrCmd(SSD1331_CMD_SETCONTRASTB);
	drv->WrCmd(cont);
	digitalWrite(drv->CsPin, HIGH);
	digitalWrite(drv->CsPin, LOW);
	drv->WrCmd(SSD1331_CMD_SETCONTRASTC);
	drv->WrCmd(cont);
	digitalWrite(drv->CsPin, HIGH);
	delay(5);
	return drv;
}

int ssd1331_spi::GetX(void *driverHandlerPtr) {
	return 96;
}

int ssd1331_spi::GetY(void *driverHandlerPtr) {
	return 64;
}

Screen *ssd1331_spi::DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	DrvDrawPixelClip(driverHandlerPtr, drv->box, x, y, color);
	return (Screen *)driverHandlerPtr;
}

void transfer(ssd1331_spi *drv, uint8_t *buf, int len) {
	for( int i = 0; i < len; i++) {
		drv->spi->transfer(drv->buf[i]);
	}
}

Screen *ssd1331_spi::DrvDrawPixelClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	/* Check if outside the display */
	if(x < 0 || y < 0 || y > 63)
		return drv;
	/* Check if outside the window */
	if(box) {
		if(x < box->x_min ||
			x >= box->x_max ||
				y < box->y_min ||
					y >= box->y_max)
			return drv;
	}
	uint8_t cmds[8];
	cmds[0] = SSD1331_CMD_DRAWLINE;	//draw rectangle
	cmds[1] = x;					// start column
	cmds[2] = y;					// start row
	cmds[3] = x;					// end column
	cmds[4] = y;					//end row

	cmds[5] = color ? 255 : 0;	//R
	cmds[6] = color ? 255 : 0;	//G
	cmds[7] = color ? 255 : 0;	//B
	digitalWrite(drv->CsPin, LOW);
	transfer(drv, cmds, 8);
	//drv->spi->transfer((uint8_t*)cmds, 8);
	digitalWrite(drv->CsPin, HIGH);
	return drv;
}

Screen *ssd1331_spi::DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	return DrvDrawRectangleClip(driverHandlerPtr, drv->box, x, y, x_size, y_size, fill, color);
}

Screen *ssd1331_spi::DrvDrawRectangleClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	box_t box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 96;
		box__.y_min = 0;
		box__.y_max = 64;
	}
	int16_t x_end = x + x_size - 1 ,y_end = y + y_size - 1;
	if(x >= box__.x_max &&
		y >= box__.y_max &&
			x_end < box__.x_min &&
				y_end < box__.y_min)
		return drv;

	uint8_t cmds[13];
	cmds[0] = SSD1331_CMD_FILLWINDOW;	//draw rectangle
	if(fill)
		cmds[1] = SSD1331_ENABLE_FILL;	//draw rectangle
	else
		cmds[1] = SSD1331_DISABLE_FILL;	//draw rectangle
	digitalWrite(drv->CsPin, LOW);
	transfer(drv, cmds, 2);
	//drv->spi->transfer(cmds, 2);
	digitalWrite(drv->CsPin, HIGH);

	cmds[0] = SSD1331_CMD_DRAWRECTANGLE;	//draw rectangle
	cmds[1] = x;					// start column
	cmds[2] = y;					// start row
	cmds[3] = x_end;					// end column
	cmds[4] = y_end;					//end row

	cmds[5] = color ? 255 : 0;	//R
	cmds[6] = color ? 255 : 0;	//G
	cmds[7] = color ? 255 : 0;	//B
	cmds[8] = color ? 255 : 0;	//R
	cmds[9] = color ? 255 : 0;	//G
	cmds[10] = color ? 255 : 0;	//B
	digitalWrite(drv->CsPin, LOW);
	transfer(drv, cmds, 11);
	//drv->spi->transfer(cmds, 11);
	digitalWrite(drv->CsPin, HIGH);
	delay(3);
	return drv;
}

Screen *ssd1331_spi::DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	return DrvDrawHLineClip(driverHandlerPtr, drv->box, x1, x2, y, width, color);
}

Screen *ssd1331_spi::DrvDrawHLineClip(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	box_t box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 96;
		box__.y_min = 0;
		box__.y_max = 64;
	}
	int16_t X1_Tmp = x1, X2_Tmp = x1 + x2 - 1;
	if(X1_Tmp < (int16_t)box__.x_min)
		X1_Tmp = (int16_t)box__.x_min;
	if(X1_Tmp >= (int16_t)box__.x_max)
		X1_Tmp = (int16_t)box__.x_max;
	if(X2_Tmp < (int16_t)box__.x_min)
		X2_Tmp = (int16_t)box__.x_min;
	if(X2_Tmp >= (int16_t)box__.x_max)
		X2_Tmp = (int16_t)box__.x_max;
	if(y < (int16_t)box__.y_min)
		y = (int16_t)box__.y_min;
	if(y >= (int16_t)box__.y_max)
		y = (int16_t)box__.y_max;
	int16_t Half_width1 = (width>>1);
	int16_t Half_width2 = width-Half_width1;
	for(;X1_Tmp < X2_Tmp; X1_Tmp++) {
		int16_t _Y_ = y - Half_width1;
		for(; _Y_ < y + Half_width2; _Y_++)
			DrvDrawPixelClip(driverHandlerPtr, &box__, (int16_t)(X1_Tmp), (int16_t)(_Y_), color);
	}
	return drv;
}

Screen *ssd1331_spi::DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	return DrvDrawVLineClip(driverHandlerPtr, drv->box, y1, y2, x, width, color);
}

Screen *ssd1331_spi::DrvDrawVLineClip(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	box_t box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 96;
		box__.y_min = 0;
		box__.y_max = 64;
	}
	int16_t Y1_Tmp = y1, Y2_Tmp = y1 + y2 - 1;
	if(x < (int16_t)box__.x_min)
		x = (int16_t)box__.x_min;
	if(x >= (int16_t)box__.x_max)
		x = (int16_t)box__.x_max;
	if(Y1_Tmp < (int16_t)box__.y_min)
		Y1_Tmp = (int16_t)box__.y_min;
	if(Y1_Tmp >= (int16_t)box__.y_max)
		Y1_Tmp = (int16_t)box__.y_max;
	if(Y2_Tmp < (int16_t)box__.y_min)
		Y2_Tmp = (int16_t)box__.y_min;
	if(Y2_Tmp >= (int16_t)box__.y_max)
		Y2_Tmp = (int16_t)box__.y_max;
	int16_t Half_width1 = (width>>1);
	int16_t Half_width2 = width-Half_width1;
	for(;Y1_Tmp < Y2_Tmp; Y1_Tmp++) {
		int16_t _X_ = x - Half_width1;
		for(; _X_ < x + Half_width2; _X_++)
			DrvDrawPixelClip(driverHandlerPtr, &box__, (int16_t)(_X_), (int16_t)(Y1_Tmp), color);
	}
	return drv;
}

Screen *ssd1331_spi::DrvClear(void *driverHandlerPtr, int color) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	DrvDrawRectangleClip(driverHandlerPtr, NULL, 0, 0, 96, 64, true, color);
	return drv;
}


Screen *ssd1331_spi::DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
  return DrvDrawStringClip(driverHandlerPtr, drv->box, string, x, y, drv->terminalMode, drv->wordWrap, foreColor, inkColor);
}

Screen *ssd1331_spi::DrvDrawStringClip(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor) {
	ssd1331_spi *drv = (ssd1331_spi *)driverHandlerPtr;
	box_t box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 96;
		box__.y_min = 0;
		box__.y_max = 64;
	}
	char *pcString = string;
	bool WordWrap = wordWrap;
	//s32 _SelStart = properties->_SelStart;
	//s32 _SelLen = properties->_SelLen;

	uint8_t chWidth = 0;
	uint8_t chHeight = 0;
	int16_t CharPtr;
	uint16_t Tmp = 0;
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
	do
	{
		int8_t Char = *pcString;
		if (Char == 0) {
			return drv;
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
		}
		else
		{
			uint8_t Temp;
			/* if CompactWriting is true search the character for free cols from right to left and clear them */
			if (!terminalMode)
			{
				for (Tmp = 1; Tmp < chWidth; Tmp++)
				{
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
			else
			{
				Tmp = chWidth;
			}
			if ((Cursor_X + Tmp >= box__.x_min)
				&& (Cursor_X < box__.x_max + Tmp)
					&& (Cursor_Y + chHeight >= box__.y_min)
						&& (Cursor_Y < box__.y_max + chHeight))
			{
				if (ulVisible)
				{
					uint8_t XX = 0;
					uint8_t YY = 0;
					for (XX = 0; XX < Tmp; XX++)
					{
#ifdef __AVR_MEGA__
						Temp = pgm_read_byte(&fontTable6x8[XX + CharPtr]);
#else
						Temp = fontTable6x8[XX + CharPtr];
#endif
						for (YY = 0; YY < chHeight; YY++)
						{
							if (Temp & 0x1)
							{
								DrvDrawPixelClip(driverHandlerPtr, &box__,
									XX + Cursor_X, YY + Cursor_Y, inkColor);
							}
							else
							{
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
		switch (Char)
		{
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
					&& WordWrap == true)
			{
				Cursor_Y += chHeight;
				Cursor_X = x;
			}
			pcString++;
		}
		CharCnt++;
	} while (1);
}
//#######################################################################################
