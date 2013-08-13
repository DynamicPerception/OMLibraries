/*

MoCoBus Client Library for Nodes

OpenMoco Core Engine Libraries 

See www.openmoco.org for more information

(c) 2008-2011 C.A. Church / Dynamic Perception LLC

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


#ifndef	OM_MOCONODE_H
#define OM_MOCONODE_H

#include <inttypes.h>
 // must load before wconstants to prevent issues
#include <math.h>
#include "Arduino.h"
#include <HardwareSerial.h>

#include "../OMMoCoBus/OMMoCoBus.h"



/**

  @brief
  MoCoBus Node Library
  
  The MoCoBus Node class provides all necessary components needed for creating 
  a new MoCoBus node using an Arduino HardwareSerial object.  With this class, all
  one needs to do to create a new node is to create a callback handler that
  understands packet codes and data, and respond correctly when needed.
  
  <b>Please note: at this time, this library only supports the Atmega328p chipset
  running with the standard UART and with the DE_PIN wired to digital pin 5.  Due
  to optimizations for timing, this cannot be changed at the present moment.  Future
  versions may resolve this issue.</b>
  
  Examples and information about creating MoCoBus node devices, see the 
  \ref omnode "Creating MoCoBus Node Devices" page.
  

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
  

class OMMoCoNode : public OMMoCoBus {

public:


	OMMoCoNode(HardwareSerial& c_serObj, unsigned int c_addr, unsigned int c_ver, char* c_id);
	uint8_t check();
	
	void response(bool p_stat);
	void response(bool p_stat, uint8_t p_resp);
	void response(bool p_stat, unsigned int p_resp);
	void response(bool p_stat, int p_resp);
	void response(bool p_stat, unsigned long p_resp);
	void response(bool p_stat, long p_resp);
	void response(bool p_stat, float p_resp);
	void response(bool p_stat, char* p_resp, int p_len);
	
	void setHandler(void(*)(uint8_t, uint8_t*));
	void setBCastHandler(void(*)(uint8_t, uint8_t*));
	
	unsigned int version();
	char* id();
	
	
private:
	void(*f_cmdHandler)(uint8_t,uint8_t*);
	void(*f_bcastHandler)(uint8_t,uint8_t*);
	
	unsigned int m_ver;
	char* m_id;	
	
	void _coreProtocol( uint8_t* p_buf );
};

#endif

/**
  @page omnode Creating MoCoBus Node Devices
  
  Creating a new device to live on a MoCoBus network is simple using the 
  OMMoCoNode library.  You only have to determine how to handle the commands
  you receive, the underlying libraries handle all the necessary functionality
  to route only commands intended for your device to you and allow you to respond
  back to masters with the information you choose.
  
  @section nodeaddr Node Address
  
  Each node on a bus must have a unique address, in this way the bus master may
  direct commands to specific nodes.  You must specify the device address when
  constructing an OMMoCoNode object.
  
  An OMMoCoNode will only handle commands issued to its address.  All other 
  commands will be safely ignored.  You may change the address at any time by
  calling the address() method, and the change will be immediately effective.
  
  There are 254 possible addresses for you to choose from - ranging from 2-255.
  The addresses 0 and 1 are reserved by the protocol.
  
  Remember that a master on the bus may change your node's address at any time,
  for information on how to detect and handle this, see the section \ref nodeaddrchg "Address Changes for Nodes"
  below.
  
  
  @section nodeident Identifying Your Device
  
  Your node must identify its self when asked, so that a master may understand
  what type of device it is speaking to.  The node library will send this 
  response for you when needed, but you must first specify the correct information.
  
  The OMMoCoNode constructor allows you to specify two identifying pieces of
  information about your node: a device identifier, and a device version.
  
  A device identifier is an 8 character string that gives the device a unique
  name.  This is used to differentiate types of devices, such as a motor axis
  or an intervalometer, so that a master may know what commands the device supports.
  
  The device version is a numeric version number for a device type.  It is used to
  differentiate the capabilities of two versions of a specific device.  For example,
  you may add a new command to a device you've already created and have been using.
  After adding this command, increasing the version number allows you to configure
  a master to only send the new command to devices having the higher version number,
  preventing an error when interacting with older devices.
  
  Please note that the device identifier must be exactly 8 characters in length, and
  any characters outside of the lower-ASCII set will be converted to the character
  '0' (numeric value 48).  
  
  @section nodecreate Creating a Device
  
  To create a device, we'll need to create an instance of the OMMoCoNode class
  and specify some information about our device:
  
  @code
    // note: you -must- include OMMoCoBus.h before including OMMoCoNode.h
  
#include "OMMoCoBus.h"
#include "OMMoCoNode.h"

 // RS-485 driver enable pin
#define DE_PIN 5

 // device address
byte devAddr = 10;

 // device identifiers
char devId[]  = "MyDevice";
int devVer    = 1;

 // initialize node
OMMoCoNode Node = OMMoCoNode(Serial, devAddr, devVer, devId);
  
void setup() {
	Serial.begin(OM_SER_BPS);
}

  @endcode

  Note that in the above example, we have to use Serial.begin() before we
  attempt to use the node.  The constant OM_SER_BPS has been defined for you
  as a convienence, you should use this unless you really need to change the
  speed of your bus.  
  
  Now that we have created a device, we'll need to specify how to handle commands.

  @section nodecomm Receiving and Processing Commands
  
  The primary method for handling commands sent to your device, is to use a 
  callback function.  This function will be called whenever a command is 
  received for your device, passing along the command id and the data received.
  
  What command IDs and data expected with those commands to receive are up to
  you.  Commands are specified as a single byte or char, and their data may take on
  any form, including a string. The only limitation is that the data for one 
  command must be no more than 32 bytes in length.
  
  Registering a callback allows you to define a function that will handle the 
  commands sent by a master on the bus, and this function will be called as
  needed by the check() method.  You should call the check() method as appropriate
  in the normal execution of your program to see if a command has arrived and
  process it.
  
  The following example expands on the previous section to specify the callback
  handler:
  
  @code
    // note: you -must- include OMMoCoBus.h before including OMMoCoNode.h
  
#include "OMMoCoBus.h"
#include "OMMoCoNode.h"

 // RS-485 driver enable pin
#define DE_PIN 5

 // device address
byte devAddr = 10;

 // device identifiers
char devId[]  = "MyDevice";
int devVer    = 1;

 // initialize node
OMMoCoNode Node = OMMoCoNode(Serial, devAddr, devVer, devId);


void setup() {
	Serial.begin(OM_SER_BPS);
	Node.setHandler(myHandler);
}


void loop() {

		// check for a new command and process
		// as needed
		
	Node.check();
}


void myHandler(byte command, byte* comData) {

	switch(command) {
		case 2:
			// do something
			
			Node.response(true);
			break;
		case 'b':
			// do something
			
			Node.response(true);
			break;
			
		default:
		
			// unknown command
			Node.response(false);
	}
}

  @endcode
  
  The check() method is called in the main loop, if a command is received and
  it is destined for this node, the callback myHandler() is executed.  For commands required
  by the protocol, such as version or identifier information, these are handled
  automatically by the node library, and you do not need to do anything for them
  to be handled properly.
  
  Note that in the above example, our callback gets two arguments: A 
  single byte command code, and a pointer to an array of data bytes.  The command
  code is a single byte which specifies the command being sent to you.  Only
  254 possible codes (2-255) are available for your use, so for more complex
  command sets you may wish to create sub-commands by putting them in the first 
  part of the data.  Codes 0 and 1 are reserved to the core protocol and will
  never be received by your callback.
  
  In our example, we show that command codes can be expressed either as numeric
  values or as single characters - since both can be represented in a single byte.  
  
  As you are the one that defines what commands your device receives, you also
  will know the amount and type of the data you will expect in addition to the
  commands and therefore can implement the processing of that data directly.
  Remember that you can receive up to 32 bytes of data.  If you wish to know
  how much data was sent to your command, see the bufferLen() method.
  
  You must take care that the data you expect to receive cannot contain a 
  sequence of bytes that matches the break sequence in the protocol (five nulls
  followed by a single byte with the value 255).  You must pad values if passing
  multiple binary values to preven this from occuring.
  
  Every command issued to your device must get a response as quickly as possible.
  Therefore, you should never perform a long-running, blocking action when
  processing a command.  If your response takes too long, the master will likely
  time out your response and handle the situation as an error.
  
  @section noderesp Sending Responses 
  
  A response must be sent to the master as soon as possible after receiving a
  command.  This response must indicate whether the command was successfully 
  handled, and any data that needs to be transported with the response.
  
  There are two codes, true or false, which are available to you.  True indicates
  that the command was successful, and false that the command in some way
  failed (e.g.: the command was unknown).
  
  The OMMoCoNode library allows you to send nearly any basic type of data as
  a response to a master, and in addition you can also send a string of a 
  specified length as a response.  The underlying mechanisms ensure this
  data is transmitted in a way that the master can understand.
  
  The following example shows a callback handler that sends three
  types of data, a single-byte, a floating point integer, and a string:
  
  @code
 void myHandler(byte command, byte* comData) {

 	byte foo = 1;
 	float bar = 3.25;
 	char baz[] = "OK";
 	
	switch(command) {
		case 'a':
			// do something
			Node.response(true, foo);
			break;
		case 'b':
			// do something
			Node.response(true, bar);
			break;
		
		case 'c':
			// do something
			Node.response(true, "OK", 2);
			break;
			
		default:
		
			// unknown command
			Node.response(false);
	}
}

  @endcode
  
  As we can see from the example, string responses require a length so that the
  unlderying libraries can transport the data properly.

  @section nodedata Processing Data 
  
  Of course, lots of commands we might receive will come with additional data
  about how to process them.  As all multi-byte data sent using the mocobus
  protocol is in big-endian order, you can either re-pack the bytes manually,
  or simply use the provided helper functions: ntoi(), ntoui(), ntol(), ntoul(),
  and ntof(), covering integers, unsigned integers, longs, unsigned longs, and
  floats respectively.
  
  @code
 void myHandler(byte command, byte* comData) {

 	byte foo = 1;
 	float bar = 3.25;
 	char baz[] = "OK";
 	
 	byte speed = 0;
 	int dist = 0;
 	
	switch(command) {
		case 'a':
			// accept a single byte of data
			
			speed = comData[0];

			// do something
			
			Node.response(true, foo);
			break;
			
		case 'b':
			// accept an unsigned integer
			
			dist = Node.ntoui(comData);
			
			// do something
			
			Node.response(true, bar);
			break;
		
		case 'c':
			// do something
			Node.response(true, baz, 2);
			break;
			
		default:
		
			// unknown command
			Node.response(false);
	}
}

  @endcode  
  
  Should we want to accept strings up to 31 characters in length, we can simply
  re-cast the pointer to the byte array to a pointer to a character array.  If
  doing this, you must ensure the strings are null-terminated.
  
  e.g.:
  
  @code
  
  char* foo;
  
  ...
  
  case 'd':
  	// get string from data
  	foo = (char*) comData;
  	
  	if( foo == "left" || foo == "right" ) {
  		Node.response(true, "OK", 2);
  	}
  	else {
  		Node.response(false, "Wrong Dir", 9);
  	}
  	
  	break;
  
  @endcode
  
  Now, we have all of the core elements together to create our own device types,
  and manage nodes on a network.
  
  @section nodebcast Handling Broadcast Packets
  
  The MoCoBus protocol allows for broadcast packets to be received by all nodes.
  Such packets are normally used to synchronize activities, timing, or other
  functionality on the bus.  Broadcast packets are identified by being directed
  to a specific address, which is defined by OM_SER_BCAST_ADDR.
  
  <b>You must never send a response to a broadcast packet - doing so will
  result in bus contention or collisions, resulting in bus instability.</b>
  
  There are a small set of "basic" broadcast packet codes which have been 
  pre-defined as part of the MoCoBus protocol, as they are likely to be used
  by many node types, they are specified using the following defined values:
  
  OM_BCAST_START
  	
  	To be interpreted as a start command
  	
  OM_BCAST_STOP
  
  	To be interpreted as a stop command
  	
  OM_BCAST_PAUSE
  
  	To be interpreted as a pause command
  	
  
  If your node can receive and handle broadcast commands, you will need to
  specify a broadcast handler.  If you do not specify a broadcast handler,
  no callback handler will be executed on a broadcast command, and your node
  will ignore it.
  
  The following code provides an example of handling a broadcast command in an
  Arduino sketch:
  
  @code
  void setup() {
  	...
  	Node.setBCastHandler(broadcast);
  	...
  }
  
  void broadcast(byte code, byte* data) {
  
  	switch(code) {
  	
  		case OM_BCAST_START:
  			do_start();
  			break;
  		
  		case OM_BCAST_STOP:
  			do_stop();
  			break;
  			
  		case OM_BCAST_PAUSE:
  			do_pause();
  			break;
  		
  		default:
  			break;
  	}
  }
  @endcode
  
  Note that broadcast packets may contain packet data like any normal packet. 
  To prevent collision between different node types and the broadcast commands
  they accept, it is suggested to add a definition for any broadcast commands
  you create to OMMoCoDefs.h and submit it back to the community for public
  acceptance.
  
  @section nodeaddrchg Address Changes for Nodes
  
  The MoCoBus protocol specifies an address change command as part of the
  core command set which all devices must recognize.  This library handles 
  all such commands for you.  However, one of the core commands every device
  must recognize is the change address command.  
  
  For many devices, saving the address between power cycles is necessary for
  a good user experience.  However, since the address of your particular node
  can be changed without your involvement, you will need a way to know about the
  change so that you may trigger your action.
  
  The node library provides such a way for you.  The method addressCallback()
  allows you to specify a callback which will be executed every time the address
  for your node is changed - no matter the source of the change.  The specified
  callback method must accept a single byte argument (the new address) and return
  void.  The following is an example of handling address changes:
  
  @code
  ...
  
void setup() {
	Serial.begin(OM_SER_BPS);
	Node.setHandler(myHandler);
	Node.addressCallback(myAddrChange);
}

void myAddrChange(byte p_addr) {
  // a new address was assigned to the node
  
  // do something with the new address, like save to eeprom
}

...

  @endcode
  
  With the above addition, we now have a callback which will execute an action
  whenever the address is changed.
  
  Of course, you may also choose to change the address of your node - this can
  be accomplished using the address() method.
  
  @section nodebcast Handling Broadcast Commands
  
  */
  




