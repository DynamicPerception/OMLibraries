/*
 * BusMaster.cpp
 *
 *  Created on: 17.10.2012
 *      Author: perepelitsa
 */

#include "BusMaster.h"

BusMaster::BusMaster(HardwareSerial& c_serObj, uint8_t c_dePin)
:OMMoCoMaster(c_serObj, c_dePin)
{
	cur_addr = 0;
}

BusMaster::~BusMaster() {
	// TODO Auto-generated destructor stub
}

/**
 *
 * */
void BusMaster::SelectNode(uint8_t idx)
{
	cur_addr = getNodeAddr(idx);
}
/**
 *
 * */
void BusMaster::StepMotor(uint8_t dir)
{
    cmdMoveMotor(cur_addr,dir,1);
}
/**
 * Takes up to 6 first
 *  - Send request for device identifier to each address on bus (2-255)
 *  - Match any device, and save that has device id of OMAXISVX
 *  - List Devices by Address, send Debug Enable when device is shown, debug disable when device is not shown
 *  -- Debug LED will show which device is displayed on-screen
 * */
void BusMaster::ScanDevices()
{
	uint8_t bus_addr = 2;
	resetList();
	while (getListCount() < getMaxCount())
	{
     char* p_name = getId(bus_addr);
     if (p_name != 0) {
    	 if (strcmp(p_name, "OMAXISVX") == 0) {
    		 //add to list
    		 addToList(bus_addr, p_name);
    		 //send DebugLED Enable
    		 cmdDebugLed(bus_addr,1);
    	 } else {
    		 //send DebugLED Disable
    		 cmdDebugLed(bus_addr,0);
    	 }
     }
     if (bus_addr == 255) break;
     bus_addr++;
  }
  // last entry always "NEXT"
}

/**
 *
 * */
void BusMaster::SetupMovie()
{
 // if (isCameraControl()) {
 //   uint8_t node = findControlNode();
//	SetupCameraControlNode(node);

//  } else {

  //}

}
