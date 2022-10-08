/*
 * font6x8.h
 *
 *  Created on: Sep 8, 2022
 *      Author: cry
 */

#ifndef LIBRARIES_ARDUFPGA_COMMON_DEV_SRC_DEVICE_FONT6X8_H_
#define LIBRARIES_ARDUFPGA_COMMON_DEV_SRC_DEVICE_FONT6X8_H_

#include "Arduino.h"


#ifdef __AVR_MEGA__
#include <avr/pgmspace.h>
extern const byte fontTable6x8[] PROGMEM;
#else
extern const byte fontTable6x8[];
#endif

#endif /* LIBRARIES_ARDUFPGA_COMMON_DEV_SRC_DEVICE_FONT6X8_H_ */
