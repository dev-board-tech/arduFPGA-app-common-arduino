/*
 * wpdw21.h
 *
 *  Created on: Aug 31, 2022
 *      Author: cry
 */

#ifndef LIBRARIES_ARDUINO_SDK_SRC_DRIVER_WPDW21_SPI_H_
#define LIBRARIES_ARDUINO_SDK_SRC_DRIVER_WPDW21_SPI_H_

#include "../device/screen.h"
#if defined(QT_WIDGETS_LIB)
typedef char SPIClass;
#endif

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
	static void TriggerUpdate(void *driverHandlerPtr);
	static int GetX(void *driverHandlerPtr);
	static int GetY(void *driverHandlerPtr);
	static Screen *DrvRefresh(void *driverHandlerPtr);
	static Screen *DrvOn(void *driverHandlerPtr, bool state);
	static Screen *DrvSetContrast(void *driverHandlerPtr, byte cont);
	static Screen *DrvDrawPixel(void *driverHandlerPtr, int x, int y, int color);
	static Screen *DrvDrawPixelClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int color);
	static Screen *DrvDrawRectangle(void *driverHandlerPtr, int x, int y, int x_size, int y_size, bool fill, int color);
	static Screen *DrvDrawRectangleClip(void *driverHandlerPtr, struct box_s *box, int x, int y, int x_size, int y_size, bool fill, int color);
	static Screen *DrvDrawHLine(void *driverHandlerPtr, int x1, int x2, int y, byte width, int color);
	static Screen *DrvDrawHLineClip(void *driverHandlerPtr, struct box_s *box, int x1, int x2, int y, byte width, int color);
	static Screen *DrvDrawVLine(void *driverHandlerPtr, int y1, int y2, int x, byte width, int color);
	static Screen *DrvDrawVLineClip(void *driverHandlerPtr, struct box_s *box, int y1, int y2, int x, byte width, int color);
	static Screen *DrvClear(void *driverHandlerPtr, int color);
	static Screen *DrvDrawString(void *driverHandlerPtr, char *string, int x, int y, int foreColor, int inkColor);
	static Screen *DrvDrawStringClip(void *driverHandlerPtr, struct box_s *box, char *string, int x, int y, bool terminalMode, bool wordWrap, int foreColor, int inkColor);

	enum {
		IDLE,
		PWR_ON_REFRESH,
		REFRESH_REFRESH,
		PWR_OFF_REFRESH,
		PWR_ON_REPAINT
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
	int repaintCnt;
	int refreshCnt;
};

#endif /* LIBRARIES_ARDUINO_SDK_SRC_DRIVER_WPDW21_SPI_H_ */
