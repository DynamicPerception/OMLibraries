/*
 * PbtnInput.cpp
 *
 *  Created on: 11.09.2012
 *      Author: perepelitsa
 */

#include "PbtnInput.h"

PbtnInput::PbtnInput(uint8_t up, uint8_t down, uint8_t enter, uint8_t menu) {

		_up_pin = up; //
		_down_pin = down; //
		_enter_pin = enter; //
		_menu_pin = menu; //
		//
		pinMode(_up_pin, INPUT);
		pinMode(_down_pin, INPUT);
		pinMode(_enter_pin, INPUT);
		pinMode(_menu_pin, INPUT);

		pinMode(13, OUTPUT);

		// enable internal pull-up resistors
		digitalWrite(_up_pin, HIGH);
		digitalWrite(_down_pin, HIGH);
		digitalWrite(_enter_pin, HIGH);
		digitalWrite(_menu_pin, HIGH);

		digitalWrite(13,HIGH);
}

PbtnInput::~PbtnInput() {
	// TODO Auto-generated destructor stub
}


/**
 * For better portability.
 * Use digitalRead instead of direct read from port, but
 * N.B. Read is skew i.e. not all pins at same time
 * return zero bits for pressed buttons
 * */
uint8_t PbtnInput::sampleButtons(int* not_used)
{
uint8_t scan = 0;

  if (digitalRead(_up_pin)) {
	  bitSet(scan, 3); //match K_UP;
  }

  if (digitalRead(_down_pin)) {
	  bitSet(scan, 2);// |= K_DOWN;
  }

  if (digitalRead(_enter_pin)) {
	 bitSet(scan, 0); // match K_ENTER;
  }

  if (digitalRead(_menu_pin)) {
	  bitSet(scan, 1); //match K_MENU;
  }
  return scan;
}
