/*
 * UserInput.h
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#ifndef USERINPUT_H_
#define USERINPUT_H_

#include <inttypes.h>
#include "Arduino.h"
#include "MenuContext.h"

//KEYBOARD CONSTANTS
#define TYPEMATIC_INITIAL 10	//1 sec before first repeat
#define TYPEMATIC 2				//200mS for every increment thereafter
#define TYPEMATIC_STEP 10		//10 increments before going up a multiplier

#define IDLE_SCAN 0x00          //code to return if nothing pressed

//bit masks for keys
#define K_NOUSE 1		  	    //(not used) Keyboard Code associated with a key
#define K_OK 2				    //Keyboard Code associated with a key
#define K_UP 4					//Keyboard Code associated with a key
#define K_DOWN 8				//Keyboard Code associated with a key
#define K_LEFT 16               //Keyboard Code associated with a key
#define K_RIGHT 32              //Keyboard Code associated with a key

#define KEYBOARD_TIMEOUT 1200	//2 minutes for keyboad before timeout
/**
 * Scans user input, like keyboard, joystick, etc.
 * Debounce reading and sets flag on ready.
 * */
class UserInput {
	uint8_t cScanCode;
	//uint8_t iKeyboardIncrement;
	uint8_t cKeyboardLoopCounter;
	uint8_t cKeyboardCode;
	uint8_t cPB[3];	//for debounce of scancodes
	int sample; //low-level value for debug

protected:
	/* return scancode regardless of acquiring method*/
	virtual uint8_t sampleButtons(int* buf) = 0;

public:
	UserInput();
	virtual ~UserInput();
	/* key press/depress logic and repeat */
	uint8_t KeyboardRead(MenuContext& status);

	/* mostly for debug*/
	int GetSample() const {return sample;}
};

#endif /* USERINPUT_H_ */
