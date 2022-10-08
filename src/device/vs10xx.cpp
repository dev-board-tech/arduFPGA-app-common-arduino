/*
 * vs10xx.c
 *
 * Created: 19.06.2020 21:33:26
 *  Author: MorgothCreator
 */ 

#include "vs10xx.h"

#include <stdint.h>

void vs10xx::assertRst() {
	digitalWrite(RstPin, LOW);
}

void vs10xx::deassertRst() {
	digitalWrite(RstPin, HIGH);
}

void vs10xx::assertDcs() {
	digitalWrite(DcsPin, LOW);
}

void vs10xx::deassertDcs() {
	digitalWrite(DcsPin, HIGH);
}

void vs10xx::assertCs() {
	digitalWrite(CsPin, LOW);
}

void vs10xx::deassertCs() {
	digitalWrite(CsPin, HIGH);
}

void vs10xx::checkBusy() {
	while(~digitalRead(DreqPin));
}

bool vs10xx::checkBusySkip() {
	if(~digitalRead(DreqPin))
		return true;
	else
		return false;
}

vs10xx::vs10xx(uint8_t csPin, uint8_t dcsPin, uint8_t rstPin, uint8_t dreqPin) {
	CsPin = csPin;
	DcsPin = dcsPin;
	RstPin = rstPin;
	DreqPin = dreqPin;
	spi = &SPI;
	digitalWrite(CsPin, HIGH);
	digitalWrite(DcsPin, HIGH);
	digitalWrite(RstPin, HIGH);
	digitalWrite(DreqPin, HIGH);
	pinMode(CsPin, OUTPUT);
	pinMode(DcsPin, OUTPUT);
	pinMode(RstPin, OUTPUT);
	pinMode(DreqPin, INPUT);
	hardReset();
}

vs10xx::~vs10xx() {

}

void vs10xx::transfer(uint8_t *buf, int len) {
	for( int i = 0; i < len; i++) {
		spi->transfer(buf[i]);
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
	spi->transfer(VS_INS_READ);
	spi->transfer(reg);
	value = (spi->transfer(0xFF) << 8) | spi->transfer(0xFF);
	deassertCs();
	return value;
}
#define VS1011
void vs10xx::softReset() {
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
}

void vs10xx::hardReset() {
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
	for(nullCount = 0; nullCount < len; nullCount++)
		spi->transfer(255);
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
	if(checkBusySkip()) {
		spi->transfer(255);
		return true;
	}
	return false;
}
