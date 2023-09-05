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

#ifndef __LIBRARIES_ARDUINO_SDK_DEV_SCREEN_H__
#define __LIBRARIES_ARDUINO_SDK_DEV_SCREEN_H__

#include <stdint.h>
#if !defined(QT_WIDGETS_LIB)
#include <Arduino.h>
#include <SPI.h>
#else
typedef uint8_t byte;
#endif
#include "../include/global.h"

class Screen {
public:
	Screen *init();
	void idle();
	void triggerRefresh();
	void triggerUpdate();
	int getX();
	int getY();
	Screen *drvRefresh();
	Screen *drvOn(bool state);
	Screen *drvSetContrast(byte cont);
	Screen *drvDrawPixel(int x, int y, int color);
	Screen *drvDrawPixelClip(struct box_s *box, int x, int y, int color);
    Screen *drvDrawRectangle(int x, int y, int x_size, int y_size, bool fill, int color);
    Screen *drvDrawRectangleClip(struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color);
    Screen *drvDrawRoundedRectangle(int x, int y, int x_size, int y_size, int corner_rad, bool fill, int color);
    Screen *drvDrawRoundedRectangle(struct box_s *box, int x, int y, int x_size, int y_size, int corner_rad, bool fill, int color);
    Screen *drvDrawHLine(int x1, int x2, int y, byte width, int color);
	Screen *drvDrawHLineClip(struct box_s *box, int x1, int x2, int y, byte width, int color);
	Screen *drvDrawVLine(int y1, int y2, int x, byte width, int color);
	Screen *drvDrawVLineClip(struct box_s *box, int y1, int y2, int x, byte width, int color);
	Screen *drvClear(int color);
	Screen *drvDrawString(char *string, int x, int y, int foreColor, int inkColor);
	Screen *drvDrawStringClip(struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor);

	Screen *setBox(struct box_s *box) {
		this->box = box;
		return this;
	}
	box_s *getBox() { return this->box; }
	Screen *setTerminalMode(bool mode) {
		this->terminalMode = mode;
		return this;
	}
	bool getTerminalMode() { return this->terminalMode; }
	Screen *setWordWrap(bool mode) {
		this->wordWrap = mode;
		return this;
	}
	bool getWordWrap() { return this->wordWrap; }
	Screen *setReverseColor(bool reversed) {
		this->reverseColor = reversed;
		return this;
	}
	bool getReverseColor() { return this->reverseColor; }
	Screen *setBuf(void *buf) {
		this->buf = (byte *)buf;
		return this;
	}
	byte *getBuf() { return this->buf; }

	Screen *drawCircle(int x, int y, unsigned int _radius, bool fill, unsigned int color);
	Screen *drawCircle(struct box_s *box, int x, int y, unsigned int _radius, bool fill, unsigned int color);
	Screen *drawLine(int X1, int Y1, int X2, int Y2, int width, unsigned int color);
	Screen *drawLine(struct box_s *box, int X1, int Y1, int X2, int Y2, int width, unsigned int color);
	Screen *drawElipse(int x, int y,unsigned int rx,unsigned int ry, bool fill, int color);
	Screen *drawElipse(struct box_s *box, int x, int y,unsigned int rx,unsigned int ry, bool fill, int color);
	Screen *drawTriangle(int  ax, int  ay, int  bx, int  by, int  cx, int  cy, bool fill, int color);
	Screen *drawTriangle(struct box_s *box,  int  ax, int  ay, int  bx, int  by, int  cx, int  cy, bool fill, int color);
#ifdef __AVR_MEGA__
	Screen *drawMsgAndProgress(const char *text_P, long progress_min, long progress_max, long progress_value);
#endif
	Screen *drawMsgAndProgress(char *text_R, long progress_min, long progress_max, long progress_value);
	Screen *drawMsg(const char *text_P, int x, int y);
	Screen *drawMsg(char *text_R, int x, int y);
	Screen *drawChar4x6(int x, int y,  char c, int foreColor, int ink);
	Screen *drawChar4x6(struct box_s *box, int x, int y,  char c, int foreColor, int ink);

	Screen *setScreen(Screen *scr);
	Screen *setDefault();
	Screen *getDefault();
	bool remove();
/* Platform specific variables */

	bool terminalMode;
	bool wordWrap;
	bool reverseColor;
	box_s *box;
#if !defined(QT_WIDGETS_LIB)
    SPIClass *spi;
#endif
	screenOrientation ScreenOrientation;
#if !defined(SSD1306_USE_NO_BUF) && !defined(WDPW21_USE_NO_BUF)
#ifdef SSD1306_BUF_SIZE_BYTES
	byte buf[SSD1306_BUF_SIZE_BYTES];
#elif WDPW21_BUF_SIZE_BYTES
	byte buf[WDPW21_BUF_SIZE_BYTES];
#else
	uint8_t *buf;
#endif // SSD1306_BUF_SIZE_BYTES
#endif // SSD1306_USE_BUF
private:
	void elipseplot(struct box_s *box, int xc, int yc,
		int x, int y, bool fill, int color);
	void drawMsgAndProgress(const char *text_P, char *text_R, long progress_min, long progress_max, long progress_value);
	void drawMsg(const char *text_P, char *text_R, int x, int y);
protected:
	// Driver
	Screen *(*InitPtr)(void *driverHandlerPtr);
	void (*IdlePtr)(void *driverHandlerPtr);
	void (*TriggerRefreshPtr)(void *driverHandlerPtr);
	void (*TriggerUpdatePtr)(void *driverHandlerPtr);
	int (*GetXPtr)(void *driverHandlerPtr);
	int (*GetYPtr)(void *driverHandlerPtr);
	Screen *(*DrvRefreshPtr)(void *driverHandlerPtr);
	Screen *(*DrvOnPtr)(void *driverHandlerPtr, bool state);
	Screen *(*DrvSetContrastPtr)(void *driverHandlerPtr, byte cont);
	Screen *(*DrvDrawPixelPtr)(void *driverHandlerPtr, int x, int y, int color);
	Screen *(*DrvDrawPixelClipPtr)(void *driverHandlerPtr, struct box_s *box, int x, int y, int color);
	Screen *(*DrvDrawRectanglePtr)(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color);
	Screen *(*DrvDrawRectangleClipPtr)(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color);
	Screen *(*DrvDrawHLinePtr)(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color);
	Screen *(*DrvDrawHLineClipPtr)(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color);
	Screen *(*DrvDrawVLinePtr)(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color);
	Screen *(*DrvDrawVLineClipPtr)(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color);
	Screen *(*DrvClearPtr)(void *driverHandlerPtr, int color);
	Screen *(*DrvDrawStringPtr)(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor);
	Screen *(*DrvDrawStringClipPtr)(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor);
    void *DriverPtr;
	uint8_t CsPin;
	uint8_t DcPin;
	uint8_t RstPin;
	uint8_t VccEnPin;
	uint8_t PModEnPin;
	uint8_t Busy;
};

class gfxString{
public:
    gfxString();
	int getRowsInBox4x6();
	int getRowsInBox4x6(struct box_s *box);
	int getColsInBox4x6();
	int getColsInBox4x6(struct box_s *box);
    gfxString *drawStringWindowed4x6(char *string, int16_t x, int16_t y, int16_t cursorPos = -1, bool cursorState = false, int16_t endCursorPos = -1);
    gfxString *drawStringWindowed4x6(struct box_s *box, char *string, int16_t x, int16_t y, int16_t cursorPos = -1, bool cursorState = false, int16_t endCursorPos = -1);
	int getRowsInBox6x8();
	int getRowsInBox6x8(struct box_s *box);
	int getColsInBox6x8();
	int getColsInBox6x8(struct box_s *box);
    gfxString *drawStringWindowed6x8(char *string, int16_t x, int16_t y, int16_t cursorPos = -1, bool cursorState = false);
    gfxString *drawStringWindowed6x8(struct box_s *box, char *string, int16_t x, int16_t y, int16_t cursorPos = -1, bool cursorState = false);

	gfxString *setEdgeTouch(unsigned char edgeTouch) {
		this->edgeTouch = edgeTouch;
		return this;
	}
	gfxString *setWordWrap(bool wordWrap) {
		this->wordWrap = wordWrap;
		return this;
	}
	gfxString *setTerminalMode(bool mode) {
		this->terminalMode = mode;
		return this;
	}
	gfxString *setForeColor(int color) {
		this->foreColor = color;
		return this;
	}
	gfxString *setInkColor(int color) {
		this->inkColor = color;
		return this;
	}
	gfxString *setMaxLineLen(unsigned int len) {
		this->maxLineLen = len;
		return this;
	}
	gfxString *setMaxLen(unsigned int len) {
		this->maxLen = len;
		return this;
	}
    gfxString *setTabSpaces(unsigned char spaces) {
        this->tabSpaces = spaces;
        return this;
    }
    gfxString *setTransparent(bool transparent) {
        this->transparent = transparent;
        return this;
    }
    gfxString *setExtraSpace(unsigned char extraSpace) {
        this->extraSpace = extraSpace;
        return this;
    }

    unsigned char getEdgeTouch() { return edgeTouch;}
    bool getWordWrap() { return wordWrap;}
    bool getTerminalMode() { return terminalMode;}
    int getForeColor() { return foreColor;}
    int getInkColor() { return inkColor;}
    unsigned int getMaxLineLen() { return maxLineLen;}
    unsigned int getMaxLen() { return maxLen;}
    unsigned char getTabSpaces() { return tabSpaces;}
    bool getTransparent() { return transparent;}
    unsigned int getRowCnt() { return rowCnt;}
    unsigned int getRowLenPix() { return rowLenPix; }

	enum {
	    EDGE_OUTSIDE_UP = 0x1,
	    EDGE_OUTSIDE_DOWN = 0x2,
	    EDGE_OUTSIDE_LEFT = 0x4,
	    EDGE_OUTSIDE_RIGHT = 0x8,
    }edgeOutside_e;

private:
    unsigned char edgeTouch;
    bool wordWrap;
    bool terminalMode;
    int foreColor;
    int inkColor;
    unsigned int maxLineLen;
    unsigned int maxLen;
    unsigned char tabSpaces;
    bool transparent;
    unsigned int rowCnt;
    int16_t rowLenPix;
    unsigned char extraSpace;

};

extern Screen *defaultScreen;


#endif /* LIBRARIES_ARDUINO_SDK_DEV_SCREEN_H_ */
