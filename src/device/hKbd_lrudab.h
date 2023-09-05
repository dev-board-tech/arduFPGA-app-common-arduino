/*
 * Keyboard driver file for arduFPGA designs.
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

#ifndef KBD_H_
#define KBD_H_

#include <stdbool.h>
#include <stdint.h>
#include "sys/sTimer.h"

#define KBD_L_KEY	(1<<5)
#define KBD_U_KEY	(1<<4)
#define KBD_D_KEY	(1<<3)
#define KBD_R_KEY	(1<<2)
#define KBD_A_KEY	(1<<1)
#define KBD_B_KEY	(1<<0)

class hKbd_lrudab {
public:
    hKbd_lrudab(int repeatKeyDelay = 100, int firstKeyPause = 400);
    void loop(uint8_t stat = 0);
    bool getChanged();
    void getChangedPutBack(bool chg);
    uint8_t get();
    void getPutBack(char c);
    void setTimings(int repeatKeyDelay = 100, int firstKeyPause = 400) {
        this->firstKeyPause = firstKeyPause;
        this->repeatKeyDelay = repeatKeyDelay;
    }
    void setFirstKeyPause(int firstKeyPause) {
        this->firstKeyPause = firstKeyPause;
    };
    void setRepeatKeyDelay(int repeatKeyDelay) {
        this->repeatKeyDelay = repeatKeyDelay;
    }
private:
    int firstKeyPause = 400;
    int repeatKeyDelay = 100;
};

#endif /* KBD_H_ */
