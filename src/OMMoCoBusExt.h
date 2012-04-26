/*
 * OMMoCoBusExt.h
 *
 *  Created on: 24.04.2012
 *      Author: perepelitsa
 */

#ifndef OMMOCOBUSEXT_H_
#define OMMOCOBUSEXT_H_

//#include "OMMoCo.h"
#include "OMMoCoDefs.h"

/**!
 *
 * */
class OMMoCoBus {
public:
	OMMoCoBus(HardwareSerial& port, uint8_t, unsigned int mode);
	virtual ~OMMoCoBus();

	unsigned int address();
	void address(unsigned int);

	uint8_t* buffer();
	uint8_t bufferLen();

	void addressCallback(void(*)(uint8_t));
protected:

	uint8_t getPacket();

	// send to address, response code, data length
	void sendPacketHeader(unsigned int, uint8_t, uint8_t);

	// writing raw data (command or response contents)
	void write(uint8_t);
	void write(unsigned int);
	void write(int);
	void write(unsigned long);
	void write(long);

private:

	void(*f_newAddr)(uint8_t);

	uint8_t _getNextByte(uint8_t&);
	uint8_t _targetUs();
	void _flushSerial();

	HardwareSerial* m_serObj;

	uint8_t m_serBuffer[OM_SER_BUFLEN];
	unsigned int m_devAddr;
	uint8_t m_dePin;
	uint8_t m_bufSize;

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

#endif /* OMMOCOBUSEXT_H_ */
