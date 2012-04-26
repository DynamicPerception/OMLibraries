/*
 * OMMoCoDefs.h
 *
 *  Created on: 24.04.2012
 *      Author: perepelitsa
 */

#ifndef OMMOCODEFS_H_
#define OMMOCODEFS_H_


// types of data responses which can be sent back
enum {
	R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STR
};

// bus 'master' address, for responses
#define OM_SER_MASTER_ADDR	0

 // timeout in milliseconds
#define OM_SER_MASTER_TIMEOUT  200

// maximum time in ms between serial bytes
#define OM_SER_WAIT 100
// command data buffer size in bytes
#define OM_SER_BUFLEN 32

// return codes
#define OM_SER_OK 1
#define OM_SER_TIMEOUT 2
#define OM_SER_ERR 3
#define OM_SER_NOT_US 4

// serial bits per second on the bus
// 57.6k is the maximum bit rate without risking
// the high probability of framing errors
// this number must be a floating point!

#define OM_SER_BPS 57600.0

// serial clear time in uS (how long it takes to punch one bit
// out through the RS485 transceiver at the current bitrate).
// the transceiver adds 0.07uS delay in punching out a bit

#define OM_SER_CLEAR_TM 1000000.0 / OM_SER_BPS + 0.07

// bus protocol core version
#define OM_SER_VER	1

// bus 'master' address, for responses
#define OM_SER_MASTER	0

// core bus protocol command values
#define OM_SER_BASECOM		1

#define OM_SER_COREPROTO	1
#define OM_SER_COREID		2
#define OM_SER_COREVER		3
#define OM_SER_COREADDR		4


#endif /* OMMOCODEFS_H_ */
