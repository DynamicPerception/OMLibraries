/*
 * Commander.h
 *
 *  Created on: 17.10.2012
 *      Author: perepelitsa
 */

#ifndef COMMANDER_H_
#define COMMANDER_H_

#include <inttypes.h>
#include "MenuContext.h"
//constants for StepMotor
#define STEP_LEFT 0
#define STEP_RIGHT 1
#define SET_HOME 3



/**
 * Commander Interface class.
 **/
class Commander {

	MenuContext status;
public:
	Commander();
	virtual ~Commander();

	const MenuContext& GetContext (void)const {return status;}
	MenuContext& GetWrContext (void) {return status;}

	//list interface wrapper
	uint8_t getListCount(void){return status.getListCount();}
	uint8_t getMaxCount(void){return status.getMaxCount();}
	uint8_t getNodeAddr(uint8_t idx){return status.getNodeAddr(idx);}
	void resetList(void){status.resetList();}
	void addToList(uint8_t bus_addr, const char *p_name) {status.addToList(bus_addr, p_name);}

	//ToDo provide real code
	//interface wrapper
	bool isCameraControl(void) {return false;}

    /*Serves motor steps in initial position setup*/
	virtual void StepMotor(uint8_t _dir) = 0;
	/* */
	virtual void ScanDevices() = 0;
	/* */
	virtual void SetupMovie() = 0;
};

#endif /* COMMANDER_H_ */
