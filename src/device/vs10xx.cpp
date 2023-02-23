/*
 * vs10xx.c
 *
 * Created: 19.06.2020 21:33:26
 *  Author: MorgothCreator
 */ 

#include "vs10xx.h"

#include <stdint.h>
#if defined(QT_WIDGETS_LIB)
#define LOW         0
#define HIGH        1
#endif

void vs10xx::assertRst() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(RstPin, LOW);
#endif
}

void vs10xx::deassertRst() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(RstPin, HIGH);
#endif
}

void vs10xx::assertDcs() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(DcsPin, LOW);
#endif
}

void vs10xx::deassertDcs() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(DcsPin, HIGH);
#endif
}

void vs10xx::assertCs() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(CsPin, LOW);
#endif
}

void vs10xx::deassertCs() {
#if !defined(QT_WIDGETS_LIB)
    digitalWrite(CsPin, HIGH);
#endif
}

void vs10xx::checkBusy() {
#if !defined(QT_WIDGETS_LIB)
    while(~digitalRead(DreqPin));
#endif
}

bool vs10xx::checkBusySkip() {
#if !defined(QT_WIDGETS_LIB)
    if(~digitalRead(DreqPin))
		return true;
	else
#endif
        return false;
}

vs10xx::vs10xx(uint8_t csPin, uint8_t dcsPin, uint8_t rstPin, uint8_t dreqPin) {
	CsPin = csPin;
	DcsPin = dcsPin;
	RstPin = rstPin;
	DreqPin = dreqPin;
#if !defined(QT_WIDGETS_LIB)
    spi = &SPI;
    digitalWrite(CsPin, HIGH);
	digitalWrite(DcsPin, HIGH);
	digitalWrite(RstPin, HIGH);
	digitalWrite(DreqPin, HIGH);
	pinMode(CsPin, OUTPUT);
	pinMode(DcsPin, OUTPUT);
	pinMode(RstPin, OUTPUT);
	pinMode(DreqPin, INPUT);
#endif
	hardReset();
}

vs10xx::~vs10xx() {

}

void vs10xx::transfer(uint8_t *buf, int len) {
	for( int i = 0; i < len; i++) {
#if !defined(QT_WIDGETS_LIB)
        spi->transfer(buf[i]);
#endif
	}
}

void vs10xx::regWrite(uint8_t reg, uint16_t value) {
	checkBusy();
	assertCs();
	uint8_t buffer[4];
	buffer[0] = VS_INS_WRITE;
	buffer[1] = reg;
	buffer[2] = value >> 8;
	buffer[3] = value;
	transfer(buffer, 4);
	//spi->transfer((uint8_t*)buffer, 4);
	deassertCs();
	checkBusy();
}

uint16_t vs10xx::regRead(unsigned char reg) {
	uint16_t value;
	checkBusy();
	assertCs();
#if !defined(QT_WIDGETS_LIB)
    spi->transfer(VS_INS_READ);
	spi->transfer(reg);
	value = (spi->transfer(0xFF) << 8) | spi->transfer(0xFF);
	deassertCs();
	return value;
#else
    return 0;
#endif
}
#define VS1011
void vs10xx::softReset() {
#if !defined(QT_WIDGETS_LIB)
    spi->setClockDivider(3);
#ifdef VS10XX_CRISTAL_FREQ
	vs10xx_set_pll(spi, VS10XX_CRISTAL_FREQ);
#else
	setPll(12288000);
#endif
	spi->setClockDivider(0);
	regRead(VS_SCI_STATUS);
	regWrite(VS_SCI_MODE, VS_SM_SDINEW | VS_SM_RESET);
	//vs10xx_reg_write(param, VS_SCI_CLOCKF, 0x2000);
    // Switch on the analog parts
	//vs10xx_reg_write(spi, VS_SCI_AUDATA, 44101); // 44.1kHz stereo
	// The next clocksetting allows SPI clocking at 5 MHz, 4 MHz is safe then.
	//vs10xx_reg_write(spi, VS_SCI_CLOCKF, 6 << 12); // Normal clock settings multiplyer 3.0 = 12.2 MHz
	delay(2);
#endif
}

void vs10xx::hardReset() {
#if !defined(QT_WIDGETS_LIB)
    spi->setClockDivider(3);
	assertRst();
	delay(1);
	deassertRst();
	delay(200);
#ifdef VS10XX_CRISTAL_FREQ
	vs10xx_set_pll(spi, VS10XX_CRISTAL_FREQ);
#else
	setPll(12288000);
#endif
	spi->setClockDivider(2);
	//*spi->spcr = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (0 << SPR0);
	//*spi->spsr |= (1<<SPI2X);
	regRead(VS_SCI_STATUS);
	regWrite(VS_SCI_MODE, VS_SM_SDINEW);
	delay(2);
#endif
}

void vs10xx::setPll(uint32_t qFreq) {
	regWrite(VS_SCI_CLOCKF, 0x8000 + (qFreq / 2000));
}

void vs10xx::setVolume(uint8_t r, uint8_t l) {
	regWrite(VS_SCI_VOL, r + (l << 8));
}

uint16_t vs10xx::getVolume() {
	return regRead(VS_SCI_VOL);
}

void vs10xx::setBas(uint8_t bl, uint8_t bc, uint8_t tl, uint8_t tc ) {
	regWrite(VS_SCI_BASS, ((bl & 0x000F) << 12) | ((bc & 0x000F) << 8) | ((tl & 0x000F) << 4) | (tc & 0x000F));// 0x7AFA
}

void vs10xx::sendCancel() {
	regWrite(VS_SCI_MODE, regRead(VS_SCI_MODE) | VS_SM_CANCEL);// 0x7AFA
}

void vs10xx::sendNull(uint16_t len) {
	checkBusy();
	assertDcs();
	uint16_t nullCount;
#if !defined(QT_WIDGETS_LIB)
    for(nullCount = 0; nullCount < len; nullCount++)
		spi->transfer(255);
#endif
	deassertDcs();
}

bool vs10xx::send32BData(uint8_t* buffer) {
	if(checkBusySkip())
		return false;
	assertDcs();
	transfer(buffer, 32);
	deassertDcs();
	return true;
}

bool vs10xx::send1byteData(uint8_t data) {
#if !defined(QT_WIDGETS_LIB)
    if(checkBusySkip()) {
		spi->transfer(255);
		return true;
	}
#endif
	return false;
}
