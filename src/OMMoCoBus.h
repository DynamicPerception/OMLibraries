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
 // must load before wconstants to prevent issues
#include <math.h>


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

 // timeout in milliseconds
#define OM_SER_TIMEOUT  200

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

 // types of data responses which can be sent back

enum {
	R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STR
};


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

class OMMoCoBus {

public:
	OMMoCoBus();
    virtual ~OMMoCoBus();

	virtual char Init(uint8_t ucBusAddress, unsigned long portBaud ) = 0;
	virtual char Close( void ) = 0;
	virtual char Enable(void) = 0;
	virtual char Disable(void) = 0;
	virtual char Poll(void) = 0;

	static const uint8_t OM_ADDRESS_MIN = 2;
	static const uint8_t OM_ADDRESS_MAX = 254;
};


/**

 @page mocobus  The MoCoBus Protocol

 The MoCoBus protocol is designed for speaking to multiple devices connected
 to the same bus.  All communication is handled in a sequence of structured
 packets: master devices on the bus send commands, and node devices act and
 respond to them.

 While the MoCoBus AVR libraries are designed around RS-485 buses,
 nearly any transport layer could be supported that allows for multiple independent
 devices to exist on the same bus.  This protocol defines how commands and
 responses are issued.

 The MoCoBus protocol serves as a light-weight mechanism for
 speaking to and controlling multiple devices running simple microprocessors.
 It is intended to act as an in-between protocol where the complexities of
 a full-weight TCP/IP implementation are undesireable or impossible to support,
 yet the need for a robust transport with the ability to determine the successful
 receipt of messages is required.

 Like most protocols for use with directly connected devices, each device must
 have an address assigned to it.

 Much of this document will discuss the low-level implementation, including
 packet structure and timing.  This document serves as the technical documentation
 for the protocol its self, and those who wish to use the OpenMoCo AVR libraries
 will find that the OMMoCoMaster and OMMoCoNode classes abstract these complexities
 away and provide a simple interface via high-level methods.

 @section bustypes Types of Devices

 A bus is a series of devices all sharing a common set of communication wires.
 All devices on a bus can speak to each other - but only one at a time.  Most devices
 on a bus will be there to perform some specific activity, such as moving a motor. These
 devices, in most cases, will be dedicated to the task they perform, and not to
 providing a human interface - these are node devices.  They accept commands and
 react to them.

 Another type of device on the bus is one which provides a human interface, or
 acts on behalf of some other interface to command these node devices.  These are
 the master devices.

 As only one device may speak at a time, initiating commands is left to the
 master nodes.  Of course, this generally means only one master may be active at
 any given time.

 <center><img src="busmastnode.png"></center>

 Each node device must be assigned a unique address - master devices may only
 send commands and read responses, and therefore are all given the same
 address of 0.

 @section busproto The Basic Protocol

 The basic protocol uses structured packets to communicate information between
 nodes.  Each packet uses a predictable sequence of bytes that enables a microcontroller-based
 device to quickly determine if the packet is intended for it, or not.

 Every MoCoBus packet consists of up to two parts: the packet header, and the
 packet data (payload).  Every packet must have a header, at a minimum, but additional
 data beyond the header is not required.   The header is a ten-byte sequence and
 consists of four key sections: the break sequence, the target address, the packet
 code, and the data length.

 <center><img src="pktheader.png"></center>

 <b>The Break Sequence</b> is a series of bytes which indicates the start of a packet.
 This sequence is always defined as five null bytes followed by the value 255 (or,
 0xFF).  As each device on a bus will see all traffic destined to any device on
 the bus, the break sequence serves as an indicator that a packet has begun.  This
 allows a node that is powered on in the middle of a command to another node, for
 example, to quickly understand that it is missing part of the packet and to ignore
 any further data until a proper start sequence is read.  In this way, one may
 safely communicate with a device, no matter the current state of any other
 device on the bus.  <i>As the break sequence is a reserved sequence of bytes, it
 must never be repeated in the payload for a packet.  This is not of concern unless
 you are sending a sequence of raw bytes greater than 4-bytes in length.  In such cases
 one must take care to ensure that this sequence cannot be repeated - by padding
 multi-byte values, or using ascii strings for example.</i>

 <b>The Target Address</b> section is a two-byte value representing the address
 of the device which the packet is destined for. At this time, only the least
 significant byte of this value is utilized - limiting the address values between
 0 and 255. The additional byte is reserved for future address expansion, if needed.

 <b>Packet Code</b> is a special header section, it defines a particular code
 for the packet which represents a command or response value to the target of
 the packet.

 <b>The Data Length</b> section specifies how many additional bytes of data accompany
 the packet as data, or payload.  The maximum payload allowed per-packet is 32 bytes.
 If zero is specified for a packet, no additional data bytes may follow.


 <hr size=1 width="50%">

 There are two core types of packets which can be sent: command packets and
 response packets.

 <b>Command Packets</b> are sent by a master device to a node device.  They
 use the packet code to represent a primary command to the node.  They may also
 include up to 32 bytes of additional data in the payload, and there is no explicit
 structure to this payload (it is defined by the node and usually exposed to a
 master through an interface class).  A command code sent by a master must be
 greater than 0, and the command code 1 is reserved for core protocol commands
 that every node device must support, such as identifier, version, and bus protocol
 version.  This leaves 253 possible one-byte commands which can be sent to any
 node.  For any node device type requiring more commands, sub-commands may be
 sent as part of the data payload.  These packets are addressed to a specific
 device, and that devices address appears in the address section.  All other
 devices must accept, and then discard this packet.

 <b>Response Packets</b> are sent by a node device to a master's command packet.
 There are only two possible packet codes, in this case referred to as a <i>response
 code</i>, allowed: 0 for error, and 1 for success.  All node devices must begin
 transmitting their response to the master within 100mS, and each additional byte
 in the response must come within 100mS of a previous byte, otherwise the node
 will be timed out.

 All response packets must be addressed to the address 0, which is reserved for
 responses back to the master.

 If a node wishes to send data in response to the master's command, it must add a
 byte immediately following the payload length byte, which specifies which kind of
 data the node is responding with.  This enables masters to quickly and easily
 distinguish the response data, and enables nodes to offer different data responses
 based on system conditions without requiring additional hand-shaking complexity.
 Six key data types are available, all represented by constants.  For more information
 on these constants, see @ref masterdata "Receiving Data from the Node" in the
 Master tutorial. Note that the data type value byte must be included in the data
 length.  The following diagram shows a hypothetical command packet and response
 packet in a session between a node and master.

 <center><img src="resppkt.png"></center>

 */

#endif /* OMMOCOBUS_H_ */
