/*
 * wpdw21.h
 *
 *  Created on: Aug 31, 2022
 *      Author: cry
 */

#ifndef LIBRARIES_ARDUINO_SDK_SRC_DRIVER_WPDW21_SPI_H_
#define LIBRARIES_ARDUINO_SDK_SRC_DRIVER_WPDW21_SPI_H_

#include "../dev/screen.h"

class wpdW21_spi : public Screen {
public:
	wpdW21_spi(SPIClass *spi, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy);
	wpdW21_spi(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy);
	wpdW21_spi(SPIClass *spi, int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy);
	wpdW21_spi(int bufSize, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy);
#ifndef WDPW21_USE_NO_BUF
	wpdW21_spi(SPIClass *spi, void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy);
	wpdW21_spi(void *vBuf, uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t busy);
#endif
	~wpdW21_spi();

	void deriverInit();
	static Screen *Init(void *driverHandlerPtr);
	static void Idle(void *driverHandlerPtr);
	static void TriggerRefresh(void *driverHandlerPtr);
	static int GetX(void *driverHandlerPtr);
	static int GetY(void *driverHandlerPtr);
	static Screen *DrvRefresh(void *driverHandlerPtr);
	static Screen *DrvOn(void *driverHandlerPtr, bool state);
	static Screen *DrvSetContrast(void *driverHandlerPtr, byte cont);
	static Screen *DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color);
	static Screen *DrvDrawPixelBox(void *driverHandlerPtr, struct box_s *box, int x, int y, int color);
	static Screen *DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color);
	static Screen *DrvDrawRectangleBox(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color);
	static Screen *DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color);
	static Screen *DrvDrawHLineBox(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color);
	static Screen *DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color);
	static Screen *DrvDrawVLineBox(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color);
	static Screen *DrvClear(void *driverHandlerPtr, int color);
	static Screen *DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor);
	static Screen *DrvDrawStringBox(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor);

	enum {
		IDLE,
		PWR_ON,
		REFRESH,
		PWR_OFF
	}state;
	void full_lut_bw();
	void part_lut_bw();
	void lcd_chkstatus();
	bool lcd_chkstatus_nonblocking();
	void upData();
	void WrCmd(byte cmd);
	void WrData(byte data);
	byte backBuf[1280];
	bool initialized;
};

#endif /* LIBRARIES_ARDUINO_SDK_SRC_DRIVER_WPDW21_SPI_H_ */
