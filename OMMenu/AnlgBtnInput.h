/*
 * AnlgBtnInput.h
 * Works with 5 resistor ladder read via analog input.
 *  Created on: 11.09.2012
 *      Author: perepelitsa
 */

#ifndef ANLGBTNINPUT_H_
#define ANLGBTNINPUT_H_

#include "UserInput.h"

// analog button read values
#define BUT0_VAL  70
#define BUT1_VAL  250
#define BUT2_VAL  450
#define BUT3_VAL  655
#define BUT4_VAL  830

  // button variance range
#define BUT_THRESH  60


/**
 * Works with 5 resistor ladder read via analog input.
 * */
class AnlgBtnInput: public UserInput {
	uint8_t _an_pin; //
	int last_but_rd;
	protected:
	/* return scancode regardless of acquiring method*/
virtual uint8_t sampleButtons(int* buf);
public:
	AnlgBtnInput(uint8_t a_pin);
	virtual ~AnlgBtnInput();
};

#endif /* ANLGBTNINPUT_H_ */
