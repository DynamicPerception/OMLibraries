/*
 * ProgramControl.h
 *
 *  Created on: 13.04.2012
 *      Author: perepelitsa
 */

#ifndef PROGRAMCONTROLBASE_H_
#define PROGRAMCONTROLBASE_H_

#include <inttypes.h>

// core bus protocol command values
#define OM_SER_BASECOM		1

#define OM_SER_COREPROTO	1
#define OM_SER_COREID		2
#define OM_SER_COREVER		3
#define OM_SER_COREADDR		4

class ProgramControlBase {
public:
	ProgramControlBase();
	virtual ~ProgramControlBase();

	bool dispatch(uint8_t cmd, uint8_t** mailbox) const;

private:
	virtual bool nop() = 0;
	virtual bool start() = 0;
	virtual bool pause() = 0;
	virtual bool cameraEnable() = 0;
	virtual bool cameraDisable() = 0;
	virtual bool moveMotorSimple(bool dir, unsigned short steps) = 0;
	virtual bool sendMotorHome() = 0;
	virtual bool exposeNow(unsigned long exposure) = 0;
	virtual bool motorSleep(bool isSleep) = 0;
	virtual bool motorMaxStepRate(unsigned short stepsPerSecond) = 0;
	virtual bool motorMaxStepSpeed(unsigned short stepsPerSecond) = 0;
	virtual bool nop1() = 0;
	virtual bool nop2() = 0;
	virtual bool nop3() = 0;
	virtual bool debugLed(bool isOn) = 0;
	virtual bool stopMotorNow() = 0;
	virtual bool microStepLevel(unsigned char level) = 0;
	virtual bool timingMaster(bool isMaster) = 0;
	virtual bool moveMotorComplex(bool dir, unsigned long steps,
			unsigned long totalTravelTime, unsigned long accelTime,
			unsigned long decelTime) = 0;
	virtual bool planInterleavedTravel(bool dir, unsigned long steps,
			unsigned long totalTravelIntervals, unsigned long accelIntervals,
			unsigned long decelIntervals) = 0;
	virtual bool cleanPlannedTravel() = 0;
	virtual bool statusRequest(unsigned char statusType,
			unsigned char* blob) = 0;
};

#endif /* PROGRAMCONTROL_H_ */
