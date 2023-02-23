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

#include "hKbd_lrudab.h"
#include "sys/sTimer.h"
#if !defined(QT_WIDGETS_LIB)
#include <Arduino.h>
#endif

uint8_t state = 0;
uint8_t last_state = 0;
sTimer kbd_u_timer;
sTimer kbd_d_timer;
sTimer kbd_l_timer;
sTimer kbd_r_timer;

hKbd_lrudab::hKbd_lrudab(int repeatKeyDelay, int firstKeyPause) {
    this->repeatKeyDelay = repeatKeyDelay;
    this->firstKeyPause = firstKeyPause;
    kbd_u_timer.SetInterval(repeatKeyDelay);
    kbd_d_timer.SetInterval(repeatKeyDelay);
    kbd_l_timer.SetInterval(repeatKeyDelay);
    kbd_r_timer.SetInterval(repeatKeyDelay);
	/* Configure the GPIO_LED pin */
#ifdef KBD_A
	pinMode(KBD_A, INPUT_PULLUP);
#endif
#ifdef KBD_B
	pinMode(KBD_B, INPUT_PULLUP);
#endif
#ifdef KBD_U
	pinMode(KBD_U, INPUT_PULLUP);
#endif
#ifdef KBD_D
	pinMode(KBD_D, INPUT_PULLUP);
#endif
#ifdef KBD_R
	pinMode(KBD_R, INPUT_PULLUP);
#endif
#ifdef KBD_L
	pinMode(KBD_L, INPUT_PULLUP);
#endif
}

void hKbd_lrudab::loop(uint8_t stat) {
    state = stat;
#if !defined(QT_WIDGETS_LIB)
#ifdef KBD_L
	if(!digitalRead(KBD_L))
		state |= KBD_L_KEY;
#endif
#ifdef KBD_U
	if(!digitalRead(KBD_U))
		state |= KBD_U_KEY;
#endif
#ifdef KBD_D
	if(!digitalRead(KBD_D))
		state |= KBD_D_KEY;
#endif
#ifdef KBD_R
	if(!digitalRead(KBD_R))
		state |= KBD_R_KEY;
#endif
#ifdef KBD_A
	if(!digitalRead(KBD_A))
		state |= KBD_A_KEY;
#endif
#ifdef KBD_B
	if(!digitalRead(KBD_B))
		state |= KBD_B_KEY;
#endif
#else
#endif
}

bool hKbd_lrudab::getChanged() {
	bool changed = false;
#if 1
	if((state & KBD_U_KEY) != (last_state & KBD_U_KEY) && (state & KBD_U_KEY)) {
		last_state |= KBD_U_KEY;
        kbd_u_timer.Start(firstKeyPause);
		changed = true;
	} else if((state & KBD_U_KEY) != (last_state & KBD_U_KEY) && (~state & KBD_U_KEY)) {
		last_state &= ~KBD_U_KEY;
		kbd_u_timer.Stop();
	}
	if((state & KBD_D_KEY) != (last_state & KBD_D_KEY) && (state & KBD_D_KEY)) {
		last_state |= KBD_D_KEY;
        kbd_d_timer.Start(firstKeyPause);
		changed = true;
	} else if((state & KBD_D_KEY) != (last_state & KBD_D_KEY) && (~state & KBD_D_KEY)) {
		last_state &= ~KBD_D_KEY;
		kbd_d_timer.Stop();
	}
	if((state & KBD_L_KEY) != (last_state & KBD_L_KEY) && (state & KBD_L_KEY)) {
		last_state |= KBD_L_KEY;
        kbd_l_timer.Start(firstKeyPause);
		changed = true;
	} else if((state & KBD_L_KEY) != (last_state & KBD_L_KEY) && (~state & KBD_L_KEY)) {
		last_state &= ~KBD_L_KEY;
		kbd_l_timer.Stop();
	}
	if((state & KBD_R_KEY) != (last_state & KBD_R_KEY) && (state & KBD_R_KEY)) {
		last_state |= KBD_R_KEY;
        kbd_r_timer.Start(firstKeyPause);
		changed = true;
	} else if((state & KBD_R_KEY) != (last_state & KBD_R_KEY) && (~state & KBD_R_KEY)) {
		last_state &= ~KBD_R_KEY;
		kbd_r_timer.Stop();
	}
	if(kbd_u_timer.Tick()) {
        kbd_u_timer.SetInterval(repeatKeyDelay);
		changed = true;
	}
	if(kbd_d_timer.Tick()) {
        kbd_d_timer.SetInterval(repeatKeyDelay);
		changed = true;
	}
	if(kbd_l_timer.Tick()) {
        kbd_l_timer.SetInterval(repeatKeyDelay);
		changed = true;
	}
	if(kbd_r_timer.Tick()) {
        kbd_r_timer.SetInterval(repeatKeyDelay);
		changed = true;
	}
	if((last_state & (KBD_A_KEY | KBD_B_KEY)) ^ (state & (KBD_A_KEY | KBD_B_KEY))) {
		changed = true;
#if !defined(QT_WIDGETS_LIB)
        delay(10);
#endif
	}
	last_state = (last_state & ~(KBD_A_KEY | KBD_B_KEY)) | (state & (KBD_A_KEY | KBD_B_KEY));

#else
	if(last_state ^ state) {
		changed = true;
		delay(10);
	}
	last_state = state;
#endif
	return changed;
}

uint8_t hKbd_lrudab::get() {
	return last_state;
}
