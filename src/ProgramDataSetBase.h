/*
 * ProgramDataSet.h
 *
 *  Created on: 13.04.2012
 *      Author: perepelitsa
 */

#ifndef PROGRAMDATASET_H_
#define PROGRAMDATASET_H_

#include <inttypes.h>

class ProgramDataSetBase {
public:
	ProgramDataSetBase();
	virtual ~ProgramDataSetBase();

	bool dispatch(uint8_t command, uint8_t sub, uint8_t** mailbox) const;

protected:
	virtual bool cdInterval(unsigned short interval) = 0;
	virtual bool cdExposure(unsigned long exposure) = 0;
	virtual bool cdFocus(unsigned short focus) = 0;
	virtual bool cdMaxShots(unsigned short count) = 0;
	virtual bool cdExposureDelay(unsigned short delay) = 0;
	virtual bool cdFocusWShutter(bool isOn) = 0;
	virtual bool cdRepeatCycles(unsigned char count) = 0;
	virtual bool mdStepsEachInterval(unsigned short steps) = 0;
	virtual bool mdNOOP(void) = 0;
	virtual bool mdDirection(bool dir) = 0;
	virtual bool mdMaximumTravelSteps(unsigned long steps) = 0;
	virtual bool mdMotorEnable(void) = 0;
	virtual bool mdMotorDisable(void) = 0;
	virtual bool mdSetHomePositionHere(void) = 0;
	virtual bool mdBackslashAmount(unsigned char steps) = 0;
	virtual bool mdEasingMode(unsigned char mode) = 0;
	virtual bool mdContinuousMode(bool isOn) = 0;

};

#endif /* PROGRAMDATASET_H_ */
