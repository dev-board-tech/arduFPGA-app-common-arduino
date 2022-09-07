/*
 * util_math.h
 *
 *  Created on: Apr 3, 2022
 *      Author: cry
 */

#ifndef __LIBRARIES_ARDUINO_SDK_UTIL_MATH_H__
#define __LIBRARIES_ARDUINO_SDK_UTIL_MATH_H__

class math {
public:
	typedef struct ToPercentageWithDecimals_s
	{
		long Value;
		char Decimal;
	}ToPercentageWithDecimals_t;

	static long intToPercent(long maxPercentageValue, long minValue, long maxValue, long value);
	static unsigned long uintToPercent(unsigned long maxPercentageValue, unsigned long minValue, unsigned long maxValue, unsigned long value);
	static double doubleToPercent(double maxPercentageValue, double minValue, double maxValue, double value);
	static ToPercentageWithDecimals_t intToPercentDecimal(long maxPercentageValue, long minValue, long maxValue, long value);
	static long percentToInt(long minValue, long maxValue, long maxPercentageValue, long value);
	static double percentToDouble(double minValue, double maxValue, double maxPercentageValue, double value);

};


#endif /* LIBRARIES_ARDUINO_SDK_UTIL_MATH_H_ */
