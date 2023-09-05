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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(QT_WIDGETS_LIB)
#include <SPI.h>
#else
#define DISPLAY_ZOOM    4
#define LOW 0
#define HIGH 1
#endif
#include "../include/global.h"
#include "ssd1306_spi.h"
#include "font6x8.h"

#if defined(QT_WIDGETS_LIB)
#include <QRectF>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsView>
extern QGraphicsView *graphicsView_Display;
extern QGraphicsScene *scene;
#endif

#ifndef SSD1306_USE_NO_BUF
#if __AVR_MEGA__
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
#endif // __AVR_MEGA__
#endif // SSD1306_USE_BUF

#if !defined(QT_WIDGETS_LIB)
uint8_t CsPin;
uint8_t DcPin;
uint8_t RstPin;
uint8_t Busy;
SPIClass *spi;

static inline void SPI_SSD1306_CS_ASSERT(uint8_t CsPin) {
	digitalWrite(CsPin, LOW);
}

static inline void SPI_SSD1306_CS_DEASSERT(uint8_t CsPin) {
	digitalWrite(CsPin, HIGH);
}

static inline void SPI_SSD1306_COMMAND(uint8_t DcPin) {
	digitalWrite(DcPin, LOW);
}

static inline void SPI_SSD1306_DATA(uint8_t DcPin) {
	digitalWrite(DcPin, HIGH);
}
#endif

#if defined(QT_WIDGETS_LIB)
ssd1306_spi::ssd1306_spi(SPIClass *Spi) {
    memset(this, 0, sizeof(this));
#ifndef SSD1306_USE_NO_BUF
#ifndef SSD1306_BUF_SIZE_BYTES
    setBuf(malloc(1024));
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
    setDefault();
    deriverInit();
}
#endif

ssd1306_spi::ssd1306_spi(SPIClass *Spi, int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
    memset(this, 0, sizeof(this));
#ifndef SSD1306_USE_NO_BUF
#ifndef SSD1306_BUF_SIZE_BYTES
    setBuf(malloc(bufSize));
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
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

ssd1306_spi::ssd1306_spi(int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	memset(this, 0, sizeof(this));
#ifndef SSD1306_USE_NO_BUF
#ifndef SSD1306_BUF_SIZE_BYTES
	setBuf(malloc(bufSize));
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
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

ssd1306_spi::ssd1306_spi(SPIClass *Spi, uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	memset(this, 0, sizeof(this));
#ifndef SSD1306_USE_NO_BUF
#ifndef SSD1306_BUF_SIZE_BYTES
	setBuf(malloc(1024));
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
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


ssd1306_spi::ssd1306_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	memset(this, 0, sizeof(this));
#ifndef SSD1306_USE_NO_BUF
#ifndef SSD1306_BUF_SIZE_BYTES
	setBuf(malloc(1024));
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
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


#ifndef SSD1306_USE_NO_BUF
ssd1306_spi::ssd1306_spi(SPIClass *Spi, void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	memset(this, 0, sizeof(this));
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
#if !defined(QT_WIDGETS_LIB)
    spi = Spi;
	setBuf(vBuf);
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
ssd1306_spi::ssd1306_spi(void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin) {
	memset(this, 0, sizeof(this));
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
#if !defined(QT_WIDGETS_LIB)
    spi = &SPI;
	setBuf(vBuf);
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
#endif

ssd1306_spi::~ssd1306_spi() {
	//DrvClear(false);
	//DrvRefresh();
#ifndef SSD1306_USE_NO_BUF
#ifndef SSD1306_BUF_SIZE_BYTES
	if(getBuf())
		free(getBuf());
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
	remove();
}

void ssd1306_spi::deriverInit() {
	DriverPtr = this;
	IdlePtr = &ssd1306_spi::Idle;
	TriggerRefreshPtr = &ssd1306_spi::TriggerRefresh;
	TriggerUpdatePtr = &ssd1306_spi::TriggerUpdate;
	InitPtr = &ssd1306_spi::Init;
	GetXPtr = &ssd1306_spi::GetX;
	GetYPtr = &ssd1306_spi::GetY;
	DrvRefreshPtr = &ssd1306_spi::DrvRefresh;
	DrvOnPtr = &ssd1306_spi::DrvOn;
	DrvSetContrastPtr = &ssd1306_spi::DrvSetContrast;
	DrvDrawPixelPtr = &ssd1306_spi::DrvDrawPixel;
	DrvDrawPixelClipPtr = &ssd1306_spi::DrvDrawPixelClip;
	DrvDrawRectanglePtr = &ssd1306_spi::DrvDrawRectangle;
	DrvDrawRectangleClipPtr = &ssd1306_spi::DrvDrawRectangleClip;
	DrvDrawHLinePtr = &ssd1306_spi::DrvDrawHLine;
	DrvDrawHLineClipPtr = &ssd1306_spi::DrvDrawHLineClip;
	DrvDrawVLinePtr = &ssd1306_spi::DrvDrawVLine;
	DrvDrawVLineClipPtr = &ssd1306_spi::DrvDrawVLineClip;
	DrvClearPtr = &ssd1306_spi::DrvClear;
	DrvDrawStringPtr = &ssd1306_spi::DrvDrawString;
	DrvDrawStringClipPtr = &ssd1306_spi::DrvDrawStringClip;
}

void ssd1306_spi::Idle(void *driverHandlerPtr) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
}

void ssd1306_spi::TriggerRefresh(void *driverHandlerPtr) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
}

void ssd1306_spi::TriggerUpdate(void *driverHandlerPtr) {
	DrvRefresh(driverHandlerPtr);
}

Screen *ssd1306_spi::Init(void *driverHandlerPtr) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(drv->RstPin, LOW);
	delay(2);
	digitalWrite(drv->RstPin, HIGH);
	delay(10);

	drv->WrCmd(SSD1306_DISPLAYOFF);
	drv->WrCmd(SSD1306_SETDISPLAYCLOCKDIV);
	drv->WrCmd(0xF0);
	drv->WrCmd(SSD1306_CHARGEPUMP);
	drv->WrCmd(0x14);
	drv->WrCmd(SSD1306_SEGREMAP | 0x1);
	drv->WrCmd(SSD1306_COMSCANDEC);
	drv->WrCmd(SSD1306_SETCONTRAST);
	drv->WrCmd(0x8F);
	drv->WrCmd(SSD1306_SETPRECHARGE);
	drv->WrCmd(0xF1);
	drv->WrCmd(SSD1306_MEMORYMODE);
	drv->WrCmd(0x00);
	
	delay(2);
	drv->WrCmd(SSD1306_DISPLAYON);
	delay(2);
#endif
	DrvClear(driverHandlerPtr, false);
	DrvRefresh(driverHandlerPtr);
	return (Screen *)driverHandlerPtr;
}

void ssd1306_spi::WrCmd(byte cmd) {
#if !defined(QT_WIDGETS_LIB)
    SPI_SSD1306_COMMAND(DcPin);
	SPI_SSD1306_CS_ASSERT(CsPin);
	spi->transfer(cmd);
#endif
}

void ssd1306_spi::WrData(byte data) {
#if !defined(QT_WIDGETS_LIB)
    SPI_SSD1306_DATA(DcPin);
	SPI_SSD1306_CS_ASSERT(CsPin);
	spi->transfer(data);
#endif
}

int ssd1306_spi::GetX(void *driverHandlerPtr) {
	return 128;
}

int ssd1306_spi::GetY(void *driverHandlerPtr) {
	return 64;
}

void transfer(ssd1306_spi *drv, int len) {
#if !defined(QT_WIDGETS_LIB)
    for( int i = 0; i < len; i++) {
        spi->transfer(drv->buf[i]);
    }
#else
    scene->clear();
    for(int y = 0; y < 8; y++) {
        for(int x = 0; x < 128; x++) {
            for(int z = 0; z < 8; z++) {
                uint8_t state = drv->buf[(y * 128) + x] & (1 << z);
                QPen pen = QPen(state ? QColor(255, 255, 255) : QColor(0, 0, 0));
                int _y = (y * 8) + z;
                scene->addRect(QRectF(x * DISPLAY_ZOOM, _y * DISPLAY_ZOOM, DISPLAY_ZOOM - 1, DISPLAY_ZOOM - 1), pen);
                scene->addRect(QRectF((x * DISPLAY_ZOOM) + 1, (_y * DISPLAY_ZOOM) + 1, DISPLAY_ZOOM - 3, DISPLAY_ZOOM - 3), pen);
            }
        }
    }
#endif
}

Screen *ssd1306_spi::DrvRefresh(void *driverHandlerPtr) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
	/* Set start line */
	//WrCmd(0x40);

#ifndef SSD1306_USE_NO_BUF
#if !defined(QT_WIDGETS_LIB)
    SPI_SSD1306_CS_ASSERT(drv->CsPin);
	drv->WrCmd(0x21);
	drv->WrCmd(0x00);
	drv->WrCmd(0x7F);
	drv->WrCmd(0x22);
	drv->WrCmd(0x00);
	drv->WrCmd(0x07);
	SPI_SSD1306_DATA(drv->DcPin);
#endif
	//drv->spi->transfer(drv->buf, 1024);
	transfer(drv, 1024);
#if !defined(QT_WIDGETS_LIB)
    SPI_SSD1306_CS_DEASSERT(drv->CsPin);
#endif
#endif // SSD1306_USE_BUF
	DrvOn(driverHandlerPtr, true);
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvOn(void *driverHandlerPtr, bool state) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#if !defined(QT_WIDGETS_LIB)
    drv->WrCmd(state ? 0xAF : 0xAE);
	SPI_SSD1306_CS_DEASSERT(drv->CsPin);
#endif
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvSetContrast(void *driverHandlerPtr, byte cont) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#if !defined(QT_WIDGETS_LIB)
    drv->WrCmd(0x81);
	drv->WrCmd(cont);
	SPI_SSD1306_CS_DEASSERT(drv->CsPin);
#endif
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
	DrvDrawPixelClip(driverHandlerPtr, drv->box, x, y, color);
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvDrawPixelClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#ifndef SSD1306_USE_NO_BUF
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
	/* Calculate the byte where the bit will be written. */
	unsigned char *tmp_buff = drv->buf + (((y >> 3) * 128) + x);
	/* Create the mask of the bit that will be edited inside the selected byte. */
	byte mask;
#if __AVR_MEGA__
		mask = pgm_read_byte(&SSD1306_BIT_MASK_TABLE[y & 0x07]);
#else // !__AVR_MEGA__
		mask = 1 << (y & 0x07);
#endif // __AVR_MEGA__
	if (drv->reverseColor ^ (color ? 1 : 0))
		*tmp_buff |= mask;
	else
		*tmp_buff &= ~mask;
#endif // SSD1306_USE_BUF
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
	return DrvDrawRectangleClip(driverHandlerPtr, drv->box, x, y, x_size, y_size, fill, color);
}

Screen *ssd1306_spi::DrvDrawRectangleClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 128;
		box__.y_min = 0;
		box__.y_max = 64;
	}
	int x_end = x + x_size ,y_end = y + y_size;
	if(x >= box__.x_max ||
		y >= box__.y_max ||
			x_end < box__.x_min ||
				y_end < box__.y_min)
		return (Screen *)driverHandlerPtr;
#if !defined(QT_WIDGETS_LIB)
//    register
#endif
    int LineCnt = y;
	if(fill) {
		if(LineCnt < box__.y_min)
			LineCnt = box__.y_min;
		int _x_start = x;
		if(_x_start < box__.x_min)
			_x_start = box__.x_min;
		int _x_end = x_end;
		if(_x_end > box__.x_max)
			_x_end = box__.x_max;
        /*int width_to_refresh = (_x_end - _x_start);
		if((width_to_refresh + _x_start) > box__.x_max)
            width_to_refresh = (box__.x_max - _x_start);*/
#ifndef SSD1306_USE_NO_BUF
		for( ; LineCnt < y_end; LineCnt++) {
			if(LineCnt >= box__.y_max)
				return (Screen *)driverHandlerPtr;
#if !defined(QT_WIDGETS_LIB)
//            register
#endif
            int x = _x_start;
            for( ; x < _x_end ; x++) {
				DrvDrawPixelClip(driverHandlerPtr, &box__, x, LineCnt, color);
			}
		}
#else // !SSD1306_USE_BUF
		for( ; LineCnt < y_end; LineCnt+=8) {
			if(LineCnt >= box__.y_max)
				return (Screen *)driverHandlerPtr;
			/*register*/ int x = _x_start;
			for( ; x < _x_end ; x++) {
				WrCmd(0x21);
				WrCmd(x);
				WrCmd(x);
				WrCmd(0x22);
				WrCmd(LineCnt >> 3);
				WrCmd(LineCnt >> 3);
				SPI_SSD1306_DATA();
				spi->transfer((reverseColor ^ (color ? 1 : 0)) ? 0xFF : 0x00);
				SPI_SSD1306_CS_DEASSERT(drv->CsPin);
			}
		}
#endif // SSD1306_USE_BUF
		return (Screen *)driverHandlerPtr;
	}
#ifndef SSD1306_USE_NO_BUF
	int _x_end = x_end;
	int _x_start = x;
	if(_x_end > box__.x_max) 
		_x_end = box__.x_max;
	if(_x_start < box__.x_min) 
		_x_start = box__.x_min;
	if(y >= box__.y_min) {
		for(LineCnt = _x_start ; LineCnt < _x_end ; LineCnt++) {
			DrvDrawPixelClip(driverHandlerPtr, &box__, LineCnt, y, color);
		}
	}

	if(y_end <= box__.y_max) {
		for(LineCnt = _x_start ; LineCnt < _x_end ; LineCnt++) {
			DrvDrawPixelClip(driverHandlerPtr, &box__, LineCnt, y_end - 1, color);
		}
	}

	int _y_end = y_end;
	if(_y_end > box__.y_max) 
		_y_end = box__.y_max;
	int _y_start = y;
	if(_y_start < box__.y_min) 
		_y_start = box__.y_min;
	if(x >= box__.x_min) {
		for(LineCnt = _y_start ; LineCnt < _y_end ; LineCnt++) {
			DrvDrawPixelClip(driverHandlerPtr, &box__, x, LineCnt, color);
		}
	}

	if(x_end <= box__.x_max) {
		for(LineCnt = _y_start ; LineCnt < _y_end ; LineCnt++) {
			DrvDrawPixelClip(driverHandlerPtr, &box__, (x_end - 1), LineCnt, color);
		}
	}
#endif // SSD1306_USE_BUF
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
	return DrvDrawHLineClip(driverHandlerPtr, drv->box, x1, x2, y, width, color);
}

Screen *ssd1306_spi::DrvDrawHLineClip(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color) {
    //ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#ifndef SSD1306_USE_NO_BUF
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 128;
		box__.y_min = 0;
		box__.y_max = 64;
	}
    int X1_Tmp = x1, X2_Tmp = x2;
	if(X1_Tmp < (int)box__.x_min)
		X1_Tmp = (int)box__.x_min;
    if(X1_Tmp > (int)box__.x_max)
		X1_Tmp = (int)box__.x_max;
	if(X2_Tmp < (int)box__.x_min)
		X2_Tmp = (int)box__.x_min;
    if(X2_Tmp > (int)box__.x_max)
		X2_Tmp = (int)box__.x_max;
	if(y < (int)box__.y_min)
		y = (int)box__.y_min;
    if(y > (int)box__.y_max)
		y = (int)box__.y_max;
    int Half_width1 = width - (width>>1);
    int Half_width2 = width - Half_width1;
    for(;X1_Tmp <= X2_Tmp; X1_Tmp++) {
        int _Y_ = y - Half_width2;
        for(; _Y_ < y + Half_width1; _Y_++)
			DrvDrawPixelClip(driverHandlerPtr, &box__, (int)(X1_Tmp), (int)(_Y_), color);
	}
#endif // SSD1306_USE_BUF
	return (Screen *)driverHandlerPtr;
}

Screen *ssd1306_spi::DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
	return DrvDrawVLineClip(driverHandlerPtr, drv->box, y1, y2, x, width, color);
}

Screen *ssd1306_spi::DrvDrawVLineClip(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color) {
    //ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#ifndef SSD1306_USE_NO_BUF
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 128;
		box__.y_min = 0;
		box__.y_max = 64;
	}
    int Y1_Tmp = y1, Y2_Tmp = y2;
	if(x < (int)box__.x_min)
		x = (int)box__.x_min;
    if(x > (int)box__.x_max)
		x = (int)box__.x_max;
	if(Y1_Tmp < (int)box__.y_min)
		Y1_Tmp = (int)box__.y_min;
    if(Y1_Tmp > (int)box__.y_max)
		Y1_Tmp = (int)box__.y_max;
	if(Y2_Tmp < (int)box__.y_min)
		Y2_Tmp = (int)box__.y_min;
    if(Y2_Tmp > (int)box__.y_max)
		Y2_Tmp = (int)box__.y_max;
    int Half_width1 = width - (width>>1);
    int Half_width2 = width - Half_width1;
    for(;Y1_Tmp <= Y2_Tmp; Y1_Tmp++) {
        int _X_ = x - Half_width2;
        for(; _X_ < x + Half_width1; _X_++)
			DrvDrawPixelClip(driverHandlerPtr, &box__, (int)(_X_), (int)(Y1_Tmp), color);
	}
#endif // SSD1306_USE_BUF
	return (Screen *)driverHandlerPtr;
}

/*#####################################################*/

Screen *ssd1306_spi::DrvClear(void *driverHandlerPtr, int color) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
#ifndef SSD1306_USE_NO_BUF
	memset(drv->buf, (drv->reverseColor ^ (color ? 1 : 0)) ? 0xFF : 0x00, 1024);
#else // !SSD1306_USE_BUF
	DrvDrawRectangle(NULL, 0, 0, 128, 64, 1, reverseColor ^ (color ? 1 : 0));
#endif // SSD1306_USE_BUF
	return (Screen *)driverHandlerPtr;
}

/*#####################################################*/

Screen *ssd1306_spi::DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor) {
	ssd1306_spi *drv = (ssd1306_spi *)driverHandlerPtr;
  return DrvDrawStringClip(driverHandlerPtr, drv->box, string, x, y, drv->terminalMode, drv->wordWrap, foreColor, inkColor);
}

Screen *ssd1306_spi::DrvDrawStringClip(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor) {
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = 128;
		box__.y_min = 0;
		box__.y_max = 64;
	}
	char *pcString = string;
	bool WordWrap = wordWrap;
	//s32 _SelStart = properties->_SelStart;
	//s32 _SelLen = properties->_SelLen;

	char chWidth = 0;
	char chHeight = 0;
	int CharPtr;
	char Tmp = 0;
	int Cursor_X = x;
	int Cursor_Y = y;
	bool ulVisible = true;
	int CharCnt = 0;
	bool ulOpaque = false;
#ifdef __AVR_MEGA__
	chWidth = pgm_read_byte(&fontTable6x8[2]);
	chHeight = pgm_read_byte(&fontTable6x8[3]);
#else // !__AVR_MEGA__
	chWidth = fontTable6x8[2];
	chHeight = fontTable6x8[3];
#endif // __AVR_MEGA__
	do {
		char Char = *pcString;
		if (Char == 0) {
			return (Screen *)driverHandlerPtr;
		}
#ifdef __AVR_MEGA__
		CharPtr = ((Char - pgm_read_byte(&fontTable6x8[4])) * chWidth) + pgm_read_byte(&fontTable6x8[0]);
		if(Char < pgm_read_byte(&fontTable6x8[4]) || Char > pgm_read_byte(&fontTable6x8[5]))
#else // !__AVR_MEGA__
		CharPtr = ((Char - fontTable6x8[4]) * chWidth) + fontTable6x8[0];
		if (Char < fontTable6x8[4] || Char > fontTable6x8[5])
#endif // __AVR_MEGA__
		{
			//chWidth_Tmp = chWidth;
			chWidth = 0;
		} else {
			char Temp;
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
					int XX = 0;
					int YY = 0;
					for (XX = 0; XX < Tmp; XX++) {
#ifdef __AVR_MEGA__
						Temp = pgm_read_byte(&fontTable6x8[XX + CharPtr]);
#else // !__AVR_MEGA__
						Temp = fontTable6x8[XX + CharPtr];
#endif // __AVR_MEGA__
#ifndef SSD1306_USE_NO_BUF
						for (YY = 0; YY < chHeight; YY++) {
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
#else // !SSD1306_USE_BUF
						if(XX + Cursor_X < box__.x_max) {
							SPI_SSD1306_CS_ASSERT(drv->CsPin);
							WrCmd(0x21);
							WrCmd(XX + Cursor_X);
							WrCmd(XX + Cursor_X);
							WrCmd(0x22);
							WrCmd(Cursor_Y >> 3);
							WrCmd(Cursor_Y >> 3);
							SPI_SSD1306_DATA();
							spi->transfer((reverseColor ^ (inkColor ? 1 : 0)) ? Temp : ~Temp);
							SPI_SSD1306_CS_DEASSERT(drv->CsPin);
						}
#endif // SSD1306_USE_BUF
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
