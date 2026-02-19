#ifndef __DTOSTRF_H__
#define __DTOSTRF_H__

#if defined(ARDUINO_ARCH_RP2040)
char * dtostrf(double number, signed char width, unsigned char prec, char *s);
#endif

#endif



