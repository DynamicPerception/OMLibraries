/*
 * UserInput.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#include "UserInput.h"

/**
 * Non-virtual destructor, change if wants to grow
 * */
UserInput::UserInput()
{
	cPB[0]=cPB[1]=cPB[2]=0;
	cScanCode = 0xFF;
}

/**
 * inline not good for GCC
 * */
UserInput::~UserInput(){

};

/***
 *
 *
 * */
uint8_t UserInput::KeyboardRead(MenuContext& status)
{
	//shift last readings
	cPB[0]=cPB[1];
	cPB[1]=cPB[2];
	cPB[2]= sampleButtons(&sample);
	//3 scans for debounce
	if ((cPB[0]==cPB[1]) && (cPB[0]==cPB[2]))
	{
		if (cScanCode != cPB[2])
		{
			status.setContextBits(PB_DELTA);
			cScanCode = cPB[2];
		}
	}

	//here there is a key press or key release
	if (status.getContext() & PB_DELTA)
	{
		status.clrContextBits(PB_DELTA);
		//clear the calling flag
		//and start the timer with the initial period, i.e. for the 1st 1 sec nothing happens
		status.setTimer(T_KEYBOARD_REPEAT, TYPEMATIC_INITIAL);
		status.setKeyboardIncrement(1);		//inititlise the increment
		//(it can be 1, 10, 100, & 1000 & 10000)
		cKeyboardLoopCounter = 1;
		//number of loops before the KeyboardIncrement is incremented
		status.setContextBits(KEYBOARD_VALID);
		status.setTimer(T_KEYBOARD_TIMEOUT, KEYBOARD_TIMEOUT);
		//refresh timer

		//table defined scanCode to KeyCode translation
		if (bitRead(cScanCode,0)) {
			cKeyboardCode = K_NOUSE;
		} else if (bitRead(cScanCode,1)) {
			cKeyboardCode = K_OK;
		} else if (bitRead(cScanCode,2)) {
			cKeyboardCode = K_UP;
		} else if (bitRead(cScanCode,3)) {
			cKeyboardCode = K_DOWN;
		} else if (bitRead(cScanCode,4)) {
			cKeyboardCode = K_RIGHT;
		} else if (bitRead(cScanCode,5)) {
			cKeyboardCode = K_LEFT;
		} else {
			cKeyboardCode = 0x00;
			status.clrContextBits(KEYBOARD_VALID);
				//not a valid code
		}

	} else {
	//key hold, check for typematic increases
		if (((cKeyboardCode==K_UP) || (cKeyboardCode==K_DOWN)) && (cScanCode != IDLE_SCAN))
		{//only if up or down
			if (status.getTimer(T_KEYBOARD_REPEAT) == 0)
			{// only when time is up
				cKeyboardLoopCounter++;
				if (cKeyboardLoopCounter > TYPEMATIC_STEP)
				{
					cKeyboardLoopCounter = 1;
					status.raiseKeyboardIncrement();
				}
				status.setTimer(T_KEYBOARD_REPEAT, TYPEMATIC);
				status.setContextBits(KEYBOARD_VALID);
			}
		}
	}
	status.setKeyboardCode(cKeyboardCode);
	return cKeyboardCode;
}
