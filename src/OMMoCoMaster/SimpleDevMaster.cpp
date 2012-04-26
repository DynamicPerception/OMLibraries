/*
 * SimpleDevMaster.cpp
 *
 *  Created on: 25.04.2012
 *      Author: perepelitsa
 */

#include "SimpleDevMaster.h"

extern SimpleDevMaster g_simpleDevMaster;

SimpleDevMaster::SimpleDevMaster(HardwareSerial& c_serObj, uint8_t c_dePin)
 : OMMoCoMaster(c_serObj, c_dePin)
{
}

char SimpleDevMaster::Init(uint8_t ucBusAddress, uint32_t portBaud)
{
	//calculate
	serObj.begin(portBaud);
	//call init

	OMMoCoMaster::Init(serObj, 0);
	return 0;
}


	//virtual char Close(void);
	//virtual char Enable(void);
	//virtual char Disable(void);
	//virtual char Poll(void);

SimpleDevMaster::~SimpleDevMaster() {
}

