/*
 * MenuEngine.h
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#ifndef MENUENGINE_H_
#define MENUENGINE_H_

#include <inttypes.h>

#include "AnlgBtnInput.h"
#include "HorizMenu.h"
#include "MenuContext.h"
#include "Commander.h"

#define HUNDRED_MS 100

#define MS_TIMER 0

/**
 * Main class of menu library.
 * method processUI() should be called in endless loop.
 * Provides finite state machine for menu system.
 * Interaction with outer system via MenuContext parameters.
 * Notification update is not implemented yet.
 * */
class MenuEngine: public HorizMenu
{
	//time stamps to calculate time
	uint32_t mSTimer0;
	uint32_t mSTimer0_Freeze;

	uint8_t iEnginePhase; //former iPhase
	uint8_t iInterPhase;
	uint8_t cActivity;

	uint8_t _led;

	uint8_t mode;

	struct StackType {
		unsigned char cStackPointer;
		unsigned char cScreenCreatedToGetHere[MAX_SCREEN_SEL];
		unsigned char cTopLine[MAX_SCREEN_SEL];
		unsigned char cPointerLine[MAX_SCREEN_SEL];
		unsigned int iCallingInterPhase[MAX_SCREEN_SEL];
		};

	StackType ProcessStack;

protected:

    AnlgBtnInput keyboard;

public:
	MenuEngine(LiquidCrystal& _lcd, Commander& _cmd);

	void ProcessUI(void);

	//async functions

protected:
	void InitialiseDisplay(void) {ResetDisplay();}
    //
	unsigned char mSecTimeup (void *data1,void *data2);
	void mSecInitialise(void *data1, void *data2, unsigned long timerValue);
	//
	void PopStack (void);
	void FlattenStack (void);
	void PushStack (void);

	void OverallProcess(void);
	uint8_t Keyboard(void);
	void DisplayInterfaceManager(void);
	void TimeoutNoKeyEntry (void);

	//
	void DisplaySelectedItem(void);
	void NavigateNext(void);

	//
	uint8_t ChangeMode();
};

#endif /* MENUENGINE_H_ */
