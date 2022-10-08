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


#ifndef __25FLASH_H__
#define __25FLASH_H__

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

class Flash25 {
public:
	Flash25(SPIClass *spi, uint8_t csPin);
	~Flash25();
	void writeStatus(uint8_t status);
	void write(uint32_t addr, uint8_t *buff, uint16_t size);
	void read(uint32_t addr, uint8_t *buff, uint16_t size);
	void erase(uint32_t addr);
private:
	uint8_t readStatus();
	void writeEnable(bool state);
	SPIClass *spi;
	uint8_t csPin;
};

void writeStatus(uint8_t status);
void write(uint32_t addr, uint8_t *buff, uint16_t size);
void read(uint32_t addr, uint8_t *buff, uint16_t size);
void erase(uint32_t addr);

#endif /* 25FLASH_H_ */
