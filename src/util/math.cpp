/*
 * util_math.cpp
 *
 *  Created on: Apr 3, 2022
 *      Author: cry
 */

#include "math.h"

/*
 * This function convert a Value between MinValue and MaxValue to a fit into a value from 0 to MaxPercentageValue.
 */
long math::intToPercent(long maxPercentageValue, long minValue, long maxValue, long value) {
	int ReturnedValue = 0;
	if(value > maxValue)
		value = maxValue;
	else if(value < minValue)
		value = minValue;
	if (maxValue < 65536)
		ReturnedValue = ((value - minValue) * 0x10000) / (((maxValue - minValue) * 0x10000) / maxPercentageValue);
	else
		ReturnedValue = (value - minValue) / ((maxValue - minValue) / maxPercentageValue);
	if(ReturnedValue > maxPercentageValue)
		ReturnedValue = maxPercentageValue;
	else if(ReturnedValue < 0)
		ReturnedValue = 0;
	return ReturnedValue;
}

/*
 * This function convert a Value between MinValue and MaxValue to a fit into a value from 0 to MaxPercentageValue.
 */
unsigned long math::uintToPercent(unsigned long maxPercentageValue, unsigned long minValue, unsigned long maxValue, unsigned long value) {
	unsigned long ReturnedValue = 0;
	if (maxValue < 65536)
		ReturnedValue = ((value - minValue) * 0x10000) / (((maxValue - minValue) * 0x10000) / maxPercentageValue);
	else
		ReturnedValue = (value - minValue) / ((maxValue-minValue) / maxPercentageValue);
	if(ReturnedValue > maxPercentageValue)
		ReturnedValue = maxPercentageValue;
	else if(ReturnedValue < 0)
		ReturnedValue = 0;
	return ReturnedValue;
}

/*
 * This function convert a Value between MinValue and MaxValue to a fit into a value from 0 to MaxPercentageValue.
 */
double math::doubleToPercent(double maxPercentageValue, double minValue, double maxValue, double value) {
	double ReturnedValue = 0;
	ReturnedValue = (value- minValue) / ((maxValue-minValue) / maxPercentageValue);
	if(ReturnedValue > maxPercentageValue)
		ReturnedValue = maxPercentageValue;
	else if(ReturnedValue < 0)
		ReturnedValue = 0;
	return ReturnedValue;
}


/*
 * This function convert a Value between MinValue and MaxValue to a fit into a value from 0 to MaxPercentageValue with decimals.
 * At this moment does not return decimals :)
 */
math::ToPercentageWithDecimals_t math::intToPercentDecimal(long maxPercentageValue, long minValue, long maxValue, long value) {
	ToPercentageWithDecimals_t Return;
	Return.Decimal = 0;
	if (maxValue < 65536) { // Increase the precision of the result.
		Return.Value = (value * 0x10000) / (((maxValue - minValue) * 0x10000) / maxPercentageValue);
	} else {
		Return.Value = value / ((maxValue-minValue) / maxPercentageValue);
	}
	return Return;
}

/*
 * This function convert a Value between 0 and MaxPercentageValue to a value between MinValue and MaxValue.
 */
long math::percentToInt(long minValue, long maxValue, long maxPercentageValue, long value) {
	long result = (signed long long)((value * (maxValue - minValue)) / maxPercentageValue) + minValue;
	if(result > maxValue)
		result = maxValue;
	else if(result < minValue)
		result = minValue;
	return result;
}

/*
 * This function convert a Value between 0 and MaxPercentageValue to a value between MinValue and MaxValue.
 */
double math::percentToDouble(double minValue, double maxValue, double maxPercentageValue, double value) {
	double result = ((value * (maxValue - minValue)) / maxPercentageValue) + minValue;
	if(result > maxValue)
		result = maxValue;
	else if(result < minValue)
		result = minValue;
	return result;
}


