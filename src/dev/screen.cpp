/*
 * Graphics library.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "screen.h"

#include "../util/math.h"

Screen *defaultScreen = NULL;

//#######################################################################################
Screen *Screen::setDefault() {
	defaultScreen = this;
	return this;
}

Screen *Screen::setScreen(Screen *scr) {
	defaultScreen = scr;
	return defaultScreen;
}

Screen *Screen::getDefault() {
	return defaultScreen;
}

bool Screen::remove() {
	if(this == defaultScreen) {
		defaultScreen = NULL;
		return true;
	}
	return false;
}

//#######################################################################################
Screen *Screen::init() {
	return InitPtr(DriverPtr);
}
void Screen::idle() {
	IdlePtr(DriverPtr);
}
void Screen::triggerRefresh() {
	TriggerRefreshPtr(DriverPtr);
}
int Screen::getX() {
	return GetXPtr(DriverPtr);
}
int Screen::getY() {
	return GetYPtr(DriverPtr);
}
Screen *Screen::drvRefresh() {
	return DrvRefreshPtr(DriverPtr);
}
Screen *Screen::drvOn(bool state) {
	return DrvOnPtr(DriverPtr, state);
}
Screen *Screen::drvSetContrast(byte cont) {
	return DrvSetContrastPtr(DriverPtr, cont);
}
Screen *Screen::drvDrawPixel(int x, int y, int color) {
	return DrvDrawPixelPtr(DriverPtr, x, y, color);
}
Screen *Screen::drvDrawPixelBox(struct box_s *box, int x, int y, int color) {
	return DrvDrawPixelBoxPtr(DriverPtr, box, x, y, color);
}
Screen *Screen::drvDrawRectangle(int x, int y, int x_size, int y_size, bool fill, int color) {
	return DrvDrawRectanglePtr(DriverPtr, x, y, x_size, y_size, fill, color);
}
Screen *Screen::drvDrawRectangleBox(struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color) {
	return DrvDrawRectangleBoxPtr(DriverPtr, box, x, y, x_size, y_size, fill, color);
}
Screen *Screen::drvDrawHLine(int x1, int x2, int y, byte width, int color) {
	return DrvDrawHLinePtr(DriverPtr, x1, x2, y, width, color);
}
Screen *Screen::drvDrawHLineBox(struct box_s *box, int x1, int x2, int y, byte width, int color) {
	return DrvDrawHLineBoxPtr(DriverPtr, box, x1, x2, y, width, color);
}
Screen *Screen::drvDrawVLine(int y1, int y2, int x, byte width, int color) {
	return DrvDrawVLinePtr(DriverPtr, y1, y2, x, width, color);
}
Screen *Screen::drvDrawVLineBox(struct box_s *box, int y1, int y2, int x, byte width, int color) {
	return DrvDrawVLineBoxPtr(DriverPtr, box, y1, y2, x, width, color);
}
Screen *Screen::drvClear(int color) {
	return DrvClearPtr(DriverPtr, color);
}
Screen *Screen::drvDrawString(char *string, int x, int y, int foreColor, int inkColor) {
	return DrvDrawStringPtr(DriverPtr, string, x, y, foreColor, inkColor);
}
Screen *Screen::drvDrawStringBox(struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor) {
	return DrvDrawStringBoxPtr(DriverPtr, box, string, x, y, terminalMode, wordWrap, foreColor, inkColor);
}

//#######################################################################################

Screen *Screen::drawCircle(signed int x, signed int y, unsigned int _radius, bool fill, unsigned int color) {
	return drawCircle(box, x, y,_radius, fill, color);
}
Screen *Screen::drawCircle(struct box_s *box, signed int x, signed int y, unsigned int _radius, bool fill, unsigned int color) {
	unsigned int radius = _radius;
	if (radius < 0)
		radius = ~radius;
	int a, b, P;
	a = 0;
	b = radius;
	P = 1 - radius;
	int Tmp1, Tmp2, Tmp3, Tmp4;
	int Tmp5, Tmp6, Tmp7, Tmp8;
	int _Tmp5 = 5, _Tmp7 = 0;
	do {
		Tmp1 = x + a;
		Tmp2 = x - a;
		Tmp3 = x + b;
		Tmp4 = x - b;
		Tmp5 = y + a;
		Tmp6 = y - a;
		Tmp7 = y + b;
		Tmp8 = y - b;
		if (fill) {
			if (_Tmp7 != Tmp7) {
				drvDrawHLineBox(box, Tmp2, Tmp1 - Tmp2, Tmp7, 1, color);
				drvDrawHLineBox(box, Tmp2, Tmp1 - Tmp2, Tmp8, 1, color);
			}
			if (_Tmp5 != Tmp5) {
				drvDrawHLineBox(box, Tmp4, Tmp3 - Tmp4, Tmp5, 1, color);
				drvDrawHLineBox(box, Tmp4, Tmp3 - Tmp4, Tmp6, 1, color);
			}
			_Tmp5 = Tmp5;
			_Tmp7 = Tmp7;
		} else {
			drvDrawPixelBox(box, Tmp1, Tmp7, color);
			drvDrawPixelBox(box, Tmp3, Tmp5, color);
			drvDrawPixelBox(box, Tmp2, Tmp7, color);
			drvDrawPixelBox(box, Tmp4, Tmp5, color);
			drvDrawPixelBox(box, Tmp3, Tmp6, color);
			drvDrawPixelBox(box, Tmp1, Tmp8, color);
			drvDrawPixelBox(box, Tmp2, Tmp8, color);
			drvDrawPixelBox(box, Tmp4, Tmp6, color);
		}

		if (P < 0)
			P += 3 + 2 * a++;
		else
			P += 5 + 2 * (a++ - b--);
	} while (a <= b);
	//refresh();
	return this;
}
//#######################################################################################
Screen *Screen::drawLine(signed int X1, signed int Y1, signed int X2, signed int Y2, int width, unsigned int color) {
	return drawLine(box, X1, Y1, X2, Y2, width, color);
}

Screen *Screen::drawLine(struct box_s *box, signed int X1, signed int Y1, signed int X2, signed int Y2, int width, unsigned int color) {
	if (width == 1) {
		signed int currentX, currentY, Xinc, Yinc, Dx, Dy, TwoDx, TwoDy,
				twoDxAccumulatedError, twoDyAccumulatedError;
		Dx = (X2 - X1);
		Dy = (Y2 - Y1);
		TwoDx = Dx + Dx;
		TwoDy = Dy + Dy;
		currentX = X1;
		currentY = Y1;
		Xinc = 1;
		Yinc = 1;
		if (Dx < 0) {
			Xinc = -1;
			Dx = -Dx;
			TwoDx = -TwoDx;
		}
		if (Dy < 0) {
			Yinc = -1;
			Dy = -Dy;
			TwoDy = -TwoDy;
		}
		drvDrawPixelBox(box, X1, Y1, color);
		if ((Dx != 0) || (Dy != 0)) {
			if (Dy <= Dx) {
				twoDxAccumulatedError = 0;
				do {
					currentX += Xinc;
					twoDxAccumulatedError += TwoDy;
					if (twoDxAccumulatedError > Dx) {
						currentY += Yinc;
						twoDxAccumulatedError -= TwoDx;
					}
					drvDrawPixelBox(box, currentX, currentY, color);
				} while (currentX != X2);
			}
			else {
				twoDyAccumulatedError = 0;
				do {
					currentY += Yinc;
					twoDyAccumulatedError += TwoDx;
					if (twoDyAccumulatedError > Dy) {
						currentX += Xinc;
						twoDyAccumulatedError -= TwoDy;
					}
					drvDrawPixelBox(box, currentX, currentY, color);
				} while (currentY != Y2);
			}
		}
	} else {
		int half_width;
		int half_width_square;
		int dy, dx;
		int addx = 1, addy = 1, j;
		int P, diff;
		int c1, c2;
		int i = 0;
		half_width = width >> 1;
		half_width_square = half_width;
		dx = X2 - X1;
		dy = Y2 - Y1;
		if(dx < 0) {
			dx = -dx;
			half_width_square = half_width * half_width;
		}
		if(dy < 0) {
			dy = -dy;
			half_width_square = half_width * half_width;
		}
		c1 = -(dx * X1 + dy * Y1);
		c2 = -(dx * X2 + dy * Y2);
		if (X1 > X2) {
			int temp;
			temp = c1;
			c1 = c2;
			c2 = temp;
			addx = -1;
		}
		if (Y1 > Y2) {
			int temp;
			temp = c1;
			c1 = c2;
			c2 = temp;
			addy = -1;
		}
		if (dx >= dy) {
			P = 2 * dy - dx;
			diff = P - dx;

			for (i = 0; i <= dx; ++i) {
#ifdef USE_FAST_DRAW_H_LINE
				DrawVLine(box, Y1+ (-half_width), Y1+ (half_width+width % 2), X1, 1, color);
#else
				for (j = -half_width; j < half_width + width % 2; ++j) {
					int temp;

					temp = dx * X1 + dy * (Y1 + j); // Use more RAM to increase speed
					if (temp + c1 >= -half_width_square && temp + c2 <= half_width_square)
						drvDrawPixelBox(box, X1, Y1 + j, color);
				}
#endif
				if (P < 0) {
					P += 2 * dy;
					X1 += addx;
				} else {
					P += diff;
					X1 += addx;
					Y1 += addy;
				}
			}
		}
		else {
			P = 2 * dx - dy;
			diff = P - dy;
			for (i = 0; i <= dy; ++i) {
				if (P < 0) {
					P += 2 * dx;
					Y1 += addy;
				} else {
					P += diff;
					X1 += addx;
					Y1 += addy;
				}
#ifdef USE_FAST_DRAW_H_LINE
				DrawVLine(box, (X1+(-half_width)), (X1+(half_width+width % 2)), Y1, 1, color);
#else
				for (j = -half_width; j < half_width + width % 2; ++j) {
					int temp;

					temp = dx * X1 + dy * (Y1 + j); // Use more RAM to increase speed
					if (temp + c1 >= 0 && temp + c2 <= 0)
						drvDrawPixelBox(box, X1 + j, Y1, color);
				}
#endif
			}
		}
	}
	return this;
}

//#######################################################################################
void Screen::elipseplot(struct box_s *box, int xc, int yc,
	int x, int y, bool fill, int color) {
	int Tmp1 = xc + x;
	int Tmp2 = xc - x;
	int Tmp3 = yc + y;
	int Tmp4 = yc - y;
	if (fill) {
		drvDrawHLineBox(box, Tmp2, Tmp1 - Tmp2, Tmp3, 1, color);
		drvDrawHLineBox(box, Tmp2, Tmp1 - Tmp2, Tmp4, 1, color);
	} else {
		drvDrawPixelBox(box, (unsigned int) (Tmp1), (unsigned int) (Tmp3), color);
		drvDrawPixelBox(box, (unsigned int) (Tmp2), (unsigned int) (Tmp3), color);
		drvDrawPixelBox(box, (unsigned int) (Tmp1), (unsigned int) (Tmp4), color);
		drvDrawPixelBox(box, (unsigned int) (Tmp2), (unsigned int) (Tmp4), color);
	}
}
//----------------------------------------------------------------------------------------
Screen *Screen::drawElipse(int x, int y,unsigned int rx,unsigned int ry, bool fill, int color) {
	return drawElipse(box, x, y, rx, ry, fill, color);
}

Screen *Screen::drawElipse(struct box_s *box, int x, int y, unsigned int rx, unsigned int ry, bool fill, int color) {
	unsigned int _rx = rx;
	if (_rx < 0)
		_rx = 0xFFFFFFFF - _rx;
	unsigned int _ry = ry;
	if (_ry < 0)
		_ry = 0xFFFFFFFF - _ry;
	int rx2 = _rx * _rx;
	int ry2 = _ry * _ry;
	int tory2 = 2 * ry2;
	int torx2 = 2 * rx2;
	int p;
	int _x = 0;
	int _y = _ry;
	int py = torx2 * _y;
	int px = 0;
	elipseplot(box, x, y, _x, _y, fill, color);
	p = /*round(*/ry2 - (rx2 * _ry) + (0.25 * rx2)/*)*/;
	while (px < py) {
		_x++;
		px += tory2;
		if (p < 0)
			p += ry2 + px;
		else {
			_y--;
			py -= torx2;
			p += ry2 + px - py;
		}
		elipseplot(box, x, y, _x, _y, fill, color);
	}
	p = /*round(*/ry2 * (_x + 0.5) * (_x + 0.5) + rx2 * (_y - 1) * (_y - 1)
			- rx2 * ry2/*)*/;
	while (_y > 0) {
		_y--;
		py -= torx2;
		if (p > 0)
			p += rx2 - py;
		else {
			_x++;
			px += tory2;
			p += rx2 - py + px;
		}
		elipseplot(box, x, y, _x, _y, fill, color);
	}
	return this;
}

//#######################################################################################
/*
 *  the coordinates of vertices are (A.x,A.y), (B.x,B.y), (C.x,C.y); we assume that A.y<=B.y<=C.y (you should sort them first)
 * dx1,dx2,dx3 are deltas used in interpolation
 * horizline draws horizontal segment with coordinates (S.x,Y), (E.x,Y)
 * S.x, E.x are left and right x-coordinates of the segment we have to draw
 * S=A means that S.x=A.x; S.y=A.y;
 */

static void triangle_swap_nibble(int* a, int *b) {
	signed int t = *a;
	*a = *b;
	*b = t;
}

Screen *Screen::drawTriangle(int  ax, int  ay, int  bx, int  by, int  cx, int  cy, bool fill, int color) {
	return drawTriangle(box, ax, ay, bx, by, cx, cy, fill, color);
}
Screen *Screen::drawTriangle(struct box_s *box, int ax, int ay, int bx, int by, int cx, int cy, bool fill, int color) {
	if(!fill) {
		drawLine(box, ax, ay, bx, by, 1, color);
		drawLine(box, ax, ay, cx, cy, 1, color);
		drawLine(box, bx, by, cx, cy, 1, color);
		return this;
	}
	long dx1, dx2, dx3;
	long sx, ex;
	long sy, ey;

	if (ay > by) {
		triangle_swap_nibble(&ay, &by);
		triangle_swap_nibble(&ax, &bx);
	}
	if (ay > cy) {
		triangle_swap_nibble(&ay, &cy);
		triangle_swap_nibble(&ax, &cx);
	}
	if (by > cy) {
		triangle_swap_nibble(&by, &cy);
		triangle_swap_nibble(&bx, &cx);
	}
	if (by - ay > 0)
		dx1 = ((long) (bx - ax) << 16) / (by - ay);
	else
		dx1 = 0;
	if (cy - ay > 0)
		dx2 = ((long) (cx - ax) << 16) / (cy - ay);
	else
		dx2 = 0;
	if (cy - by > 0)
		dx3 = ((long) (cx - bx) << 16) / (cy - by);
	else
		dx3 = 0;
	ex = sx = (long) ax << 16;
	ey = sy = ay;

	if (dx1 > dx2) {
		while (sy <= by) {
			drawLine(box, sx >> 16, sy++, ex >> 16, ey++, 1, color);
			sx += dx2;
			ex += dx1;
		}
		ex = (long) bx << 16;
		ey = by;
		while (sy <= cy) {
			drawLine(box, sx >> 16, sy++, ex >> 16, ey++, 1, color);
			sx += dx2;
			ex += dx3;
		}
	}
	else {
		while (sy <= by) {
			drawLine(box, sx >> 16, sy++, ex >> 16, ey++, 1, color);
			sx += dx1;
			ex += dx2;
		}
		sx = (long) bx << 16;
		sy = by;
		while (sy <= cy) {
			drawLine(box, sx >> 16, sy++, ex >> 16, ey++, 1, color);
			sx += dx3;
			ex += dx2;
		}
	}
	return this;
}

//#######################################################################################
#ifdef __AVR_MEGA__
Screen *Screen::drawMsgAndProgress(const char *text_P, long progress_min, long progress_max, long progress_value) {
	drawMsgAndProgress(text_P, NULL, progress_min, progress_max, progress_value);
	return this;
}
#endif
Screen *Screen::drawMsgAndProgress(char *text_R, long progress_min, long progress_max, long progress_value) {
	drawMsgAndProgress(NULL, text_R, progress_min, progress_max, progress_value);
	return this;
}
void Screen::drawMsgAndProgress(const char *text_P, char *text_R, long progress_min, long progress_max, long progress_value) {
	char *buf = NULL;
#ifdef __AVR_MEGA__
	if(text_P) {
		buf = (char *)malloc(strlen_P(text_P) + 1);
		if(!buf) {
			return;
		}
		strcpy_P(buf, text_P);
	} else
#endif
	if(text_R) {
		buf = text_R;
	} else {
		return;
	}
	int32_t bar_len = math::intToPercent(getX(), progress_min, progress_max, progress_value);
	drvClear(0);
	if(bar_len) {
		drvDrawRectangleBox(NULL, 0, 32, bar_len, 8, true, true);
	}
	drvDrawString(buf, 0, 8, 0, -1);
}

//#######################################################################################
Screen *Screen::drawMsg(const char *text_P, int x, int y) {
	drawMsg(text_P, NULL, x, y);
	return this;
}
Screen *Screen::drawMsg(char *text_R, int x, int y) {
	drawMsg(NULL, text_R, x, y);
	return this;
}
void Screen::drawMsg(const char *text_P, char *text_R, int x, int y) {
	char *buf = NULL;
	if(text_P) {
		buf = (char *)malloc(strlen_P(text_P) + 1);
		if(!buf) {
			return;
		}
		strcpy_P(buf, text_P);
	} else if(text_R) {
		buf = text_R;
	} else {
		return;
	}
	drvClear(0);
	drvDrawString(buf, x, y, 0, 1);
}


//#######################################################################################


// Font Definition
#ifdef __AVR_MEGA__
const char font4x6[97][2] PROGMEM = {
#else
const char font4x6[97][2] = {
#endif
	{ 0x00  ,  0x00 },   /*SPACE*/
	{ 0x49  ,  0x08 },   /*'!'*/
	{ 0xb4  ,  0x00 },   /*'"'*/
	{ 0xbe  ,  0xf6 },   /*'#'*/
	{ 0x7b  ,  0x7a },   /*'$'*/
	{ 0xa5  ,  0x94 },   /*'%'*/
	{ 0x55  ,  0xb8 },   /*'&'*/
	{ 0x48  ,  0x00 },   /*'''*/
	{ 0x29  ,  0x44 },   /*'('*/
	{ 0x44  ,  0x2a },   /*')'*/
	{ 0x15  ,  0xa0 },   /*'*'*/
	{ 0x0b  ,  0x42 },   /*'+'*/
	{ 0x00  ,  0x50 },   /*','*/
	{ 0x03  ,  0x02 },   /*'-'*/
	{ 0x00  ,  0x08 },   /*'.'*/
	{ 0x25  ,  0x90 },   /*'/'*/
	{ 0x76  ,  0xba },   /*'0'*/
	{ 0x59  ,  0x5c },   /*'1'*/
	{ 0xc5  ,  0x9e },   /*'2'*/
	{ 0xc5  ,  0x38 },   /*'3'*/
	{ 0x92  ,  0xe6 },   /*'4'*/
	{ 0xf3  ,  0x3a },   /*'5'*/
	{ 0x73  ,  0xba },   /*'6'*/
	{ 0xe5  ,  0x90 },   /*'7'*/
	{ 0x77  ,  0xba },   /*'8'*/
	{ 0x77  ,  0x3a },   /*'9'*/
	{ 0x08  ,  0x40 },   /*':'*/
	{ 0x08  ,  0x50 },   /*';'*/
	{ 0x2a  ,  0x44 },   /*'<'*/
	{ 0x1c  ,  0xe0 },   /*'='*/
	{ 0x88  ,  0x52 },   /*'>'*/
	{ 0xe5  ,  0x08 },   /*'?'*/
	{ 0x56  ,  0x8e },   /*'@'*/
	{ 0x77  ,  0xb6 },   /*'A'*/
	{ 0x77  ,  0xb8 },   /*'B'*/
	{ 0x72  ,  0x8c },   /*'C'*/
	{ 0xd6  ,  0xba },   /*'D'*/
	{ 0x73  ,  0x9e },   /*'E'*/
	{ 0x73  ,  0x92 },   /*'F'*/
	{ 0x72  ,  0xae },   /*'G'*/
	{ 0xb7  ,  0xb6 },   /*'H'*/
	{ 0xe9  ,  0x5c },   /*'I'*/
	{ 0x64  ,  0xaa },   /*'J'*/
	{ 0xb7  ,  0xb4 },   /*'K'*/
	{ 0x92  ,  0x9c },   /*'L'*/
	{ 0xbe  ,  0xb6 },   /*'M'*/
	{ 0xd6  ,  0xb6 },   /*'N'*/
	{ 0x56  ,  0xaa },   /*'O'*/
	{ 0xd7  ,  0x92 },   /*'P'*/
	{ 0x76  ,  0xee },   /*'Q'*/
	{ 0x77  ,  0xb4 },   /*'R'*/
	{ 0x71  ,  0x38 },   /*'S'*/
	{ 0xe9  ,  0x48 },   /*'T'*/
	{ 0xb6  ,  0xae },   /*'U'*/
	{ 0xb6  ,  0xaa },   /*'V'*/
	{ 0xb6  ,  0xf6 },   /*'W'*/
	{ 0xb5  ,  0xb4 },   /*'X'*/
	{ 0xb5  ,  0x48 },   /*'Y'*/
	{ 0xe5  ,  0x9c },   /*'Z'*/
	{ 0x69  ,  0x4c },   /*'['*/
	{ 0x91  ,  0x24 },   /*'\'*/
	{ 0x64  ,  0x2e },   /*']'*/
	{ 0x54  ,  0x00 },   /*'^'*/
	{ 0x00  ,  0x1c },   /*'_'*/
	{ 0x44  ,  0x00 },   /*'`'*/
	{ 0x0e  ,  0xae },   /*'a'*/
	{ 0x9a  ,  0xba },   /*'b'*/
	{ 0x0e  ,  0x8c },   /*'c'*/
	{ 0x2e  ,  0xae },   /*'d'*/
	{ 0x0e  ,  0xce },   /*'e'*/
	{ 0x56  ,  0xd0 },   /*'f'*/
	{ 0x55  ,  0x3B },   /*'g'*/
	{ 0x93  ,  0xb4 },   /*'h'*/
	{ 0x41  ,  0x44 },   /*'i'*/
	{ 0x41  ,  0x51 },   /*'j'*/
	{ 0x97  ,  0xb4 },   /*'k'*/
	{ 0x49  ,  0x44 },   /*'l'*/
	{ 0x17  ,  0xb6 },   /*'m'*/
	{ 0x1a  ,  0xb6 },   /*'n'*/
	{ 0x0a  ,  0xaa },   /*'o'*/
	{ 0xd6  ,  0xd3 },   /*'p'*/
	{ 0x76  ,  0x67 },   /*'q'*/
	{ 0x17  ,  0x90 },   /*'r'*/
	{ 0x0f  ,  0x38 },   /*'s'*/
	{ 0x9a  ,  0x8c },   /*'t'*/
	{ 0x16  ,  0xae },   /*'u'*/
	{ 0x16  ,  0xba },   /*'v'*/
	{ 0x16  ,  0xf6 },   /*'w'*/
	{ 0x15  ,  0xb4 },   /*'x'*/
	{ 0xb5  ,  0x2b },   /*'y'*/
	{ 0x1c  ,  0x5e },   /*'z'*/
	{ 0x6b  ,  0x4c },   /*'{'*/
	{ 0x49  ,  0x48 },   /*'|'*/
	{ 0xc9  ,  0x5a },   /*'}'*/
	{ 0x54  ,  0x00 },   /*'~'*/
	{ 0x56  ,  0xe2 },   /*''*/
	{ 0xFF  ,  0xFE }    /*''*/
};
#define FIRST_CHAR_IDX      32

Screen *Screen::drawChar4x6(int x, int y,  char c, int foreColor, int inkColor) {
	return drawChar4x6(box, x, y, c, foreColor, inkColor);
}
Screen *Screen::drawChar4x6(struct box_s *box, int x, int y,  char c, int foreColor, int inkColor) {
	const unsigned char index = ((unsigned char)(c)) - FIRST_CHAR_IDX;
	if (index > 97)
		return this;
#ifdef __AVR_MEGA__
	unsigned char data1 = pgm_read_byte(&font4x6[index][0]);
	unsigned char data2 = pgm_read_byte(&font4x6[index][1]);
#else
	unsigned char data1 = font4x6[index][0];
	unsigned char data2 = font4x6[index][1];
#endif
	int fg = inkColor;
	if(foreColor != inkColor) {
		drvDrawRectangleBox(box, x, y, 4, 6, true, foreColor);
	}
	if (data2 & 1)	// Descender e.g. j, g
	{
		y++;

		if(data1 & 0x80)
			drvDrawPixelBox(box ,x, y, fg);
		if (data1 & 0x40)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data1 & 0x20)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data1 & 0x10)
			drvDrawPixelBox(box ,x, y, fg);
		if (data1 & 0x8)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data1 & 0x4)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data1 & 0x2)
			drvDrawPixelBox(box ,x, y, fg);
		if (data1 & 0x1)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data2 & 0x2)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data2 & 0x80)
			drvDrawPixelBox(box ,x, y, fg);
		if (data2 & 0x40)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data2 & 0x20)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data2 & 0x10)
			drvDrawPixelBox(box ,x, y, fg);
		if (data2 & 0x8)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data2 & 0x4)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;
	} else {
		if (data1 & 0x80)
			drvDrawPixelBox(box ,x, y, fg);
		if (data1 & 0x40)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data1 & 0x20)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data1 & 0x10)
			drvDrawPixelBox(box ,x, y, fg);
		if (data1 & 0x8)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data1 & 0x4)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data1 & 0x2)
			drvDrawPixelBox(box ,x, y, fg);
		if (data1 & 0x1)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data2 & 0x2)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data2 & 0x80)
			drvDrawPixelBox(box ,x, y, fg);
		if (data2 & 0x40)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data2 & 0x20)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;

		if (data2 & 0x10)
			drvDrawPixelBox(box ,x, y, fg);
		if (data2 & 0x8)
			drvDrawPixelBox(box ,x + 1, y, fg);
		if (data2 & 0x4)
			drvDrawPixelBox(box ,x + 2, y, fg);
		y++;
	}
	return this;
}

static inline char getCharHeight4x6() {
	return 6;
}

static inline char getCharWidth4x6() {
	return 4;
}

int gfxString::getRowsInBox4x6() {
	if(defaultScreen == NULL)
		return 0;
	return getRowsInBox4x6(defaultScreen->getBox());
}
int gfxString::getRowsInBox4x6(struct box_s *box) {
	int cnt = 1;
	int t = box->y_min + getCharHeight4x6();
	for(; t < box->y_max; t += getCharHeight4x6())
		cnt++;
	return cnt;
}

int gfxString::getColsInBox4x6() {
	if(defaultScreen == NULL)
		return 0;
	return getColsInBox4x6(defaultScreen->getBox());
}
int gfxString::getColsInBox4x6(struct box_s *box) {
	int cnt = 1;
	int t = box->x_min + getCharWidth4x6();
	for(; t < box->x_max; t += getCharWidth4x6())
		cnt++;
	return cnt;
}

gfxString *gfxString::drawStringWindowed4x6(char *string, int16_t x, int16_t y, int16_t cursorPos, bool cursorState) {
	//if(defaultScreen == NULL)
		//return this;
	return drawStringWindowed4x6(defaultScreen->getBox(), string, x, y, cursorPos, cursorState);
}
gfxString *gfxString::drawStringWindowed4x6(struct box_s *box, char *string, int16_t x, int16_t y, int16_t cursorPos, bool cursorState) {
	//if(defaultScreen == NULL)
		//return this;
    struct box_s box__;
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
    int16_t cX = x;
    int16_t cY = y;
    int16_t cCnt = 0;
    edgeTouch = 0;
    uint16_t cLineCnt = 0;;
    rowCnt = 0;
    do {
	    if(cCnt == maxLen && maxLen) {
		    return this;
	    }
	    if(cursorPos == cCnt) {
		    if(cX < box__.x_min) {
			    edgeTouch |= EDGE_OUTSIDE_LEFT;
		    }
		    if(cY <= box__.y_min) {
			    edgeTouch |= EDGE_OUTSIDE_UP;
		    }
		    if(cX >= box__.x_max) {
			    edgeTouch |= EDGE_OUTSIDE_RIGHT;
		    }
		    if(cY >= box__.y_max - 6) {
			    edgeTouch |= EDGE_OUTSIDE_DOWN;
		    }
	    }
	    char C, c = C = *pcString;
	    if(cursorPos == cCnt) {
		    if((cursorState) || (cursorState && C == 0))
		    	C = (uint8_t)(96+32);
	    }
	    if(!transparent)
	    	defaultScreen->drawChar4x6(&box__, cX, cY, C, foreColor, inkColor);
	    if (c == 0) {
		    return this;
	    }
	    switch (c) {
		    case '\r':
		    case '\n':
				if(cY > box__.y_max)
					return this;
				cX = x;
				cY += getCharHeight4x6();
				rowCnt++;
				break;
		    case 0x09:
				cX = (getCharWidth4x6() * tabSpaces) + ((cX / (getCharWidth4x6() * tabSpaces)) * (getCharWidth4x6() * tabSpaces));
				break;
		    default:
				cX += getCharWidth4x6();
				if ((cX + getCharWidth4x6() > box__.x_max)
				&& wordWrap == true) {
					cY += getCharHeight4x6();
					cX = x;
				}
				break;
	    }
	    cLineCnt++;
	    if(cLineCnt > maxLineLen && maxLineLen) {
		    cLineCnt = 0;
		    cX = x;
		    cY += getCharHeight4x6();
		    rowCnt++;
	    }
	    pcString++;
	    cCnt++;
    } while (1);
}

/*#####################################################*/
#ifdef __AVR_MEGA__
#include <avr/pgmspace.h>
static const uint8_t drawCharTable6x8[] PROGMEM =
#else
static const uint8_t drawCharTable6x8[] =
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

static inline char getCharHeight6x8() {
	return 6;
}

static inline char getCharWidth6x8() {
	return 4;
}

int gfxString::getRowsInBox6x8() {
	if(defaultScreen == NULL)
		return 0;
	return getRowsInBox4x6(defaultScreen->getBox());
}
int gfxString::getRowsInBox6x8(struct box_s *box) {
	int cnt = 1;
	int t = box->y_min + getCharHeight6x8();
	for(; t < box->y_max; t += getCharHeight6x8())
		cnt++;
	return cnt;
}

int gfxString::getColsInBox6x8() {
	if(defaultScreen == NULL)
		return 0;
	return getColsInBox6x8(defaultScreen->getBox());
}
int gfxString::getColsInBox6x8(struct box_s *box) {
	int cnt = 1;
	int t = box->x_min + getCharWidth6x8();
	for(; t < box->x_max; t += getCharWidth6x8())
		cnt++;
	return cnt;
}

gfxString *gfxString::drawStringWindowed6x8(char *string, int16_t x, int16_t y, int16_t cursorPos, bool cursorState) {
	if(defaultScreen == NULL)
		return this;
	return drawStringWindowed6x8(defaultScreen->getBox(), string, x, y, cursorPos, cursorState);
}
gfxString *gfxString::drawStringWindowed6x8(struct box_s *box, char *string, int16_t x, int16_t y, int16_t cursorPos, bool cursorState) {
	if(defaultScreen == NULL)
		return this;
    struct box_s box__;
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
	int8_t chWidth = 0;
	int8_t chHeight = 0;
	int16_t CharPtr;
	int8_t Tmp = 0;
	int16_t Cursor_X = x;
	int16_t Cursor_Y = y;
	bool ulVisible = true;
	int16_t CharCnt = 0;
	bool ulOpaque = false;
#ifdef __AVR_MEGA__
	chWidth = pgm_read_byte(&drawCharTable6x8[2]);
	chHeight = pgm_read_byte(&drawCharTable6x8[3]);
#else
	chWidth = drawCharTable6x8[2];
	chHeight = drawCharTable6x8[3];
#endif
	do {
		int8_t Char = *pcString;
		if (Char == 0) {
			return this;
		}
#ifdef __AVR_MEGA__
		CharPtr = ((Char - pgm_read_byte(&drawCharTable6x8[4])) * chWidth) + pgm_read_byte(&drawCharTable6x8[0]);
		if(Char < pgm_read_byte(&drawCharTable6x8[4]) || Char > pgm_read_byte(&drawCharTable6x8[5]))
#else
		CharPtr = ((Char - drawCharTable6x8[4]) * chWidth) + drawCharTable6x8[0];
		if (Char < drawCharTable6x8[4] || Char > drawCharTable6x8[5])
#endif
		{
			//chWidth_Tmp = chWidth;
			chWidth = 0;
			} else {
			int8_t Temp;
			/* if CompactWriting is true search the character for free cols from right to left and clear them */
			if (!terminalMode) {
				for (Tmp = 1; Tmp < chWidth; Tmp++) {
#ifdef __AVR_MEGA__
					Temp = pgm_read_byte(&drawCharTable6x8[Tmp + CharPtr]);
#else
					Temp = drawCharTable6x8[Tmp + CharPtr];
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
					int16_t XX = 0;
					int16_t YY = 0;
					for (XX = 0; XX < Tmp; XX++) {
#ifdef __AVR_MEGA__
						Temp = pgm_read_byte(&drawCharTable6x8[XX + CharPtr]);
#else
						Temp = drawCharTable6x8[XX + CharPtr];
#endif
						for (YY = 0; YY < chHeight; YY++) {
							if (Temp & 0x1) {
								defaultScreen->drvDrawPixelBox(&box__,
										XX + Cursor_X, YY + Cursor_Y, inkColor);
							}
							else {
								if(foreColor != inkColor)
									defaultScreen->drvDrawPixelBox(&box__,
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

