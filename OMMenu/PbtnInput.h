/*
 * PbtnInput.h
 *
 *  Created on: 11.09.2012
 *      Author: perepelitsa
 */

#ifndef PBTNINPUT_H_
#define PBTNINPUT_H_

#include "UserInput.h"
/***
 * Works with simple pushbuttons.
 * */
class PbtnInput: public UserInput {
	uint8_t _up_pin; //
	uint8_t _down_pin; //
	uint8_t _enter_pin; //
	uint8_t _menu_pin; //

protected:
	/* return scancode regardless of acquiring method*/
	virtual uint8_t sampleButtons(int* buf);
public:
	PbtnInput(uint8_t up, uint8_t down, uint8_t enter, uint8_t menu);
	virtual ~PbtnInput();
};

#endif /* PBTNINPUT_H_ */
