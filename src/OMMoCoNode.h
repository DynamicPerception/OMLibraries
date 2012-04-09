/*
 * OMMoCoNode.h
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#ifndef OMMOCONODE_H_
#define OMMOCONODE_H_

#include "OMMoCoBus.h"
#include "OMMoCoTransiver.h"

class OMMoCoNode: public OMMoCoBus, public OMMoCoTransiver {
	enum
	{
	    STATE_ENABLED,
	    STATE_DISABLED,
	    STATE_NOT_INITIALIZED
	} eOMState;


	//SerialPort* port;
	uint8_t ucMBAddress;

	uint8_t   *ucOMFrame;
	uint8_t    ucRcvAddress;
	uint8_t    ucFunctionCode;
	unsigned short   usLength;

public:
	OMMoCoNode();//(const SerialPort& port);
	virtual ~OMMoCoNode();
	virtual char Close( void );
	virtual char Enable(void);
	virtual char Disable(void);
	virtual char Poll(void);
	virtual char Init(uint8_t ucBusAddress, unsigned long usPortBaud);

};

#endif /* OMMOCONODE_H_ */
