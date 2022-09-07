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

#include "wpdw21_spi.h"

#include "Arduino.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <SPI.h>
#include "../include/global.h"
#include "../dev/screen.h"

#if __AVR_MEGA__
#include <avr/pgmspace.h>
#endif

#if __AVR_MEGA__
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
#endif // SSD1306_USE_BUF

#if __AVR_MEGA__
static const unsigned char lut_w1[] PROGMEM = {
#else
static const unsigned char lut_w1[] = {
#endif
0x60	,0x5A	,0x5A	,0x00	,0x00	,0x01	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
};
#if __AVR_MEGA__
static const unsigned char lut_b1[] PROGMEM = {
#else
static const unsigned char lut_b1[] = {
#endif
0x90	,0x5A	,0x5A	,0x00	,0x00	,0x01	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
};
#if __AVR_MEGA__
static const unsigned char lut_w[] PROGMEM = {
#else
static const unsigned char lut_w[] = {
#endif
0x60	,0x01	,0x01	,0x00	,0x00	,0x01	,
0x80	,0x0f	,0x00	,0x00	,0x00	,0x01	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
};
#if __AVR_MEGA__
static const unsigned char lut_b[] PROGMEM = {
#else
static const unsigned char lut_b[] = {
#endif
0x90	,0x01	,0x01	,0x00	,0x00	,0x01	,
0x40	,0x0f	,0x00	,0x00	,0x00	,0x01	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
};

static inline void SPI_WDPW21_CS_ASSERT(uint8_t CsPin) {
	digitalWrite(CsPin, LOW);
}

static inline void SPI_WDPW21_CS_DEASSERT(uint8_t CsPin) {
	digitalWrite(CsPin, HIGH);
}

static inline void SPI_WDPW21_COMMAND(uint8_t DcPin) {
	digitalWrite(DcPin, LOW);
}

static inline void SPI_WDPW21_DATA(uint8_t DcPin) {
	digitalWrite(DcPin, HIGH);
}

wpdW21_spi::wpdW21_spi(SPIClass *Spi, int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
#ifndef WDPW21_BUF_SIZE_BYTES
	setBuf(malloc(bufSize));
#endif // WDPW21_BUF_SIZE_BYTES
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	spi = Spi;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
	setDefault();
	deriverInit();
	drvClear(false);
}

wpdW21_spi::wpdW21_spi(int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
#ifndef WDPW21_BUF_SIZE_BYTES
	setBuf(malloc(bufSize));
#endif // WDPW21_BUF_SIZE_BYTES
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	Busy = busy;
	spi = &SPI;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
	setDefault();
	deriverInit();
	drvClear(false);
}

wpdW21_spi::wpdW21_spi(SPIClass *Spi, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
#ifndef WDPW21_BUF_SIZE_BYTES
	setBuf(malloc(1280));
#endif // WDPW21_BUF_SIZE_BYTES
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	Busy = busy;
	spi = Spi;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
	setDefault();
	deriverInit();
	drvClear(false);
}


wpdW21_spi::wpdW21_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
#ifndef WDPW21_BUF_SIZE_BYTES
	setBuf(malloc(1280));
#endif // WDPW21_BUF_SIZE_BYTES
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	Busy = busy;
	spi = &SPI;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
	setDefault();
	deriverInit();
	drvClear(false);
}


wpdW21_spi::wpdW21_spi(SPIClass *Spi, void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	Busy = busy;
	spi = Spi;
	setBuf(vBuf);
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
	setDefault();
	deriverInit();
	drvClear(false);
}
wpdW21_spi::wpdW21_spi(void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
	CsPin = csPin;
	DcPin = dcPin;
	RstPin = rstPin;
	Busy = busy;
	spi = &SPI;
	setBuf(vBuf);
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
	setDefault();
	deriverInit();
	drvClear(false);
}

wpdW21_spi::~wpdW21_spi() {
	drvClear(false);
	drvRefresh();
#ifndef WDPW21_BUF_SIZE_BYTES
	if(getBuf())
		free(getBuf());
#endif // WDPW21_BUF_SIZE_BYTES
	remove();
}


void wpdW21_spi::deriverInit() {
	InitPtr = &wpdW21_spi::Init;
	IdlePtr = &wpdW21_spi::Idle;
	TriggerRefreshPtr = &wpdW21_spi::TriggerRefresh;
	GetXPtr = &wpdW21_spi::GetX;
	GetYPtr = &wpdW21_spi::GetY;
	DrvRefreshPtr = &wpdW21_spi::DrvRefresh;
	DrvOnPtr = &wpdW21_spi::DrvOn;
	DrvSetContrastPtr = &wpdW21_spi::DrvSetContrast;
	DrvDrawPixelPtr = &wpdW21_spi::DrvDrawPixel;
	DrvDrawPixelBoxPtr = &wpdW21_spi::DrvDrawPixelBox;
	DrvDrawRectanglePtr = &wpdW21_spi::DrvDrawRectangle;
	DrvDrawRectangleBoxPtr = &wpdW21_spi::DrvDrawRectangleBox;
	DrvDrawHLinePtr = &wpdW21_spi::DrvDrawHLine;
	DrvDrawHLineBoxPtr = &wpdW21_spi::DrvDrawHLineBox;
	DrvDrawVLinePtr = &wpdW21_spi::DrvDrawVLine;
	DrvDrawVLineBoxPtr = &wpdW21_spi::DrvDrawVLineBox;
	DrvClearPtr = &wpdW21_spi::DrvClear;
	DrvDrawStringPtr = &wpdW21_spi::DrvDrawString;
	DrvDrawStringBoxPtr = &wpdW21_spi::DrvDrawStringBox;
	DriverPtr = this;
}

void wpdW21_spi::Idle(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	switch(drv->state) {
	case drv->PWR_ON:
		if(drv->lcd_chkstatus_nonblocking()) {
			drv->WrCmd(0x00);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0b01111111);  //from outside

			drv->WrCmd(0x01);  	//power setting
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData (0x03);
			drv->WrData (0x00);
			drv->WrData (0x2b);
			drv->WrData (0x2b);

			drv->WrCmd(0x06);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x3f);

			drv->WrCmd(0x2A);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x00);
			drv->WrData(0x00);

			drv->WrCmd(0x30);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x13);

			drv->WrCmd(0x50);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x57);

			drv->WrCmd(0x60);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x22);

			drv->WrCmd(0x61);	//resolution setting
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x50);  //source 128
			drv->WrData(0x80);

			drv->WrCmd(0x82);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x12);  //-1v

			drv->WrCmd(0xe3);
			SPI_WDPW21_DATA(drv->DcPin);
			drv->WrData(0x33);

			drv->full_lut_bw();	//Load waveform file

			for(uint16_t i=0;i<1280;i++) {
				drv->backBuf[i] = 0xFF;
			}

			drv->WrCmd(0x92);			//resolution setting
			drv->WrCmd(0x10);
			SPI_WDPW21_DATA(drv->DcPin);
			for(uint16_t i=0;i<1280;i++) {
				drv->WrData(0xFF);
			}
			drv->WrCmd(0x13);	      //Transfer new data
			//SPI_WDPW21_DATA(drv->DcPin);
			//drv->spi->transfer(drv->buf, 1280);
			for(int i = 0; i < 1280; i++) {
				drv->WrData(drv->buf[i]);
				drv->backBuf[i] = drv->buf[i];
			}
			SPI_WDPW21_CS_DEASSERT(drv->CsPin);
			drv->WrCmd(0x04);
			drv->state = drv->REFRESH;
			delay(2);
		}
		break;
	case drv->REFRESH:
		if(drv->lcd_chkstatus_nonblocking()) {
			drv->WrCmd(0x12);
			drv->state = drv->PWR_OFF;
			delay(2);
		}
		break;
	case drv->PWR_OFF:
		if(drv->lcd_chkstatus_nonblocking()) {
			drv->WrCmd(0x02);
			drv->state = drv->IDLE;
			delay(2);
		}
		break;
	}
}

void wpdW21_spi::TriggerRefresh(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	if(drv->state == drv->IDLE) {
		drv->state = drv->PWR_ON;
	}
}

void wpdW21_spi::full_lut_bw() {
	unsigned int count;
	WrCmd(0x23);
	for(count=0;count<42;count++)
		WrData(lut_w1[count]);
	WrCmd(0x24);
	for(count=0;count<42;count++)
		WrData(lut_b1[count]);
}

void wpdW21_spi::part_lut_bw() {
	unsigned int count;
	WrCmd(0x23);
	for(count=0;count<42;count++)
		WrData(lut_w[count]);

	WrCmd(0x24);
	for(count=0;count<42;count++)
		WrData(lut_b[count]);
}


Screen *wpdW21_spi::Init(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	drv->deriverInit();
	digitalWrite(drv->RstPin, LOW);
	delay(2);
	digitalWrite(drv->RstPin, HIGH);
	delay(10);
	drv->WrCmd(0xD2);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x3F);

	drv->WrCmd(0x00);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0b01111111);  //from outside

	drv->WrCmd(0x01);  	//power setting
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData (0x03);
	drv->WrData (0x00);
	drv->WrData (0x2b);
	drv->WrData (0x2b);

	drv->WrCmd(0x06);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x3f);

	drv->WrCmd(0x2A);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x00);
	drv->WrData(0x00);

	drv->WrCmd(0x30);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x13);

	drv->WrCmd(0x50);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x57);

	drv->WrCmd(0x60);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x22);

	drv->WrCmd(0x61);	//resolution setting
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x50);  //source 128
	drv->WrData(0x80);

	drv->WrCmd(0x82);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x12);  //-1v

	drv->WrCmd(0xe3);
	SPI_WDPW21_DATA(drv->DcPin);
	drv->WrData(0x33);

	drv->full_lut_bw();	//Load waveform file
	for(uint16_t i=0;i<1280;i++) {
		drv->backBuf[i] = 0xFF;
	}
	drv->initialized = false;
	drv->drvRefresh();
	drv->initialized = true;
	return drv;
}

void wpdW21_spi::WrCmd(byte cmd) {
	SPI_WDPW21_COMMAND(DcPin);
	SPI_WDPW21_CS_ASSERT(CsPin);
	spi->transfer(cmd);
	SPI_WDPW21_CS_DEASSERT(CsPin);
}

void wpdW21_spi::WrData(byte data) {
	SPI_WDPW21_DATA(DcPin);
	SPI_WDPW21_CS_ASSERT(CsPin);
	spi->transfer(data);
	SPI_WDPW21_CS_DEASSERT(CsPin);
}



int wpdW21_spi::GetX(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	if(drv->screenRotate == screenRotation::PORTRAIT || drv->screenRotate == screenRotation::PORTRAIT_FLIPPED)
		return 80;
	else
		return 128;
}

int wpdW21_spi::GetY(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	if(drv->screenRotate == screenRotation::PORTRAIT || drv->screenRotate == screenRotation::PORTRAIT_FLIPPED)
		return 128;
	else
		return 80;
}

bool wpdW21_spi::lcd_chkstatus_nonblocking(void) {
	unsigned char busy;
	WrCmd(0x71);
	busy = digitalRead(Busy);
	busy =!(busy & 0x01);
	return busy ? false : true;
}

void wpdW21_spi::lcd_chkstatus(void) {
	unsigned char busy;
	do {
		WrCmd(0x71);
		busy = digitalRead(Busy);
		busy =!(busy & 0x01);
	} while(busy);
	delay(2);
}

void wpdW21_spi::upData() {
	WrCmd(0x04);     		//power on
	lcd_chkstatus();
	WrCmd(0x12);
	lcd_chkstatus();
	WrCmd(0x02);
	lcd_chkstatus();
}

Screen *wpdW21_spi::DrvRefresh(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	SPI_WDPW21_CS_ASSERT(drv->CsPin);

	if(drv->initialized) {
		drv->WrCmd(0xD2); //Factory setting parameters
		drv->WrData(0x3F);

		drv->WrCmd(0x00);
		drv->WrData (0x6F);  //from outside

		drv->WrCmd(0x01);  			//power setting
		drv->WrData (0x03);
		drv->WrData (0x00);
		drv->WrData (0x2b);
		drv->WrData (0x2b);

		drv->WrCmd(0x06);
		drv->WrData(0x3f);

		drv->WrCmd(0x2A);
		drv->WrData(0x00);
		drv->WrData(0x00);

		drv->WrCmd(0x30); //PLL
		drv->WrData(0x05);

		drv->WrCmd(0x50);	//VCOM AND DATA INTERVAL SETTING
		drv->WrData(0xF2);

		drv->WrCmd(0x60);
		drv->WrData(0x22);

		drv->WrCmd(0x82);		//VCOM
		drv->WrData(0x0);//-0.1v

		drv->WrCmd(0xe3);
		drv->WrData(0x33);

		drv->part_lut_bw();

		drv->WrCmd(0x91);			//resolution setting
		drv->WrCmd(0x90);			//resolution setting
		drv->WrData (0);
		drv->WrData (79);
		drv->WrData (0);
		drv->WrData (127);
		drv->WrData (0x00);
		drv->WrCmd(0x10);
		SPI_WDPW21_DATA(drv->DcPin);
		for(uint16_t i=0;i<1280;i++) {
			drv->WrData(drv->backBuf[i]);
		}
	} else {
		drv->WrCmd(0x92);			//resolution setting
		drv->WrCmd(0x10);
		SPI_WDPW21_DATA(drv->DcPin);
		for(uint16_t i=0;i<1280;i++) {
			drv->WrData(0xFF);
		}
	}
	drv->WrCmd(0x13);	      //Transfer new data
	for(int i = 0; i < 1280; i++) {
		drv->WrData(drv->buf[i]);
		drv->backBuf[i] = drv->buf[i];
	}
	SPI_WDPW21_CS_DEASSERT(drv->CsPin);
	drv->upData();
	return drv;
}

Screen *wpdW21_spi::DrvOn(void *driverHandlerPtr, bool state) {
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvSetContrast(void *driverHandlerPtr, byte cont) {
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	DrvDrawPixelBox(driverHandlerPtr, drv->box, x, y, color);
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawPixelBox(void *driverHandlerPtr, struct box_s *box, int x, int y, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	/* Check if outside the display */
	if(x < 0 || y < 0 || y > GetY(driverHandlerPtr))
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
	unsigned char *tmp_buff;
	byte mask;
	if(drv->screenRotate == screenRotation::PORTRAIT || drv->screenRotate == screenRotation::LANDSCAPE) {
		if(drv->screenRotate == screenRotation::LANDSCAPE) {
			int t = 79 - y;
			y = x;
			x = t;
		}
		tmp_buff = drv->buf + (((y >> 3) * 80) + (x >> 3) + ((y & 0x07) * (80 >> 3)));
#if __AVR_MEGA__
		mask = pgm_read_byte(&SSD1306_BIT_MASK_TABLE[x & 0x07]);
#else // !__AVR_MEGA__
		mask = 0x80 >> (x & 0x07);
#endif // __AVR_MEGA__
	} else if(drv->screenRotate == screenRotation::PORTRAIT_FLIPPED || drv->screenRotate == screenRotation::LANDSCAPE_FLIPPED) {
		if(drv->screenRotate == screenRotation::PORTRAIT_FLIPPED) {
			y = GetY(driverHandlerPtr) - y;
			x = GetX(driverHandlerPtr) - x;
		} else {
			int t = y;
			y = 127 - x;
			x = t;
		}
		tmp_buff = drv->buf + (((y >> 3) * 80) + (x >> 3) + ((y & 0x07) * (80 >> 3)));
#if __AVR_MEGA__
		mask = pgm_read_byte(&SSD1306_BIT_MASK_TABLE[x & 0x07]);
#else // !__AVR_MEGA__
		mask = 0x80 >> (x & 0x07);
#endif // __AVR_MEGA__
	}
	/* Create the mask of the bit that will be edited inside the selected byte. */
	if (drv->reverseColor ^ (color ? 0 : 1))
		*tmp_buff |= mask;
	else
		*tmp_buff &= ~mask;
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawRectangleBox(driverHandlerPtr, drv->box, x, y, x_size, y_size, fill, color);
}

Screen *wpdW21_spi::DrvDrawRectangleBox(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
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
	register int LineCnt = y;
	if(fill) {
		if(LineCnt < box__.y_min)
			LineCnt = box__.y_min;
		int _x_start = x;
		if(_x_start < box__.x_min)
			_x_start = box__.x_min;
		int _x_end = x_end;
		if(_x_end > box__.x_max)
			_x_end = box__.x_max;
		int width_to_refresh = (_x_end - _x_start);
		if((width_to_refresh + _x_start) > box__.x_max)
			width_to_refresh = (box__.x_max - _x_start);
		for( ; LineCnt < y_end; LineCnt++) {
			if(LineCnt >= box__.y_max)
				return (Screen *)driverHandlerPtr;
			register int x = _x_start;
			for( ; x < _x_end ; x++) {
				DrvDrawPixelBox(driverHandlerPtr, &box__, x, LineCnt, color);
			}
		}
		return (Screen *)driverHandlerPtr;
	}
	int _x_end = x_end;
	int _x_start = x;
	if(_x_end > box__.x_max)
		_x_end = box__.x_max;
	if(_x_start < box__.x_min)
		_x_start = box__.x_min;
	if(y >= box__.y_min) {
		for(LineCnt = _x_start ; LineCnt < _x_end ; LineCnt++) {
			DrvDrawPixelBox(driverHandlerPtr, &box__, LineCnt, y, color);
		}
	}

	if(y_end <= box__.y_max) {
		for(LineCnt = _x_start ; LineCnt < _x_end ; LineCnt++) {
			DrvDrawPixelBox(driverHandlerPtr, &box__, LineCnt, y_end - 1, color);
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
			DrvDrawPixelBox(driverHandlerPtr, &box__, x, LineCnt, color);
		}
	}

	if(x_end <= box__.x_max) {
		for(LineCnt = _y_start ; LineCnt < _y_end ; LineCnt++) {
			DrvDrawPixelBox(driverHandlerPtr, &box__, (x_end - 1), LineCnt, color);
		}
	}
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawHLineBox(driverHandlerPtr, drv->box, x1, x2, y, width, color);
}

Screen *wpdW21_spi::DrvDrawHLineBox(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = drv->GetXPtr(driverHandlerPtr);
		box__.y_min = 0;
		box__.y_max = drv->GetYPtr(driverHandlerPtr);
	}
	int X1_Tmp = x1, X2_Tmp = x1 + x2;
	if(X1_Tmp < (int)box__.x_min)
		X1_Tmp = (int)box__.x_min;
	if(X1_Tmp >= (int)box__.x_max)
		X1_Tmp = (int)box__.x_max;
	if(X2_Tmp < (int)box__.x_min)
		X2_Tmp = (int)box__.x_min;
	if(X2_Tmp >= (int)box__.x_max)
		X2_Tmp = (int)box__.x_max;
	if(y < (int)box__.y_min)
		y = (int)box__.y_min;
	if(y >= (int)box__.y_max)
		y = (int)box__.y_max;
	int Half_width1 = (width>>1);
	int Half_width2 = width-Half_width1;
	for(;X1_Tmp < X2_Tmp; X1_Tmp++) {
		int _Y_ = y - Half_width1;
		for(; _Y_ < y + Half_width2; _Y_++)
			DrvDrawPixelBox(driverHandlerPtr, &box__, (int)(X1_Tmp), (int)(_Y_), color);
	}
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawVLineBox(driverHandlerPtr, drv->box, y1, y2, x, width, color);
}

Screen *wpdW21_spi::DrvDrawVLineBox(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = drv->GetXPtr(driverHandlerPtr);
		box__.y_min = 0;
		box__.y_max = drv->GetYPtr(driverHandlerPtr);
	}
	int Y1_Tmp = y1, Y2_Tmp = y1 + y2;
	if(x < (int)box__.x_min)
		x = (int)box__.x_min;
	if(x >= (int)box__.x_max)
		x = (int)box__.x_max;
	if(Y1_Tmp < (int)box__.y_min)
		Y1_Tmp = (int)box__.y_min;
	if(Y1_Tmp >= (int)box__.y_max)
		Y1_Tmp = (int)box__.y_max;
	if(Y2_Tmp < (int)box__.y_min)
		Y2_Tmp = (int)box__.y_min;
	if(Y2_Tmp >= (int)box__.y_max)
		Y2_Tmp = (int)box__.y_max;
	int Half_width1 = (width>>1);
	int Half_width2 = width-Half_width1;
	for(;Y1_Tmp < Y2_Tmp; Y1_Tmp++) {
		int _X_ = x - Half_width1;
		for(; _X_ < x + Half_width2; _X_++)
			DrvDrawPixelBox(driverHandlerPtr, &box__, (int)(_X_), (int)(Y1_Tmp), color);
	}
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvClear(void *driverHandlerPtr, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	memset(drv->buf, (drv->reverseColor ^ (color ? 0 : 1)) ? 0xFF : 0x00, 1280);
	return (Screen *)driverHandlerPtr;
}

/*#####################################################*/
#ifdef __AVR_MEGA__
const byte CharTable6x8[] PROGMEM =
#else
const byte CharTable6x8[] =
#endif
{
	6                          ,0          ,6          ,8          ,32            ,128,
	/*  OffsetOfBeginingCharTable  ,0=Y-X|1=X-X,X-Dimension,Y-Dimension,BeginAsciiChar,EndAsciiChar*/
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x5F,0x00,0x00,0x00,0x00,0x00,//   !		32,33
	0x07,0x00,0x07,0x00,0x00,0x00,
	0x14,0x7F,0x14,0x7F,0x14,0x00,// " #		34,35
	0x24,0x2A,0x7F,0x2A,0x12,0x00,
	0x23,0x13,0x08,0x64,0x62,0x00,// 0x %		36,37
	0x36,0x49,0x55,0x22,0x50,0x00,
	0x05,0x03,0x00,0x00,0x00,0x00,// & '		38,39
	0x1C,0x22,0x41,0x00,0x00,0x00,
	0x41,0x22,0x1C,0x00,0x00,0x00,// ( )		40,41
	0x08,0x2A,0x1C,0x2A,0x08,0x00,
	0x08,0x08,0x3E,0x08,0x08,0x00,// * +		42,43
	0x50,0x30,0x00,0x00,0x00,0x00,
	0x08,0x08,0x08,0x00,0x00,0x00,// , -		44,45
	0x30,0x30,0x00,0x00,0x00,0x00,
	0x20,0x10,0x08,0x04,0x02,0x00,// . /		46,47
	0x3E,0x51,0x49,0x45,0x3E,0x00,
	0x42,0x7F,0x40,0x00,0x00,0x00,// 0 1		48,49
	0x42,0x61,0x51,0x49,0x46,0x00,
	0x21,0x41,0x45,0x4B,0x31,0x00,// 2 3		50,51
	0x18,0x14,0x12,0x7F,0x10,0x00,
	0x27,0x45,0x45,0x45,0x39,0x00,// 4 5		52,53
	0x3C,0x4A,0x49,0x49,0x30,0x00,
	0x01,0x71,0x09,0x05,0x03,0x00,// 6 7		54,55
	0x36,0x49,0x49,0x49,0x36,0x00,
	0x06,0x49,0x49,0x29,0x1E,0x00,// 8 9		56,57
	0x36,0x00,0x00,0x00,0x00,0x00,
	0x56,0x36,0x00,0x00,0x00,0x00,// : ;		58,59
	0x08,0x14,0x22,0x41,0x00,0x00,
	0x14,0x14,0x14,0x00,0x00,0x00,// < =		60,61
	0x41,0x22,0x14,0x08,0x00,0x00,
	0x02,0x01,0x51,0x09,0x06,0x00,// > ?		62,63
	0x32,0x49,0x79,0x41,0x3E,0x00,
	0x7E,0x11,0x11,0x7E,0x00,0x00,// @ A		64,65
	0x7F,0x49,0x49,0x36,0x00,0x00,
	0x3E,0x41,0x41,0x22,0x00,0x00,// B C		66,67
	0x7F,0x41,0x22,0x1C,0x00,0x00,
	0x7F,0x49,0x49,0x41,0x00,0x00,// D E		68,69
	0x7F,0x09,0x09,0x01,0x00,0x00,
	0x3E,0x41,0x51,0x32,0x00,0x00,// F G		70,71
	0x7F,0x08,0x08,0x7F,0x00,0x00,
	0x41,0x7F,0x41,0x00,0x00,0x00,// H I		72,73
	0x20,0x40,0x41,0x3F,0x01,0x00,
	0x7F,0x08,0x14,0x22,0x41,0x00,// J K		74,75
	0x7F,0x40,0x40,0x00,0x00,0x00,
	0x7F,0x02,0x04,0x02,0x7F,0x00,// L M		76,77
	0x7F,0x04,0x08,0x10,0x7F,0x00,
	0x3E,0x41,0x41,0x3E,0x00,0x00,// N O		78,79
	0x7F,0x09,0x09,0x06,0x00,0x00,
	0x3E,0x41,0x51,0x21,0x5E,0x00,// P Q		80,81
	0x7F,0x19,0x29,0x46,0x00,0x00,
	0x46,0x49,0x49,0x31,0x00,0x00,// R S		82,83
	0x01,0x7F,0x01,0x00,0x00,0x00,
	0x3F,0x40,0x40,0x3F,0x00,0x00,// T U		84,85
	0x1F,0x20,0x40,0x20,0x1F,0x00,
	0x7F,0x20,0x18,0x20,0x7F,0x00,// V W		86,87
	0x63,0x14,0x08,0x14,0x63,0x00,
	0x03,0x04,0x78,0x04,0x03,0x00,// X Y		88,89
	0x61,0x51,0x49,0x45,0x43,0x00,
	0x7F,0x41,0x41,0x00,0x00,0x00,// Z [		90,91
	0x02,0x04,0x08,0x10,0x20,0x00,
	0x41,0x41,0x7F,0x00,0x00,0x00,// \ ]		92,93
	0x04,0x02,0x01,0x02,0x04,0x00,
	0x40,0x40,0x40,0x00,0x00,0x00,// ^ _		94,95
	0x01,0x02,0x04,0x00,0x00,0x00,
	0x20,0x54,0x54,0x78,0x00,0x00,// ` a		96,97
	0x7F,0x48,0x44,0x38,0x00,0x00,
	0x38,0x44,0x44,0x00,0x00,0x00,// b c		98,99
	0x38,0x44,0x48,0x7F,0x00,0x00,
	0x38,0x54,0x54,0x18,0x00,0x00,// d e		100,101
	0x08,0x7E,0x09,0x01,0x00,0x00,
	0x08,0x14,0x54,0x3C,0x00,0x00,// f g		102,103
	0x7F,0x08,0x04,0x78,0x00,0x00,
	0x44,0x7D,0x40,0x00,0x00,0x00,// h i		104,105
	0x20,0x40,0x44,0x3D,0x00,0x00,
	0x7F,0x10,0x28,0x44,0x00,0x00,// j k		106,107
	0x41,0x7F,0x40,0x00,0x00,0x00,
	0x7C,0x04,0x18,0x04,0x78,0x00,// l m		108,109
	0x7C,0x08,0x04,0x78,0x00,0x00,
	0x38,0x44,0x44,0x38,0x00,0x00,// n o		110,111
	0x7C,0x14,0x14,0x08,0x00,0x00,
	0x08,0x14,0x18,0x7C,0x00,0x00,// p q		112,113
	0x7C,0x08,0x04,0x08,0x00,0x00,
	0x48,0x54,0x54,0x20,0x00,0x00,// r s		114,115
	0x04,0x3F,0x44,0x40,0x00,0x00,
	0x3C,0x40,0x20,0x7C,0x00,0x00,// t u		116,117
	0x1C,0x20,0x40,0x20,0x1C,0x00,
	0x3C,0x40,0x30,0x40,0x3C,0x00,// v w		118,119
	0x44,0x28,0x10,0x28,0x44,0x00,
	0x0C,0x50,0x50,0x3C,0x00,0x00,// x y		120,121
	0x44,0x64,0x54,0x4C,0x44,0x00,
	0x08,0x36,0x41,0x00,0x00,0x00,// z {		122,123
	0x7F,0x00,0x00,0x00,0x00,0x00,
	0x41,0x36,0x08,0x00,0x00,0x00,// | }		124,125
	0x08,0x2A,0x1C,0x08,0x00,0x00,
	0x08,0x1C,0x2A,0x08,0x00,0x00,// -> <-		126,127
	0x14,0x36,0x77,0x36,0x14,0x00 };//			128

Screen *wpdW21_spi::DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawStringBox(driverHandlerPtr, drv->box, string, x, y, drv->terminalMode, drv->wordWrap, foreColor, inkColor);
}

Screen *wpdW21_spi::DrvDrawStringBox(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	box_s box__;
	if(box) {
		box__.x_min = box->x_min;
		box__.x_max = box->x_max;
		box__.y_min = box->y_min;
		box__.y_max = box->y_max;
	} else {
		box__.x_min = 0;
		box__.x_max = drv->GetXPtr(driverHandlerPtr);
		box__.y_min = 0;
		box__.y_max = drv->GetYPtr(driverHandlerPtr);
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
	chWidth = pgm_read_byte(&CharTable6x8[2]);
	chHeight = pgm_read_byte(&CharTable6x8[3]);
#else // !__AVR_MEGA__
	chWidth = CharTable6x8[2];
	chHeight = CharTable6x8[3];
#endif // __AVR_MEGA__
	do {
		char Char = *pcString;
		if (Char == 0) {
			return (Screen *)driverHandlerPtr;
		}
#ifdef __AVR_MEGA__
		CharPtr = ((Char - pgm_read_byte(&CharTable6x8[4])) * chWidth) + pgm_read_byte(&CharTable6x8[0]);
		if(Char < pgm_read_byte(&CharTable6x8[4]) || Char > pgm_read_byte(&CharTable6x8[5]))
#else // !__AVR_MEGA__
		CharPtr = ((Char - CharTable6x8[4]) * chWidth) + CharTable6x8[0];
		if (Char < CharTable6x8[4] || Char > CharTable6x8[5])
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
					Temp = pgm_read_byte(&CharTable6x8[Tmp + CharPtr]);
#else
					Temp = CharTable6x8[Tmp + CharPtr];
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
						Temp = pgm_read_byte(&CharTable6x8[XX + CharPtr]);
#else // !__AVR_MEGA__
						Temp = CharTable6x8[XX + CharPtr];
#endif // __AVR_MEGA__
						for (YY = 0; YY < chHeight; YY++) {
							if (Temp & 0x1)
							{
								DrvDrawPixelBox(driverHandlerPtr, &box__,
									XX + Cursor_X, YY + Cursor_Y, inkColor);
							} else {
								if (ulOpaque)
									DrvDrawPixelBox(driverHandlerPtr, &box__,
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

