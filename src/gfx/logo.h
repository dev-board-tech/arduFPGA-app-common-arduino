/*
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


#ifndef __LOGO_H__
#define __LOGO_H__

#include <stdbool.h>
#include <stdint.h>

#include "include/global.h"

class logoBW {
    uint8_t *logo;
    bool negative;
    bool transparent;
    bool transparentColor;

    bool getPixelFromBitmap(int16_t x ,int16_t y);
    uint8_t getByteFromBitmap(int16_t x ,int16_t y);
public:
    logoBW(const unsigned char *logo, bool negative = false, bool transparent = false, bool transparentColor = false);
    logoBW *paint(struct box_s *box, int16_t x_pos ,int16_t y_pos);
    logoBW *setLogo(uint8_t *logo) { this->logo = logo; return this; }
    uint8_t *getLogo() { return logo; }
    logoBW *setNegative(bool negative) { this->negative = negative;  return this; }
    bool getNegative() { return negative; }
    logoBW *setTransparent(bool transparent) { this->transparent = transparent;  return this; }
    bool getTransparent() { return transparent; }
    logoBW *setTransparentColor(bool transparentColor) { this->transparentColor = transparentColor;  return this; }
    bool getTransparentColor() { return transparentColor; }
};

#endif /* LOGO_H_ */
