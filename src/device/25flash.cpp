/*
 * 25 series of SPI FLASH memory driver file for arduFPGA designs.
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
#include <string.h>
#include "25flash.h"

Flash25::Flash25(SPIClass *spi, uint8_t csPin) {
	memset(this, 0, sizeof(Flash25));
	this->spi = spi;
	this->csPin = csPin;
#if !defined(QT_WIDGETS_LIB)
	pinMode(csPin, OUTPUT);
	digitalWrite(csPin, 1);
#endif
}

Flash25::~Flash25() {

}

uint8_t Flash25::readStatus() {
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x05);
	uint8_t tmp = spi->transfer(0xFF);
	digitalWrite(csPin, 1);
	return tmp;
#else
    return 0;
#endif
}

void Flash25::writeEnable(bool state) {
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(state ? 0x06 : 0x04);
	digitalWrite(csPin, 1);
#endif
	uint8_t status;
	do {
		status = readStatus();
		status = state ? (~status & 0x02) : (status & 0x02);
	} while (status);
}

void Flash25::writeStatus(uint8_t status) {
	writeEnable(true);
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x01);
	spi->transfer(status);
	digitalWrite(csPin, 1);
	while(readStatus() & 0x01);
#endif
}


void Flash25::write(uint32_t addr, uint8_t *buff, uint16_t size) {
	writeEnable(true);
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x02);
	spi->transfer(addr >> 16);
	spi->transfer(addr >> 8);
	spi->transfer(addr);
	while (size--) {
		spi->transfer(*buff++);
	}
    digitalWrite(csPin, 1);
	while(readStatus() & 0x01);
#endif
}

void Flash25::read(uint32_t addr, uint8_t *buff, uint16_t size) {
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x0B);
	spi->transfer(addr >> 16);
	spi->transfer(addr >> 8);
	spi->transfer(addr);
	spi->transfer(0xFF);
	while (size--) {
		*buff++ = spi->transfer(0xFF);
	}
    digitalWrite(csPin, 1);
	while(readStatus() & 0x01);
#endif
}

void Flash25::eraseSector(uint32_t addr) {
	writeEnable(true);
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x20);
	spi->transfer(addr >> 16);
	spi->transfer(addr >> 8);
	spi->transfer(addr);
	digitalWrite(csPin, 1);
	while(readStatus() & 0x01);
#endif
}

void Flash25::eraseBlock(uint32_t addr) {
	writeEnable(true);
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x52);
	spi->transfer(addr >> 16);
	spi->transfer(addr >> 8);
	spi->transfer(addr);
	digitalWrite(csPin, 1);
	while(readStatus() & 0x01);
#endif
}

void Flash25::eraseChip() {
	writeEnable(true);
#if !defined(QT_WIDGETS_LIB)
	digitalWrite(csPin, 0);
	spi->transfer(0x60);
	digitalWrite(csPin, 1);
	while(readStatus() & 0x01);
#endif
}



