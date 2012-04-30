#include "OMMoCoMaster.h"
#include "../Utils.h"

/** Constructor

 Constructs a new instance of the class. Accepts a hardware serial object and
 the digital pin # of the transceiver DE/RE pin combination.

 A master device's address is always 0, so an address is not specified.

 @param c_serObj
 An Arduino HardwareSerial object

 @param c_dePin
 The transceiver DE/RE digital I/O pin number

 */

OMMoCoMaster::OMMoCoMaster(HardwareSerial& c_serObj, uint8_t c_dePin) :
		OMMoCoBus(c_serObj, c_dePin, (unsigned int)OM_SER_MASTER_ADDR) {

}

/** Send A Command to a Node

 @param p_addr
 Device address to send command to

 @param p_cmd
 Command code

 @return
 An integer with the packet code returned, or -1 for response timeout
 */

int OMMoCoMaster::command(uint8_t p_addr, uint8_t p_cmd) {

	// 0 bytes sent to node after command
	sendPacketHeader(p_addr, p_cmd, 0);
	return _getResponse();
}

/** Send A Command to a Node with Data

 @param p_addr
 Device address to send command to

 @param p_cmd
 Command code

 @param p_arg
 Command data to send

 @return
 An integer with the packet code returned, or -1 for response timeout
 */

int OMMoCoMaster::command(uint8_t p_addr, uint8_t p_cmd, uint8_t p_arg) {

	// 1 bytes sent to node after command
	sendPacketHeader(p_addr, p_cmd, 1);
	this->write(p_arg);

	return _getResponse();
}

/** Send A Command to a Node with Two Data Byte Values

 @param p_addr
 Device address to send command to

 @param p_cmd
 Command code

 @param p_arg
 Command data to send

 @return
 An integer with the packet code returned, or -1 for response timeout
 */

int OMMoCoMaster::command(uint8_t p_addr, uint8_t p_cmd, uint8_t p_arg,
		uint8_t p_arg2) {

	// 2 bytes sent to node after command
	sendPacketHeader(p_addr, p_cmd, 2);
	this->write(p_arg);
	this->write(p_arg2);

	return _getResponse();
}

/** Send A Command to a Node with Data

 @param p_addr
 Device address to send command to

 @param p_cmd
 Command code

 @param p_arg
 Command data to send

 @return
 An integer with the packet code returned, or -1 for response timeout
 */

int OMMoCoMaster::command(uint8_t p_addr, uint8_t p_cmd, unsigned int p_arg) {

	// 2 bytes sent to node after command
	sendPacketHeader(p_addr, p_cmd, 2);
	this->write(p_arg);

	return _getResponse();
}

/** Send A Command to a Node with Data

 @param p_addr
 Device address to send command to

 @param p_cmd
 Command code

 @param p_arg
 Command data to send

 @return
 An integer with the packet code returned, or -1 for response timeout
 */

int OMMoCoMaster::command(uint8_t p_addr, uint8_t p_cmd, unsigned long p_arg) {

	// 4 bytes sent to node after command
	sendPacketHeader(p_addr, p_cmd, 4);
	this->write(p_arg);

	return _getResponse();
}

/** Send A Command to a Node with Data

 This method allows you to send character arrays as data, by
 specifying the length of the array.

 @param p_addr
 Device address to send command to

 @param p_cmd
 Command code

 @param p_arg
 Command data to send

 @param p_len
 Data Length

 @return
 An integer with the packet code returned, or -1 for response timeout
 */

int OMMoCoMaster::command(uint8_t p_addr, uint8_t p_cmd, char* p_arg,
		uint8_t p_len) {

	//  p_len bytes sent to node after command
	sendPacketHeader(p_addr, p_cmd, p_len);

	// send each character as a byte
	for (int i = 0; i < p_len; i++)
		this->write((uint8_t) p_arg[i]);

	return _getResponse();
}

/** Set Address

 This method is overloaded from the base OMMoCoBus class, as masters cannot
 have any address but zero.  Calling this method will do nothing.

 */

void OMMoCoMaster::address(unsigned int p_addr) {
}

/** Get Response Data Type

 If a response was received, you may call this method to determine what
 type of data the response sent.  Returns the response data type, or -1
 if no response data was sent.  The following response data types are supported:

 R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STR

 @return
 Response data type or -1 if no response data received.

 */

int OMMoCoMaster::responseType() {

	if (bufferLen() < 1)
		return (-1);

	char* resp = (char*) buffer();

	return ((int) resp[0]);
}

/** Get Response Data

 If a response with data was received, you may call this method to get the data.

 You should always first use responseType() to determine whether or not
 response data was actually received before calling this method.

 @return
 A pointer to a character array holding the response data.

 */

char* OMMoCoMaster::responseData() {
	char* resp = (char*) buffer();
	// seek ahead one byte to get past data type code
	resp++;
	return (resp);
}

/** Get Response Data Length

 @return
 The length of the response data, in bytes.
 */

int OMMoCoMaster::responseLen() {
	return (bufferLen() - 1);
}

/** Get Device Type Version

 Returns the device type version from the device at the
 specified address.  Returns -1 on any error.

 @param p_addr
 The address of the device

 @return
 The version of the device, or -1 on error.
 */

int OMMoCoMaster::getVersion(uint8_t p_addr) {

	if (command(p_addr, (uint8_t) OM_SER_BASECOM, (uint8_t) OM_SER_COREVER)
			!= 1)
		return (-1);

	if (responseLen() > 0) {
		int ver = Utils::ntoi((uint8_t*) responseData());
		return (ver);
	}

	return (0);

}

/** Get Device Identifier

 Returns the textual identifier of the device.  Note that the pointer will
 be initialized to zero, and will remain so on error.  Always check this
 value.  The bus protocol specifies that every device ID must be a string of
 exactly 8 charactes, not null-terminated.

 @param p_addr
 The address of the device

 @return
 The device identifier
 */

char* OMMoCoMaster::getId(uint8_t p_addr) {

	char* dat = 0;

	if (command(p_addr, (uint8_t) OM_SER_BASECOM, (uint8_t) OM_SER_COREID) != 1)
		return (dat);

	if (responseLen() > 0) {
		char* dat = responseData();
		return (dat);
	}

	return (dat);

}

/** Change Device Address

 Changes the device address to the new address.

 @param p_addr
 The address of the device

 @param p_newAddr
 The new address the device should use

 @return
 -1 if an error occurred, or 1 if successful.
 */

int OMMoCoMaster::changeAddress(uint8_t p_addr, uint8_t p_newAddr) {

	if (command(p_addr, (uint8_t) OM_SER_BASECOM, (uint8_t) OM_SER_COREADDR,
			p_newAddr) != 1)
		return (-1);

	return (1);

}

// get response with timeout
int OMMoCoMaster::_getResponse() {

	unsigned long cur_tm = millis();

	// timeout if we don't get a response packet in
	// time

	uint8_t code = getPacket();
	while (code == 0) {
		if (millis() - cur_tm > OM_SER_MASTER_TIMEOUT)
			return -1;
		code = getPacket();
	}

	return code;
}

