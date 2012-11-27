/*

MoCoBus Core Library Definitions

OpenMoco nanoMoCo Core Engine Libraries 

See www.openmoco.org for more information

(c) 2011-2012 C.A. Church / Dynamic Perception LLC

    With contributions by Stanislav Pereplitsa

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef OMMOCODEFS_H_
#define OMMOCODEFS_H_


// types of data responses which can be sent back
enum {
	R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STR
};


#ifndef OMB_DEPIN
    #define OMB_DEPIN 5
#endif
#ifndef OMB_DEREG
    #define OMB_DEREG   PORTD
#endif
#ifndef OMB_DEPFLAG
    #define OMB_DEPFLAG PORTD5
#endif

#ifndef OMB_SRDREG
    #define OMB_SRDREG  UDR0
#endif
#ifndef OMB_SRRFLAG
    #define OMB_SRRFLAG UDRE0
#endif
#ifndef OMB_SRREG
    #define OMB_SRSREG   UCSR0A
#endif
#ifndef OMB_SRCLRFLAG
    #define OMB_SRTXFLAG   TXC0
#endif



// bus 'master' address, for responses
#define OM_SER_MASTER_ADDR	0
// bus 'broadcast' address
#define OM_SER_BCAST_ADDR	1

 // timeout in milliseconds
#define OM_SER_MASTER_TIMEOUT  100

// maximum time in ms between serial bytes
#define OM_SER_WAIT 100
// command data buffer size in bytes
#define OM_SER_BUFLEN 32

// return codes
#define OM_SER_OK 1
#define OM_SER_TIMEOUT 2
#define OM_SER_ERR 3
#define OM_SER_NOT_US 4
#define OM_SER_IS_BCAST 5

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

// commond broadcast commands
#define OM_BCAST_START		1
#define OM_BCAST_STOP		2
#define OM_BCAST_PAUSE		3


#endif /* OMMOCODEFS_H_ */
