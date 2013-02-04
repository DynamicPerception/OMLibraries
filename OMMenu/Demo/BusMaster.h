/*
 * BusMaster.h
 *
 *  Created on: 17.10.2012
 *      Author: perepelitsa
 */

#ifndef BUSMASTER_H_
#define BUSMASTER_H_

#include "Commander.h"
#include "OMMoCoMaster.h"

#define CMD_DEBUG_LED 15
#define CMD_TIMING_MASTER 18
#define CMD_MOVE_MOTOR_SMPL 6


class BusMaster: public Commander, public OMMoCoMaster {
	//selected node addr in interactive mode
	uint8_t cur_addr;
public:
	BusMaster(HardwareSerial& c_serObj);
	virtual ~BusMaster();

	//
	virtual void SelectNode(uint8_t id);
	virtual void StepMotor(uint8_t dir);
	virtual void ScanDevices();
	virtual void SetupMovie();

protected:
	void cmdDebugLed(uint8_t bus_addr, uint8_t enable){command(bus_addr, CMD_DEBUG_LED, enable);}
	void cmdTimingMaster(uint8_t bus_addr, uint8_t enable){command(bus_addr, CMD_TIMING_MASTER, enable);}
	void cmdMoveMotor(uint8_t bus_addr, uint8_t dir, uint16_t steps){command(bus_addr, CMD_MOVE_MOTOR_SMPL, dir, steps);}

};

#endif /* BUSMASTER_H_ */
