/*
 * OMMoCoNode.h
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#ifndef OMMOCONODE_H_
#define OMMOCONODE_H_

#include "OMMoCoBus.h"
#include "OMMoCoTransceiver.h"
#include "ProgramControlBase.h"
#include "ProgramDataSetBase.h"

class OMMoCoNode: public OMMoCoBus, public OMMoCoTransceiver {
	enum
	{
	    STATE_ENABLED,
	    STATE_DISABLED,
	    STATE_NOT_INITIALIZED
	} eOMState;


	//SerialPort* port;

	const ProgramControlBase* programControl;
	const ProgramDataSetBase* programDataSet;

	uint8_t ucMBAddress;

	uint8_t   *ucOMFrame;
	uint8_t    ucRcvAddress;
	uint8_t    ucFunctionCode;
	unsigned short   usLength;

public:
	OMMoCoNode(const ProgramControlBase& pc, const ProgramDataSetBase& pds);
	virtual ~OMMoCoNode();
	virtual char Close( void );
	virtual char Enable(void);
	virtual char Disable(void);
	virtual char Poll(void);
	virtual char Init(uint8_t ucBusAddress, unsigned long usPortBaud);

private:
	void process(uint8_t function, uint8_t ** pucFrame, unsigned short * pusLength);

};

#endif /* OMMOCONODE_H_ */
