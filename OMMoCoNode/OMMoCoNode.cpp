#include "OMMoCoNode.h"


/** Constructor

 Constructs a new instance of the class. Accepts a hardware serial object,
 the digital pin # of the transceiver DE/RE pin combination, and the
 current device address.

 You may change the address later using address(), note that for nodes the
 address must be greater than 1.

 A textual identifier and version number is required for all nodes on a bus.
 These are used to differentiate types of nodes and capabilities of nodes of
 the same type running different versions of firmware. You must change the
 version number when changing what commands your device accepts or how it
 responds to commands so that others who are creating masters may know if
 your device supports the capabilities they expect.

 The textual identifier for a node must be exactly 8 characters long, and
 must consist only of the lower-ASCII character set (numeric value >= 32 and <= 126).
 If you attempt to use any character outside of this set, this library will
 automatically replace these characters with the character '0' (numeric value = 48).

 @param c_serObj
 An Arduino HardwareSerial object

 @param c_addr
 The device address

 @param c_ver
 The version of this specific device

 @param c_id
 A unique, 8-character description of this device that differentiates it
 from other devices on the bus

 */

OMMoCoNode::OMMoCoNode(Stream * c_serObj, unsigned int c_addr, unsigned int c_ver, char* c_id): OMMoCoBus(c_serObj, c_addr) {
	f_cmdHandler = 0;

	m_ver = c_ver;

	m_id = c_id;

		// replace out-of-range characters with "0" (48)
	for( uint8_t i = 0; i <= 7; i++ )
		c_id[i] = ( c_id[i] < 32 || c_id[i] > 126 ) ? 48 : c_id[i];

}

/** Send Response with no Data to Master

   @param p_stat
   The result code, true for success and false for error
   */

void OMMoCoNode::response(uint8_t p_stat) {
	//if (p_stat == 0)
	//	p_stat = 2;
		// 0 bytes returned to master (no data in response)
	sendPacketHeader(OM_SER_MASTER, p_stat, 0);
}

/** Send Response Data to Master

   @param p_stat
   The result code, true for success and false for error

   @param p_resp
  The response data to send
  */

void OMMoCoNode::response(uint8_t p_stat, uint8_t p_resp) {
	if (p_stat == 0)
		p_stat = 2;
		// 2 bytes returned to master (type + data size)
	sendPacketHeader(OM_SER_MASTER, p_stat, 2);

	this->write((uint8_t) R_BYTE);
	this->write(p_resp);
}

/** Send Response Data to Master

   @param p_stat
   The result code, true for success and false for error

  @param p_resp
  The response data to send
  */

void OMMoCoNode::response(uint8_t p_stat, unsigned int p_resp) {
	if (p_stat == 0)
		p_stat = 2;
		// 3 bytes returned to master (type + data size)
	sendPacketHeader(OM_SER_MASTER, p_stat, 3);
	this->write((uint8_t) R_UINT);
	this->write(p_resp);
}

/** Send Response Data to Master

   @param p_stat
   The result code, true for success and false for error

  @param p_resp
  The response data to send
  */

void OMMoCoNode::response(uint8_t p_stat, int p_resp) {
	if (p_stat == 0)
		p_stat = 2;
		// 3 bytes returned to master (type + data size)
	sendPacketHeader(OM_SER_MASTER, p_stat, 3);

	this->write((uint8_t) R_INT);
	this->write(p_resp);
}

/** Send Response Data to Master

   @param p_stat
   The result code, true for success and false for error

  @param p_resp
  The response data to send
  */

void OMMoCoNode::response(uint8_t p_stat, long p_resp) {
	if (p_stat == 0)
		p_stat = 2;
		// 5 bytes returned to master (type + data size)
	sendPacketHeader(OM_SER_MASTER, p_stat, 5);
	this->write((uint8_t) R_LONG);
	this->write(p_resp);
}

/** Send Response Data to Master

   @param p_stat
   The result code, true for success and false for error

  @param p_resp
  The response data to send
  */

void OMMoCoNode::response(uint8_t p_stat, unsigned long p_resp) {
	if (p_stat == 0)
		p_stat = 2;
		// 5 bytes returned to master (type + data size)
	sendPacketHeader(OM_SER_MASTER, p_stat, 5);
	this->write((uint8_t) R_ULONG);
	this->write(p_resp);
}

/** Send Response Data to Master

   @param p_stat
   The result code, true for success and false for error

  @param p_resp
  The response data to send
  */

void OMMoCoNode::response(uint8_t p_stat, float p_resp) {
		// 5 bytes returned to master (type + data size)
	if (p_stat == 0)
		p_stat = 2;

	sendPacketHeader(OM_SER_MASTER, p_stat, 5);
	this->write((uint8_t) R_FLOAT);
	this->write(p_resp);
}

/** Send Response Data to Master

   This method allows you to send character arrays as data, by
   specifying the length of the array.

   @param p_stat
   The result code, true for success and false for error

  @param p_resp
  The response data to send

  @param p_len
  The length of the character array to be sent

  */

void OMMoCoNode::response(uint8_t p_stat, char* p_resp, int p_len) {
		// 1 + len bytes returned to master (type + data size)
	sendPacketHeader(OM_SER_MASTER, p_stat, p_len + 1);
	this->write((uint8_t) R_STR);

		// send each character as a byte
	for( int i = 0; i < p_len; i++ )
		this->write( (uint8_t) p_resp[i] );
}

/** Set Callback Handler

 Sets the handler to be called from check() when a packet is received
 for this node.  The funcion being passed must match the following
 prototype:

 @code
 void func(byte, byte*)
 @endcode

 @param p_Func
 Pointer to a function taking two arguments and returning void.
 */

void OMMoCoNode::setHandler( void(*p_Func)(uint8_t, uint8_t, uint8_t*) ) {
	f_cmdHandler = p_Func;
}

/** Set Broadcast Callback Handler

 Sets the handler to be called from check() when a packet is received
 as a broadcast command.  This handler is separate from the directed command
 handler as it is expected that broadcast commands would duplicate functionality
 implemented in directed commands (e.g.: start, stop, pause) but broadcast commands
 demand that there be no response back to the master.  Using a special callback
 will let you handle broadcast commands differently than directed commands. e.g.:

 @code
 void handlePacket(byte isBCast, byte pktCode, byte* pktData) {
 	if( isBCast ) {
 		// do something with broadcast packets
 	}
 	else {
 		// do something with directed packets
 	}
 }

 void bcastHandler(byte pktCode, byte* pktData) {
 	handlePacket(true, pktCode, pktData);
 }

 void pktHandler(byte pktCode, byte* pktData) {
 	handlePacket(false, pktCode, pktData);
 }
 @endcode

 The function being passed must match the following prototype:

 @code
 void func(byte, byte*)
 @endcode

 @param p_Func
 Pointer to a function taking two arguments and returning void.

 */

void OMMoCoNode::setBCastHandler( void(*p_Func)(uint8_t, uint8_t, uint8_t*) ) {
	//USBSerial.println("Setting broadcast handler");
	f_bcastHandler = p_Func;
}

/** Set Not Us Callback Handler

 Sets the handler to be called from check() when a packet is received that isn't for
 device's address.  This is to allow for a command to be rebroadcasted on a different node.
 Specifically for the NMX where a command is received over bluetooth and is to be
 broadcast over the MoCoBus and vice versa.

 */

void OMMoCoNode::setNotUsHandler( void(*p_Func)(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t*) ) {
	f_notUsHandler = p_Func;
}

/** Check for Packet Received

 This method checks for a packet received, and if one is received
 it will execute the handler set via setHandler(), if set.

 When this method is called, if a packet is ready for this node on the bus
 the packet will be fully read.

 If you do not have a callback handler set, you must retrieve the buffer yourself
 and manage the response directly.

 If the packet is a broadcast packet, and the broadcast handler has been set by
 setBCastHandler(), it will execute this handler.  If no broadcast handler has
 been set, then no handler will be called for broadcast packets.

 check() returns 0 if no packet was available, a packet was found but not
 meant for this node, a core protocol command was received, or if an error
 occurs reading the packet.  Otherwise, it returns the packet code associated
 with the packet.

 @return
 The Packet Code, or 0 if no packet available.
 */

uint8_t OMMoCoNode::check() {



	uint8_t command = this->getPacket();

		// no packet available
	if( command == 0 )
		return(0);

		//Handle packets received that isn't for this device
    if (this->notUs()){
        f_notUsHandler(addr, subaddr, command, bufferLen(), buffer());
        return(0);
    }

		// command OM_SER_BASECOM is reserved for core protocol commands.
		// We handle these automatically for the node.
	if( ! this->isBroadcast() && command == OM_SER_BASECOM ) {
		_coreProtocol(this->buffer());
		return(0);
	}
	else {
		// we have a command code which is higher than 1,
		// send to the supplied handler.
		if( f_bcastHandler != 0 && this->isBroadcast() ) {
				// a callback is assigned, and this is a
				// broadcast command
			f_bcastHandler(this->subaddr, command, this->buffer());
		}
		else if( f_cmdHandler != 0 && ! this->isBroadcast() ) {
				// a callback is assigned, call callback
				// with command code and payload
			f_cmdHandler(this->subaddr, command, this->buffer());
		}
	}


	return(command);
}



/** Get Version

*/

unsigned int OMMoCoNode::version() {
	return(m_ver);
}

/** Get Identifier

*/

char* OMMoCoNode::id() {
	return(m_id);
}

// handle core protocol commands on behalf of the device

void OMMoCoNode::_coreProtocol( uint8_t* p_buf ) {
	byte subCom = p_buf[0];

	switch( subCom ) {
	case OM_SER_COREPROTO:
		// core protocol version supported
		response(true, (unsigned int) OM_SER_VER);
		break;
	case OM_SER_COREID:
		// device identifier
		response(true, id(), 8);
		break;
	case OM_SER_COREVER:
		// device version
		response(true, m_ver);
		break;
	case OM_SER_COREADDR:
		// change node addr
		address(p_buf[1]);
		response(true);
		break;
	default:
		// error
		response(false);
	}

}


void OMMoCoNode::sendPacket(uint8_t p_addr, uint8_t p_subaddr, uint8_t p_command, uint8_t p_bufLen, uint8_t* p_buf){
    //response(true);
    //sendPacketHeader(OM_SER_MASTER, true, 0);
    sendPacketHeader(p_addr, p_subaddr, p_command, p_bufLen);
	//this->write((uint8_t) R_STR);


		// send each character as a byte
	for( int i = 0; i < p_bufLen; i++ )
		this->write( (uint8_t) p_buf[i] );



}




