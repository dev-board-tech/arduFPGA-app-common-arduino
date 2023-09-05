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

#if !defined(QT_WIDGETS_LIB)
#include "Arduino.h"
#include <SPI.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/global.h"
#include "font6x8.h"
#if !defined(QT_WIDGETS_LIB)
#include "SPI.h"
#else
#define LOW     0
#define HIGH    1
#endif

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
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(CsPin, LOW);
#endif
}

static inline void SPI_WDPW21_CS_DEASSERT(uint8_t CsPin) {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(CsPin, HIGH);
#endif
}

static inline void SPI_WDPW21_COMMAND(uint8_t DcPin) {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(DcPin, LOW);
#endif
}

static inline void SPI_WDPW21_DATA(uint8_t DcPin) {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(DcPin, HIGH);
#endif
}

wpdW21_spi::wpdW21_spi(SPIClass *Spi, int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy) {
	memset(this, 0, sizeof(this));
#ifndef WDPW21_BUF_SIZE_BYTES
	setBuf(malloc(bufSize));
#endif // WDPW21_BUF_SIZE_BYTES
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
	pinMode(busy, INPUT);
#endif
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
#if !defined(QT_WIDGETS_LIB)
    spi = &SPI;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
#endif
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
#if !defined(QT_WIDGETS_LIB)
    spi = Spi;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
#endif
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
#if !defined(QT_WIDGETS_LIB)
    spi = &SPI;
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
#endif
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
#if !defined(QT_WIDGETS_LIB)
    spi = Spi;
	setBuf(vBuf);
	digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
#endif
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
	setBuf(vBuf);
#if !defined(QT_WIDGETS_LIB)
    spi = &SPI;
    digitalWrite(csPin, HIGH);
	digitalWrite(dcPin, HIGH);
	digitalWrite(rstPin, HIGH);
	pinMode(csPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(rstPin, OUTPUT);
	pinMode(busy, INPUT);
#endif
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
	TriggerUpdatePtr = &wpdW21_spi::TriggerUpdate;
	GetXPtr = &wpdW21_spi::GetX;
	GetYPtr = &wpdW21_spi::GetY;
	DrvRefreshPtr = &wpdW21_spi::DrvRefresh;
	DrvOnPtr = &wpdW21_spi::DrvOn;
	DrvSetContrastPtr = &wpdW21_spi::DrvSetContrast;
	DrvDrawPixelPtr = &wpdW21_spi::DrvDrawPixel;
	DrvDrawPixelClipPtr = &wpdW21_spi::DrvDrawPixelClip;
	DrvDrawRectanglePtr = &wpdW21_spi::DrvDrawRectangle;
	DrvDrawRectangleClipPtr = &wpdW21_spi::DrvDrawRectangleClip;
	DrvDrawHLinePtr = &wpdW21_spi::DrvDrawHLine;
	DrvDrawHLineClipPtr = &wpdW21_spi::DrvDrawHLineClip;
	DrvDrawVLinePtr = &wpdW21_spi::DrvDrawVLine;
	DrvDrawVLineClipPtr = &wpdW21_spi::DrvDrawVLineClip;
	DrvClearPtr = &wpdW21_spi::DrvClear;
	DrvDrawStringPtr = &wpdW21_spi::DrvDrawString;
	DrvDrawStringClipPtr = &wpdW21_spi::DrvDrawStringClip;
	DriverPtr = this;
}

void wpdW21_spi::Idle(void *driverHandlerPtr) {
#if !defined(QT_WIDGETS_LIB)
    wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	switch(drv->state) {
	case drv->PWR_ON_REFRESH:
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
			drv->state = drv->REFRESH_REFRESH;
			delay(2);
		}
		break;
	case drv->REFRESH_REFRESH:
		if(drv->lcd_chkstatus_nonblocking()) {
			drv->WrCmd(0x12);
			drv->state = drv->PWR_OFF_REFRESH;
			delay(2);
		}
		break;
	case drv->PWR_OFF_REFRESH:
		if(drv->lcd_chkstatus_nonblocking()) {
			drv->WrCmd(0x02);
			drv->state = drv->IDLE;
			delay(2);
		}
		break;
	case drv->PWR_ON_REPAINT:
		if(drv->lcd_chkstatus_nonblocking()) {
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
			drv->WrCmd(0x13);	      //Transfer new data
			for(int i = 0; i < 1280; i++) {
				drv->WrData(drv->buf[i]);
				drv->backBuf[i] = drv->buf[i];
			}
			SPI_WDPW21_CS_DEASSERT(drv->CsPin);
			drv->WrCmd(0x04);
			drv->state = drv->REFRESH_REFRESH;
			delay(2);
		}
	}
	if(drv->state == drv->IDLE && drv->repaintCnt != 0) {
		drv->repaintCnt = 0;
		drv->state = drv->PWR_ON_REPAINT;
	}
	if(drv->state == drv->IDLE && drv->refreshCnt != 0) {
		drv->refreshCnt = 0;
		drv->state = drv->PWR_ON_REFRESH;
	}
#endif
}

void wpdW21_spi::TriggerRefresh(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	drv->refreshCnt++;
}

void wpdW21_spi::TriggerUpdate(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	drv->repaintCnt++;
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
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(drv->RstPin, LOW);
	delay(2);
	digitalWrite(drv->RstPin, HIGH);
	delay(10);
#endif
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
#if !defined(QT_WIDGETS_LIB)
    spi->transfer(cmd);
#endif
	SPI_WDPW21_CS_DEASSERT(CsPin);
}

void wpdW21_spi::WrData(byte data) {
	SPI_WDPW21_DATA(DcPin);
	SPI_WDPW21_CS_ASSERT(CsPin);
#if !defined(QT_WIDGETS_LIB)
    spi->transfer(data);
#endif
	SPI_WDPW21_CS_DEASSERT(CsPin);
}



int wpdW21_spi::GetX(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	if(drv->ScreenOrientation == screenOrientation::PORTRAIT || drv->ScreenOrientation == screenOrientation::PORTRAIT_FLIPPED)
		return 80;
	else
		return 128;
}

int wpdW21_spi::GetY(void *driverHandlerPtr) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	if(drv->ScreenOrientation == screenOrientation::PORTRAIT || drv->ScreenOrientation == screenOrientation::PORTRAIT_FLIPPED)
		return 128;
	else
		return 80;
}

bool wpdW21_spi::lcd_chkstatus_nonblocking(void) {
	unsigned char busy;
	WrCmd(0x71);
#if !defined(QT_WIDGETS_LIB)
    busy = digitalRead(Busy);
	busy =!(busy & 0x01);
#endif
	return busy ? false : true;
}

void wpdW21_spi::lcd_chkstatus(void) {
#if !defined(QT_WIDGETS_LIB)
    unsigned char busy;
	do {
		WrCmd(0x71);
		busy = digitalRead(Busy);
		busy =!(busy & 0x01);
	} while(busy);
	delay(2);
#endif
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
	DrvDrawPixelClip(driverHandlerPtr, drv->box, x, y, color);
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawPixelClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int color) {
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
	if(drv->ScreenOrientation == screenOrientation::PORTRAIT || drv->ScreenOrientation == screenOrientation::LANDSCAPE) {
		if(drv->ScreenOrientation == screenOrientation::LANDSCAPE) {
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
	} else if(drv->ScreenOrientation == screenOrientation::PORTRAIT_FLIPPED || drv->ScreenOrientation == screenOrientation::LANDSCAPE_FLIPPED) {
		if(drv->ScreenOrientation == screenOrientation::PORTRAIT_FLIPPED) {
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
	return DrvDrawRectangleClip(driverHandlerPtr, drv->box, x, y, x_size, y_size, fill, color);
}

Screen *wpdW21_spi::DrvDrawRectangleClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color) {
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
#if !defined(QT_WIDGETS_LIB)
    //register
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
		int width_to_refresh = (_x_end - _x_start);
		if((width_to_refresh + _x_start) > box__.x_max)
			width_to_refresh = (box__.x_max - _x_start);
		for( ; LineCnt < y_end; LineCnt++) {
			if(LineCnt >= box__.y_max)
				return (Screen *)driverHandlerPtr;
#if !defined(QT_WIDGETS_LIB)
            //register
#endif
            int x = _x_start;
			for( ; x < _x_end ; x++) {
				DrvDrawPixelClip(driverHandlerPtr, &box__, x, LineCnt, color);
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
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawHLineClip(driverHandlerPtr, drv->box, x1, x2, y, width, color);
}

Screen *wpdW21_spi::DrvDrawHLineClip(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color) {
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
			DrvDrawPixelClip(driverHandlerPtr, &box__, (int)(X1_Tmp), (int)(_Y_), color);
	}
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawVLineClip(driverHandlerPtr, drv->box, y1, y2, x, width, color);
}

Screen *wpdW21_spi::DrvDrawVLineClip(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color) {
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
			DrvDrawPixelClip(driverHandlerPtr, &box__, (int)(_X_), (int)(Y1_Tmp), color);
	}
	return (Screen *)driverHandlerPtr;
}

Screen *wpdW21_spi::DrvClear(void *driverHandlerPtr, int color) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	memset(drv->buf, (drv->reverseColor ^ (color ? 0 : 1)) ? 0xFF : 0x00, 1280);
	return (Screen *)driverHandlerPtr;
}

/*#####################################################*/
Screen *wpdW21_spi::DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor) {
	wpdW21_spi *drv = (wpdW21_spi *)driverHandlerPtr;
	return DrvDrawStringClip(driverHandlerPtr, drv->box, string, x, y, drv->terminalMode, drv->wordWrap, foreColor, inkColor);
}

Screen *wpdW21_spi::DrvDrawStringClip(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor) {
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
						for (YY = 0; YY < chHeight; YY++) {
							if (Temp & 0x1)
							{
								DrvDrawPixelClip(driverHandlerPtr, &box__,
									XX + Cursor_X, YY + Cursor_Y, inkColor);
							} else {
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

