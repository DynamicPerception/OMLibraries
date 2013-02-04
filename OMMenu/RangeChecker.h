/*
 * RangeChecker.h
 *
 *  Created on: 04.02.2013
 *      Author: perepelitsa
 */

#ifndef RANGECHECKER_H_
#define RANGECHECKER_H_

#include <inttypes.h>

#define PARAMS_COUNT 32

class RangeChecker {
	const static int32_t iParamMaxValue[PARAMS_COUNT];
	const static int16_t iParamMinValue[PARAMS_COUNT];
public:
	static uint8_t checkParamByIdx(const uint8_t idx, uint32_t* pParam);
	static uint8_t checkParamRange(uint32_t min, uint32_t max, uint32_t* pParam);
};

#endif /* RANGECHECKER_H_ */
