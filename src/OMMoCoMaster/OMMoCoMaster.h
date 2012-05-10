/*

 MoCoBus Master Device Library

 OpenMoco Core Engine Libraries

 See www.openmoco.org for more information

 Author: C. A. Church

 (c) 2012 Dynamic Perception LLC

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

#ifndef	OM_MOCOMASTER_H
#define OM_MOCOMASTER_H

#include <inttypes.h>
// must load before wconstants to prevent issues
#include <math.h>
#include "Arduino.h"
#include "HardwareSerial.h"

#include "../OMMoCoBusExt.h"



/**

 @brief
 MoCoBus Master Library

 The MoCoBus Master class provides all necessary components needed for creating
 a new MoCoBus master using an Arduino HardwareSerial object.  This class provides
 the core functionality needed to send commands to devices and receive the response

 <b>Please note: at this time, this library only supports the Atmega328p chipset
 running with the standard UART and with the DE_PIN wired to digital pin 5.  Due
 to optimizations for timing, this cannot be changed at the present moment.  Future
 versions may resolve this issue.</b>

 Examples and information about creating MoCoBus node devices, see the
 \ref ommaster "Creating MoCoBus Master Devices" page.


 @author C. A. Church

 (c) 2012 Dynamic Perception LLC

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

class OMMoCoMaster: public OMMoCoBus {

public:

	OMMoCoMaster(HardwareSerial& c_serObj, uint8_t c_dePin);
    // 0bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd);
	// 1bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd, uint8_t p_arg);
	// 2bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd, uint16_t p_arg);
	// 3bytes of data in two kind
	int command(uint8_t p_addr, uint8_t p_cmd, uint8_t p_arg1, uint16_t p_arg2);
	int command(uint8_t p_addr, uint8_t p_cmd, uint16_t p_arg1, uint8_t p_arg2);
	// 4bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd, uint32_t p_arg);
	// 5bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd, uint8_t p_arg1, uint32_t p_arg2);
    // 6bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd, uint16_t p_arg1, uint32_t p_arg2);
    // user defined
	int command(uint8_t p_addr, uint8_t p_cmd, char* p_arg, uint8_t p_len);

	int responseType();
	int responseLen();
	char* responseData();

	// overload from OMMoCoBus
	void address(unsigned int);

	int getVersion(uint8_t p_addr);
	char* getId(uint8_t p_addr);
	int changeAddress(uint8_t p_addr, uint8_t p_newAddr);

private:

	int _getResponse();
protected:
	// 2bytes of data
	int command(uint8_t p_addr, uint8_t p_cmd, uint8_t p_arg1, uint8_t p_arg2);
};

/**

 @page ommaster Creating MoCoBus Master Devices

 A master device sends commands to, and reads responses from, one or more
 node devices.  Generally speaking, a single master may control many types of
 devices operating on the same bus.  In this documentation, we'll cover how to
 create high-level interfaces that let you easily add several devices of any type
 to your sketch or program.

 When you \ref omnode "create a MoCoBus node device", you will also want to create
 a master device which can control it.  It is possible to simply create an instance of the
 OMMoCoBus class and use its methods directly, however the OMMoCoMaster class provides
 an easier way to define higher-level interfaces to your device without
 worrying about managing the protocol its self.

 It is generally expected that if you provide a new device type, you would also
 provide a corresponding interface class for that device.

 @section masteraddr Addresses

 All responses to master devices have a target address of 0, and therefore only a single master
 may be active at a time on one bus.  However, it is possible to have a device which
 operates both as a master (sends commands and listens to responses) and as a
 node (has an address >= 2, and accepts and responds to commands).  This can allow, for
 example, a single device to operate as a node if it receives a certain command within
 a certain time, or as a master if it does not.

 @section masterrefnode A Reference Device Type

 In our examples below, we'll be talking to a device.  If you haven't already,
 you should start with the \ref omnode "Creating MoCoBus Node Devices" page.

 For many examples that follow in this class, we will use the following node
 device sketch:

 node_sketch.pde:
 @code
 #include "OMMoCoNode.h"

 // RS-485 driver enable pin
 #define DE_PIN 5

 // two commands we can accept...

 #define LED_ON	2
 #define LED_OFF 3

 // device address
 byte devAddr = 5;

 // device identifiers
 char devId[]  = "MyDevice";
 int devVer    = 1;

 // initialize node
 OMMoCoNode Node = OMMoCoNode(Serial, DE_PIN, devAddr, devVer, devId);


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
 case LED_ON:
 digitalWrite(13, HIGH);
 Node.response(true);
 break;

 case LED_OFF:
 digitalWrite(13, LOW);
 Node.response(true);
 break;

 default:

 // unknown command
 Node.response(false);
 }
 }
 @endcode

 Our device takes two commands, that turn on or off an LED.


 @section masthow How to Interface with A Simple Device

 If you are interacting only with a single device of one type, with a limited
 command set, then you can create an instance of the OMMoCoMaster class directly
 in your sketch and simply use the command() method to speak to the device.  This
 way is quite simple, as the following shows:

 simple_master.pde:
 @code
 #include "OmMoCoMaster.h"

 #define DE_PIN  5
 #define LED_ON	 2
 #define LED_OFF 3

 byte nodeAddr 	= 5;
 bool on 	= false;

 OMMoCoMaster myBus = OMMoCoMaster(Serial, DE_PIN);

 void setup() {
 Serial.begin(OM_SER_BPS);
 }

 void loop() {

 if( on == false ) {
 myBus.command(nodeAddr, LED_ON);
 }
 else {
 myBus.command(nodeAddr, LED_OFF);
 }

 on = !on;
 sleep(1000);
 }
 @endcode

 However, this is not the preferred method for creating more complex sketches, or
 sharing your code with others.  A better way to handle communicating with your
 device is to create a simple interface class, which inherits from the OMMoCoMaster
 class.  In this way, you can share your library with others and re-use code without
 having to tear apart a sketch to get the right commands, and not to mention - it allows
 you to use nice, well-named methods for each of your commands.  The following section
 shows how to create an interface class easily, that can then be used as a library
 in a sketch.

 @section masterint Create an Interface Class

 The OMMoCoMaster class is intended to be sub-classed by your specific interface.
 By doing this, your new class will inherit all of the required methods for speaking
 to devices and you will only need to add methods specific to the commands you
 want to issue.

 Here is an example of a very simple interface class, showing both the header
 and source code:

 device_master.h:
 @code

 #include "OMMoCoMaster.h"

 class MyDevMaster : public OMMoCoMaster {

 public:

 MyDevMaster(HardwareSerial& c_serObj, uint8_t c_dePin) : OMMoCoMaster(c_serObj, uint8_t c_dePin);

 int ledOn(uint8_t addr);
 int ledOff(uint8_t addr);
 };

 @endcode

 Note the constructor syntax -- we're passing on our constructor's arguments to
 the master class's constructor.  We're becoming a more specific version of the
 master class.

 device_master.cpp:
 @code

 #include "device_master.h"

 #define LED_ON	2
 #define LED_OFF 3

 MyDevMaster::MyDevMaster(HardwareSerial& c_serObj, uint8_t c_dePin) : OMMoCoMaster(c_serObj, c_dePin) {

 }

 int MyDevMaster::ledOn(uint8_t addr) {
 return command(addr, LED_ON);
 }

 int MyDevMaster::ledOff(uint8_t addr) {
 return command(addr, LED_OFF);
 }
 @endcode

 All of the required construction here is handled by the OMMoCoMaster class for
 us, so we just have an empty constructor as a place-holder should we want to do
 something else.

 Our two interface methods, ledOn and ledOff, simply call the inherited command()
 method with a specific command code the node expects to read.

 <i>N.B.: the methods for transmitting data on the bus for masters and nodes are command() and response(),
 respectively. These method names indicate the flow of data, and otherwise create
 the correct protocol for a particular device.</i>

 Now, we can use this interface in a sketch to use our device:

 master_sketch.pde:
 @code

 #include "device_master.h"

 #define DE_PIN 5

 byte nodeAddr 	= 5;
 bool on 	= false;

 MyDevMaster mvDev = MyDevMaster(Serial, DE_PIN);

 void setup() {
 Serial.begin(OM_SER_BPS);
 }

 void loop() {

 if( on == false ) {
 myDev.ledOn(nodeAddr);
 }
 else {
 myDev.ledOff(nodeAddr);
 }

 on = !on;
 sleep(1000);
 }
 @endcode

 The above example alternates between two different high-level commands to our
 node every second.

 @section masterresp Checking the Response Code from the Node

 Of course, masters expect a response from a node, signaling whether the
 command was understood and handled.  The response code is passed as part of
 every response packet from the node.

 The command() method provides the response code as an integer, indicating the status
 of the command, as reported by the node.  This code can indicate whether there
 was a timeout waiting for a response, the command was successful, or the command
 failed.

 A response code of -1 indicates a timeout waiting for the response, 1 for successful
 command, and 0 for failed command.

 As an example, we'll modify our master sketch to use this information:

 @code
 void loop() {

 if( on == false ) {
 // check for error
 if ( ! myDev.ledOn(nodeAddr) )
 digitalWrite(13, HIGH);
 }
 else {
 if ( ! myDev.ledOff(nodeAddr) )
 digitalWrite(13, HIGH);
 }

 on = !on;
 sleep(1000);
 }
 @endcode

 In our changed loop, we now check to see if the result of our method call is
 false (0 or -1 in this case).  If so, we signal an error by turning on an LED
 on the master.

 @section masterdata Receiving Data from the Node

 Of course, nodes can transmit us data along with a response code.  The node
 will indicate what type of data it is sending in addition the the actual data.

 There are three methods provided for handling responses: responseType(),
 responseLen(), and responseData().  These indicate the type of data, length
 of data in bytes, and the actual data respectively.

 There are seven types of response data currently supported, and each type is
 represented using one of the following literals:

 R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STR

 responseType() will return one of these values, or -1 if no response data was
 sent.  For strings, we may need to know their length, so this we can
 retrieve using the responseLen() method.

 The responseData() method returns a pointer to a byte array containing the
 actual data sent by the node.  If you're sending only a single value of
 one data type, it is simply converted using one of the built-in conversion methods
 such as ntoi(), ntof(), etc.

 The following shows an example of handling some data responses, in a master
 interface:

 @code
 int MyDevMaster::fooCommand(uint8_t addr) {

 int code = command(addr, 'f');

 // return zero on error
 if( ! code )
 return 0;

 int rType    = responseType();
 uint8_t* dat = responseData();

 if( rType == R_INT ) {
 return ntoi(dat);
 }
 else if( rType == R_STR ) {
 if( dat == "OK" )
 return 1;
 else
 return 0;
 }

 return 0;
 }
 @endcode

 In the above method, we expect to return an integer if the node returns one
 to us, using the ntoi() method to convert the raw data back into an integer.

 Otherwise, if we received a string back, we check to see if it says OK, returning
 a positive value if it does and zero otherwise.

 @section mastervalid Validating a Node

 If it is possible that there is more than one type of node on the bus, it is also
 possible that you may have the wrong address for the node device type you're expecting.

 To validate that you're speaking to the right kind of device, you can use the
 built-in device indentifier and version commands built into the protocol.  Neither your
 master nor your node need to implement any code to create or handle these commands,
 and the OMMoCoMaster class includes the built-in getVersion(), getId() methods to
 retrieve this information for you.

 It is useful to add such a check before sending commands to any device, to
 prevent unintended actions by sending a command with a different meaning to the
 target device.

 The following example adds a new method to our earlier example to validate a node:

 device_master.h:
 @code

 #include "OMMoCoMaster.h"

 class MyDevMaster : public OMMoCoMaster {

 public:

 MyDevMaster(HardwareSerial& c_serObj, uint8_t c_dePin) : OMMoCoMaster(c_serObj, uint8_t c_dePin);

 int ledOn(uint8_t addr);
 int ledOff(uint8_t addr);
 bool validate(uint8_t addr);

 private:

 static const char devId[] 		= "MyDevice";
 static const unsigned int devVersion 	= 1;

 };

 @endcode


 device_master.cpp:
 @code

 #include "device_master.h"

 #define LED_ON	2
 #define LED_OFF 3

 MyDevMaster::MyDevMaster(HardwareSerial& c_serObj, uint8_t c_dePin) : OMMoCoMaster(c_serObj, c_dePin) {

 }

 int MyDevMaster::ledOn(uint8_t addr) {
 return command(addr, LED_ON);
 }

 int MyDevMaster::ledOff(uint8_t addr) {
 return command(addr, LED_OFF);
 }

 bool MyDevMaster::validate(uint8_t addr) {

 if( getId(addr) != devId || getVersion(addr) != devVersion )
 return false;

 return true;
 }
 @endcode

 Our new validate() method returns false if the Id and Version of the remote
 device are not the expected values.

 master_sketch.pde:
 @code

 #include "device_master.h"

 #define DE_PIN 5

 byte nodeAddr 	= 5;
 bool led 	= false;
 bool ok 	= true;

 MyDevMaster mvDev = MyDevMaster(Serial, DE_PIN);

 void setup() {
 Serial.begin(OM_SER_BPS);

 if( ! myDev.validate(nodeAddr) )
 ok = false;
 }

 void loop() {

 if( ok ) {
 if( led == false ) {
 myDev.ledOn(nodeAddr);
 }
 else {
 myDev.ledOff(nodeAddr);
 }

 led = !led;
 sleep(1000);
 }
 }
 @endcode

 */

#endif

