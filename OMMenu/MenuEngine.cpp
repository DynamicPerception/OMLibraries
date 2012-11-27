/*
 * MenuEngine.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#include "MenuEngine.h"

// which input is our button
#define BUT_PIN 14

/**
 * top level class may have normal constructor
 * */
MenuEngine::MenuEngine(LiquidCrystal& _lcd, Commander& _cmd)
: HorizMenu(_lcd, _cmd), keyboard(BUT_PIN)
{
	InitialiseDisplay();

	_led = 13;
	pinMode(_led, OUTPUT);

	iEnginePhase = 0;

	/* blink LED counter*/
	cActivity = 0;

	FlattenStack();

	mSecInitialise(&mSTimer0, &mSTimer0_Freeze, HUNDRED_MS);
}



/***
 * Call in endless loop
 * */
//ToDo decide with virtual interface using
void MenuEngine::ProcessUI() {
	switch (iEnginePhase) {
	case 0:
		UpdateView();
		iEnginePhase++;
		break;
	case 1:
		Keyboard();
		iEnginePhase++;
		break;
	case 2:
		OverallProcess();
		iEnginePhase++;
		break;
	case 3:
		DisplayInterfaceManager();
		iEnginePhase++;
		break;
	default:
		iEnginePhase = 0;
		break;
	}
}


/*******************************************
 * Define menu reaction depending on iInterPhase selector.
 * updates global iInterPhase from NextAction
 *
 * */
void MenuEngine::DisplayInterfaceManager() {
	switch (iInterPhase) {
	case 0:
		break;
	case INITIATE_LEVEL:
		CreateMenu(INITIAL_DISPLAY_SCREEN);
		clearPointerPos();
		iInterPhase++;
		break;
	case DIALOG_LEVEL_ENTRY:
		TransferMenu();
		//not internal flag prevents refresh
		//every time it is called
		if (status.getContext() & KEYBOARD_VALID) {
			status.clrContextBits(KEYBOARD_VALID);
			uint8_t keyCode = status.getKeyboardCode();
			if (keyCode == K_OK) {//only in Menu input
				DisplaySelectedItem();
			} else if (keyCode == K_DOWN) {
				//ShowDialog();
			} else if (keyCode == K_UP) {

			}
		}
		break;
	case PAGES_LEVEL_ENTRY:
	case MENU_LEVEL_ENTRY:
		TransferMenu();
		if (status.getContext() & KEYBOARD_VALID) {
			status.clrContextBits(KEYBOARD_VALID);
			uint8_t keyCode = status.getKeyboardCode();
			if  (keyCode == K_OK) {
				DisplaySelectedItem();
			} else if (keyCode == K_UP) {
				PopStack();
			} else if  (keyCode == K_LEFT) {
				PositionMoveLeft();
			} else if (keyCode == K_RIGHT) {
				PositionMoveRight();
			}

		} else {//check timeout
			TimeoutNoKeyEntry();
		}
		break;

		//TYPICAL MENU SCAN
	case WIZARD_LEVEL_ENTRY:
		TransferMenu();
		if (status.getContext() & KEYBOARD_VALID) {
			status.clrContextBits(KEYBOARD_VALID);
			uint8_t keyCode = status.getKeyboardCode();
			if  (keyCode == K_OK) {
				DisplaySelectedItem();
			} else if (keyCode == K_UP) {
				InvalidAction();
			} else if  (keyCode == K_LEFT) {
				InvalidAction();
			} else if (keyCode == K_RIGHT) {
				NavigateNext();
			}
		} else {//check timeout
			TimeoutNoKeyEntry();
		}
		break;

		//TYPICAL PARAMTER CHANGE
	case PARAM_ITEM_EDIT:
		TransferMenu();
		if (status.getContext() & KEYBOARD_VALID) {
			status.clrContextBits(KEYBOARD_VALID);

			uint8_t keyCode = status.getKeyboardCode();
			if  (keyCode == K_OK) {
				status.closeParameter(true);
				//update the parameter
				PopStack();
			} else if (keyCode == K_LEFT) {
				status.closeParameter(false);
				PopStack();
			} else if (keyCode == K_RIGHT) {
				InvalidAction();
			} else if  (keyCode == K_UP) {
				IncreaseParameter();
			} else if (keyCode == K_DOWN) {
				DecreaseParameter();
			}

		} else {//check timeout
			TimeoutNoKeyEntry();
		}
		break;

	case ACTION_SCREEN:
		TransferMenu();
			if (status.getContext() & KEYBOARD_VALID) {
				status.clrContextBits(KEYBOARD_VALID);

				uint8_t keyCode = status.getKeyboardCode();
				if  (keyCode == K_OK) {
					actionScr.closeAction(true);
					//update the parameter
					PopStack();
				} else if (keyCode == K_LEFT) {
					actionScr.Left();
					//no update of the parameter
				} else if (keyCode == K_RIGHT) {
					actionScr.Right();
				} else if  (keyCode == K_UP) {
					actionScr.closeAction(false);
					PopStack();
				} else if (keyCode == K_DOWN) {
					InvalidAction();
				}

			} else {//check timeout
				TimeoutNoKeyEntry();
			}
			break;

	default:
		iInterPhase = INITIATE_LEVEL;
		break;
	}

}

/* *********************************** */
uint8_t MenuEngine::Keyboard(void) {
	return keyboard.KeyboardRead(status);
}

/***
 *
 **/
void MenuEngine::OverallProcess(void) {

	if (mSecTimeup(&mSTimer0, &mSTimer0_Freeze)) {
		mSecInitialise(&mSTimer0, &mSTimer0_Freeze, HUNDRED_MS);
		//refresh timer for next period
		for (uint8_t i = 0; i < NUMBER_OF_TIMERS; i++) {
		//decrement all timers here every 100mS
			if (status.getTimer(i) != 0) {
				status.decTimer(i);
			}
		}
	}

	//check for activity indicator
	if (status.getTimer(T_ACTIVITY) == 0) {
		status.setTimer(T_ACTIVITY, ACTIVITY_TIME);
		//activity is essentially a divide by 2 counter
		//the LSB is used to flash the colon in the display
        cActivity++;

		if (cActivity & 0x01) {
			digitalWrite(_led, HIGH);
		} else {
			digitalWrite(_led, LOW);
		}

	}

}

/**
 * Restore parameters in order to return to the previous place
 * */
void MenuEngine::PopStack(void) {
	if (ProcessStack.cStackPointer <= 1) {
		FlattenStack();
	} else {
		ProcessStack.cStackPointer--;
		//drop back to valid reading
		CreateMenu(ProcessStack.cScreenCreatedToGetHere[ProcessStack.cStackPointer]);
		//setTopLine(ProcessStack.cTopLine[ProcessStack.cStackPointer]);
		setPointerPos(ProcessStack.cPointerLine[ProcessStack.cStackPointer]);
		iInterPhase = ProcessStack.iCallingInterPhase[ProcessStack.cStackPointer];
		//if there is a password system- clear the password validation below a certain level here as
		//well as elsewhere, as necessary

	}
}

/**
 * Force a restart of the display.
 **/
void MenuEngine::FlattenStack(void) {
	ProcessStack.cStackPointer = 0;
	iInterPhase = INITIATE_LEVEL;
	//setTopLine(0);
	setPointerPos(0);
	status.clrContextBits(TIME_DISABLE);
	//iPreviousMinutes = 0xff;

	//if there is a password system- clear the password validation here as
	//well as elsewhere, as necessary
}

void MenuEngine::PushStack (void)
{//save parameters in order to return to the same place
	ProcessStack.cScreenCreatedToGetHere[ProcessStack.cStackPointer]=
		displayBuffer.cCurrentScreen;
	//ProcessStack.cTopLine[ProcessStack.cStackPointer]=cTopLine;
	ProcessStack.cPointerLine[ProcessStack.cStackPointer] = getPointerPos();
	ProcessStack.iCallingInterPhase[ProcessStack.cStackPointer]=iInterPhase;
	ProcessStack.cStackPointer++;
	//increment the pointer so ready for next push
}

/**
 * Updates state variable.
 * */
void MenuEngine::DisplaySelectedItem(void)
{
	PushStack();
	iInterPhase = GoToDisplaySelected();
}

/**
 * Navigate in wizard mode
 * */
void MenuEngine::NavigateNext(void) {
	//take selected value
	    uint8_t item = getJumpItem();
	    uint8_t level = getJumpLevel();

		//same level or another level
	    if ((level != displayBuffer.cCurrentScreen)&&(level != 0)) {
	    	//PopStack();
	    	CreateMenu(level);
	    	iInterPhase = displayBuffer.cScreenType;
	    }

	    if (item == 0){
	    	PositionMoveRight();
	    }else {
	    	setPointerPos(item);
	    }

		status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}

// -------------------------------------------------
void MenuEngine::TimeoutNoKeyEntry (void)
{//timeout and return to main screen
//also need to clear the password
	if (status.getTimer(T_KEYBOARD_TIMEOUT) == 0)
	{
		FlattenStack();
	}
}


/**
 *
 * */
unsigned char MenuEngine::mSecTimeup (void *data1, void *data2)
{

	//takes the data (time delay period) at pointer *data1,
	//adds it to the start time pointed at by *data2.
	//If the the timer MS_TIMER is greater than the total
	//	then the time has expired.
	//It is convoluted to allow for the rollover from ff to 0
	//of MS_TIMER.
	//This function is non-reentrant.

	//PARAMETER1:    the address of the timer (so that there can be a multitude of timers)
	//PARAMETER2:    the address of the time the procedure was invoked

		//simply save the length (in mS) and the time the process is initailised
		unsigned long temp;
		//unsigned long temp2;
		unsigned long *p1;
		unsigned long *p2;
		unsigned char i;

		p1 = (unsigned long *)data1;
		p2 = (unsigned long *)data2;

		temp = millis();
		//temp2=*p1+*p2;

		if (*p2 > (*p1 + *p2)) //<<------------
		{//then a rollover condition will occur here
			//wait for rollover
			if (temp<*p2)
			{//rollover has occurred
				if (temp>(*p1  -(0xffffffff-*p2)))
				{//the time above 0 is calculated from
				 //the initial time to 0 and the to the delay
					i=1;
				}
				else {
					i=0;
				}
			}
			else {
				i=0;
			}

		}
		else {//here there will be no rollover
			if ( temp > (*p1 + *p2))
			{
				i=1;
			}
			else {
				i=0;
			}
		}
		return i;
}
//-------------------------------------------------

/**
*   PARAMETER1:    the address of the timer (so that there can be a multitude of timers)
*   PARAMETER2:    the address of the time the procedure was invoked
*	PARAMETER2:		the time in mS
*	simply save the length (in mS) and the time the process is initailised
 * */
void MenuEngine::mSecInitialise(void *data1, void *data2, unsigned long timerValue)
{
	//Take parameter2 and create the desired time by adding
	//it to the current mS timer. If there will be a rollover
	//compensation is provided

	//This function is non-reentrant.

		unsigned long *p1;
		unsigned long *p2;
		p1 = (unsigned long *)data1;
		p2 = (unsigned long *)data2;

		*p2 = millis();
		*p1 = timerValue;
}
