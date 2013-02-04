/*
 * RangeChecker.cpp
 *
 *  Created on: 04.02.2013
 *      Author: perepelitsa
 */

#include "RangeChecker.h"
#include <avr/pgmspace.h>

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))



const int32_t RangeChecker::iParamMaxValue[PARAMS_COUNT] PROGMEM = {
	0,16,16,(99*3600UL+59*60UL+59UL),1, //0-4
	1,99999,65535,65535,65535, //5-9
	7,(99*3600UL+59*60UL+59UL),0,0,1, //10 - 14
	2,3000,400,0x2459,50, //15-19
	2,1,1,7,1, //20-24
	1,100,100,0,0,0,0 //25-31
	};

const int16_t RangeChecker::iParamMinValue[PARAMS_COUNT] PROGMEM = {
		0,0,0,0,0, //0-4
		0,0,0,0,0, //5-9
		1,60,0,0,0, //10-14
		0,0,0,0,0, //15-19
		0,0,0,0,0, //20-24
		0,0,0,0,0,0,0 //25-31

	};

uint8_t RangeChecker::checkParamByIdx(const uint8_t idx, uint32_t* pParam)
{
	uint16_t paramMinValue;
	uint32_t paramMaxValue;
	//copy across FLASH space
	memcpy_P(&paramMinValue, &iParamMinValue[idx], sizeof(paramMinValue));
	memcpy_P(&paramMaxValue, &iParamMaxValue[idx], sizeof(paramMaxValue));

	return checkParamRange(paramMinValue, paramMaxValue, pParam);
}

/**
 *
 * */
uint8_t RangeChecker::checkParamRange(uint32_t min, uint32_t max, uint32_t* pParam)
{
	uint8_t result = 0;
	if (*pParam < min) {
			*pParam = min;
		} else if (*pParam > max) {
			*pParam = max;
		} else {
			result = 1;
		}
	return result;
}
