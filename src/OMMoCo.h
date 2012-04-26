/*
 * OMMoCoBus.h
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#ifndef OMMOCOBUS_H_
#define OMMOCOBUS_H_

#include <inttypes.h>
#include <stdbool.h>

#include <math.h>


/**

 @mainpage

 This set of libraries provides several key components to aid in easily and
 quickly developing motion control applications using the nanoMoCo controller
 or for other Arduino-based devices which wish to use the MoCoBus protocol for
 communicating.

 Additionally, some libraries will assist with other devices not using the MoCoBus
 protocol, but wanting to integrate with key signalling on a MoCoBus network.

 The following libraries are included:

 <h2>OMState</h2>
 The OMState class allows you to quickly and easily add a state engine to your
 firmware.  This class can be used with any arduino-based device to quickly and
 easily map states to actions and transition between them.

 <br/><br/>

 <h2>OMComHandler</h2>
 The OMComHandler class manages common-line relationships between devices on a MoCoBus
 network.

 <br/><br/>

 <h2>OMCamera</h2>
 The OMCamera class provides easy, non-blocking control of a camera connected to
 a nanoMoCo or other Arduino device via opto-couplers.  It provides a rich feature-set
 with simple workflow designed to grealty reduce the amount of code written.

 <br/><br/>

 <h2>OMMotor</h2>
 The OMMotor class provides easy, non-blocking stepper motor control of a motor
 connected to a nanoMoCo device, or other Arduino-based device with an attached
 sstep/direction stepper driver.  This class provides both continuous motion and
 motion with a specified number of steps, all without delay or blocking, designed
 to reduce the amount of code written for new applications.

 <br/><br/>

 <h2>OMMoCoBus</h2>
 The OMMoCoBus class provides the core functionality needed for implementing
 the MoCoBus protocol on RS-485 networks.

 <br/><br/>

 <h2>OMMoCoNode</h2>
 The OMMoCoNode class provides everything you need to quickly and easily implement
 a new node type on a MoCoBus network, requiring you only to write the code to
 handle your packet data.

 */

/**

 @brief
 Core MoCoBus Library

 The Core MoCoBus class provides all the basic capabilities needed for MoCoBus
 RS-485 masters and nodes using Hardware Serial output to a compatible RS-485
 driver.

 This class should generally not be used directly in a node or master, instead
 you should be using the appropriate related class, such as OMMoCoNode. The
 Core MoCoBus library is intended for those authoring node and master classes
 or thosse wishing to port to a different UART interface.

 Multiple OMMoCoBus objects may exist at the same time, as long as they use
 different hardware serial objects.

 <b>Please note: at this time, this library only supports the Atmega328p chipset
 running with the standard UART and with the DE_PIN wired to digital pin 5.  Due
 to optimizations for timing, this cannot be changed at the present moment.  Future
 versions may resolve this issue.</b>

 This library provides the core protocol handling methods necessary for masters
 or nodes on a MoCoBus network.

 @author C. A. Church

 (c) 2011 Dynamic Perception LLC

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

//status codes
#define OM_NOERR 0
#define OM_HARDERR -2
#define OM_INVALID -1
#define OM_EIO 1

class OMMoCo {

public:
	OMMoCo();
	virtual ~OMMoCo();

	virtual char Init(uint8_t ucBusAddress, uint32_t portBaud) = 0;
	virtual char Close(void) = 0;
	virtual char Enable(void) = 0;
	virtual char Disable(void) = 0;
	virtual char Poll(void) = 0;

	static const uint8_t OM_ADDRESS_MIN = 2;
	static const uint8_t OM_ADDRESS_MAX = 254;
	//very first byte of command
	static const uint8_t OM_BASECOM_CODE = 1;
	static const uint8_t OM_PROGRAM_CONTROL_CODE = 2;
	static const uint8_t OM_PROGRAM_DATA_SET_CODE = 3;
};



#endif /* OMMOCOBUS_H_ */
