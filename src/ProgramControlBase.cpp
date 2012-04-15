/*
 * ProgramControl.cpp
 *
 *  Created on: 13.04.2012
 *      Author: perepelitsa
 */

#include "ProgramControlBase.h"

ProgramControlBase::ProgramControlBase() {
	// TODO Auto-generated constructor stub

}

ProgramControlBase::~ProgramControlBase() {
	// TODO Auto-generated destructor stub
}


bool ProgramControlBase::dispatch(uint8_t cmd, uint8_t** mailbox) const {

  return true;
}
