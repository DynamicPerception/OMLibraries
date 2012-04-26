/*
 * OMMoCoNode.cpp
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#include "OMMoCoNode.h"

//declarative constructor easier to implement
OMMoCoNode::OMMoCoNode(const ProgramControlBase& pc,
		const ProgramDataSetBase& pds) :
		programControl(&pc), programDataSet(&pds) {
}

OMMoCoNode::~OMMoCoNode() {
}

char OMMoCoNode::Init(uint8_t ucSlaveAddress, unsigned long ulBaudRate) {
	eOMState = STATE_NOT_INITIALIZED;
	char eStatus = OM_NOERR;

	/* check preconditions */
	if ((ucSlaveAddress < OM_ADDRESS_MIN)
			|| (ucSlaveAddress > OM_ADDRESS_MAX)) {
		eStatus = OM_INVALID;
	} else {
		ucMBAddress = ucSlaveAddress;
		eStatus = OMMoCoTransceiver::Init(0, ucMBAddress, ulBaudRate);

		if (eStatus == OM_NOERR) {
			if (!PortEventInit()) {
				/* port dependent event module initialization failed. */
				eStatus = OM_HARDERR;
			} else {
				eOMState = STATE_DISABLED;
			}
		}
	}
	return eStatus;
}

/**
 *
 * */
char OMMoCoNode::Close(void) {
	char eStatus = OM_NOERR;

	if (eOMState == STATE_DISABLED) {
		getPort()->closePort();
	} else {
		eStatus = OM_INVALID;
	}
	return eStatus;
}

/**
 *
 * */
char OMMoCoNode::Enable(void) {
	char eStatus = OM_NOERR;

	if (eOMState == STATE_DISABLED) {
		/* Activate the protocol stack. */
		StartTransceiver();
		eOMState = STATE_ENABLED;
	} else {
		eStatus = OM_INVALID;
	}
	return eStatus;
}

/**
 *
 * */
char OMMoCoNode::Disable(void) {
	char eStatus;

	if (eOMState == STATE_ENABLED) {
		StopTransceiver();
		eOMState = STATE_DISABLED;
		eStatus = OM_NOERR;
	} else if (eOMState == STATE_DISABLED) {
		eStatus = OM_NOERR;
	} else {
		eStatus = OM_INVALID;
	}
	return eStatus;
}

/**
 * All in one function
 * */
char OMMoCoNode::Poll(void) {
	char eStatus = OM_NOERR;
	eOMEventType eEvent;

	/* Check if the protocol stack is ready. */
	if (eOMState != STATE_ENABLED) {
		return OM_INVALID;
	}

	/* Check if there is a event available. If not return control to caller.
	 * Otherwise we will handle the event. */
	if (PortEventGet(&eEvent) == true) {
		switch (eEvent) {
		case OM_READY:
			break;

		case OM_FRAME_RECEIVED:
			eStatus = Receive(&ucRcvAddress, &ucOMFrame, &usLength);
			if (eStatus == OM_NOERR) {
				/* Check if the frame is for us. If not ignore the frame. */
				if ((ucRcvAddress == ucMBAddress)) {
					(void) PortEventPost(OM_EXECUTE);
				}
			}
			break;

		case OM_EXECUTE:
			ucFunctionCode = ucOMFrame[OM_PDU_FUNC_OFF];

			process(ucFunctionCode, &ucOMFrame, &usLength);

			eStatus = Send(ucMBAddress, ucOMFrame, usLength);

			break;

		case OM_FRAME_SENT:
			break;
		}
	}
	return OM_NOERR;
}

/**
 * N.B. decide about indexing
 *
 * */
void OMMoCoNode::process(uint8_t function, uint8_t ** pucFrame,
		unsigned short * pusLength) {
	bool res;
	ByteBuffer b;
	if (function == OM_PROGRAM_CONTROL_CODE) {
		uint8_t command = *pucFrame[0];
		uint8_t* mailbox = pucFrame[1];
		res = programControl->dispatch(command, &mailbox);
	} else if (function == OM_PROGRAM_DATA_SET_CODE) {
		uint8_t command = *pucFrame[0];
		uint8_t sub = *pucFrame[1];
		uint8_t* mailbox = pucFrame[3];
		res = programDataSet->dispatch(command, sub, &mailbox);
	} else if (function == OM_BASECOM_CODE) {
		uint8_t command = *pucFrame[0];
		b.assign(pucFrame[3], 20);
		_coreProtocol(command, b);

	} else {

	}

}

/**
 * // handle core protocol commands on behalf of the device
 *
 * */
void OMMoCoNode::_coreProtocol(uint8_t subCom, ByteBuffer& b) {

	switch (subCom) {
	case OM_SER_COREPROTO:
		// core protocol version supported
		//response(true, (unsigned int) OM_SER_VER);
		break;
	case OM_SER_COREID:
		// device identifier
		//response(true, id(), 8);
		break;
	case OM_SER_COREVER:
		// device version
		//response(true, m_ver);
		break;
	case OM_SER_COREADDR:
		// change node addr
		//address(p_buf[1]);
		//response(true);
		break;
	default:
		// error
		//response(false);
		break;
	}
}
