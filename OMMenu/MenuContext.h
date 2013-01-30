/*
 * MenuContext.h
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */
#include <inttypes.h>
#include "MenuScreens.h"

#ifndef MENUCONTEXT_H_
#define MENUCONTEXT_H_

#define WRITE_LINE1 0x0001		/*request write to line 1*/
#define WRITE_LINE2 0x0002		/*request write to line 2*/
#define WRITE_LINE3 0x0004		/*request write to line 3*/
#define WRITE_LINE4 0x0008		/*request write to line 4*/
#define TIME_DISABLE 0x0010	    /*disable time update on screen */
#define COLON_REQUEST 0x0020	/*write colon to screen */
#define MINUTES_CHANGED 0x0040	/*in order to update time on display */

#define PB_DELTA 0x0080	         /*valid pushbutton detected */
#define KEYBOARD_VALID 0x0100	 /* keyboard available for analysis */
#define TRANSFER_DISPLAY_ENABLE 0x0200	/* enable TransferMessage procedure */

#define UNUSED_PARAMETER 0xff
//unused to allow detection of unused display line

#define ACTIVITY_TIME 5	//every 500mS


#define NUMBER_OF_TIMERS 3
#define T_ACTIVITY 0	//flash colon
#define T_KEYBOARD_REPEAT 1 //timer for auto repeat of keyboard
#define T_KEYBOARD_TIMEOUT 2 //timeout of keyboard

#define NUMBER_OF_PARAMETERS 32

#define INVALID_IDX 0xFF

#define DYN_LIST_SIZE 7
//
struct fixListEntry {
  const char caption [LCD_WIDTH];
  uint8_t jumpLevel;
  uint8_t jumpItem;
};


/**
 * Contains timers and most important stateful  variables.
 * Support parameter edit protocol.
 * */
class MenuContext {
    //
	int32_t iParamValue[NUMBER_OF_PARAMETERS]; //parameters value array to edit
	//
	const static int32_t iParamMaxValue[NUMBER_OF_PARAMETERS];
	const static int16_t iParamMinValue[NUMBER_OF_PARAMETERS];
	const static uint16_t iStepParameters[NUMBER_OF_PARAMETERS];

    //dynamic list variables
	uint8_t dynListNode[DYN_LIST_SIZE]; //store bus_addr
	char dynList[DYN_LIST_SIZE][LCD_WIDTH + 1]; //store display name
	uint8_t dynListSize; //list size

    //ToDo define sizes
	const static fixListEntry fixedList1[3];//None,Camera,Bulb
    const static fixListEntry fixedList2[2];//Begin,StartOver
	const static fixListEntry fixedList3[2];//Yes,No
	const static char fixedList4[8][3];//
	const static char fixedList5[2][17];//

	uint16_t periodTimers[NUMBER_OF_TIMERS];


	uint16_t iKeyboardIncrement;
	uint8_t cFocusParameter;
	uint32_t iModifiableValue;
	uint16_t iStatus; //status byte with the following bit allocations


		//parameter stored to allow parameter to be modified
		//or cancelled by just pressing menu and not enter
    uint8_t keyCode;

    //uint8_t jumpItem;
    //uint8_t jumpLevel;



protected:
    uint8_t checkParamByIdx(uint8_t idx, uint32_t* pParam);
    uint8_t checkParamRange(uint32_t min, uint32_t max, uint32_t* pParam);

public:
	MenuContext();

//context interface
	unsigned int getContext() const {return iStatus;}
	void setContextBits(const uint16_t mask) { iStatus |= mask;}
	void clrContextBits(const uint16_t mask) { iStatus &= ~(mask);}
//timer interface
	unsigned int getTimer(const uint8_t idx)const {return periodTimers[idx];}
	void setTimer(const uint8_t idx, unsigned int value);
	void decTimer(const uint8_t idx);

//keyboard interface
	uint8_t getKeyboardCode() const {return keyCode;};
    void setKeyboardCode(const uint8_t);
    void raiseKeyboardIncrement();
    void setKeyboardIncrement(const uint8_t inc) {iKeyboardIncrement = inc;}

//parameter interface
    bool isParamEdit() const;
	void openParameter(const uint8_t idx);
	void incParameter();
	void decParameter();
	void closeParameter(bool saveFlag);
	/* return last position in string*/
	uint8_t formatParameterText(const uint8_t idx, uint8_t* cLineBuf, uint8_t* level, uint8_t* item);
	uint32_t readLiveParameter(uint8_t idxParam);

//list interface
	void resetList(void);
	void addToList(uint8_t addr, const char* name);
	uint8_t getListCount(void) const {return dynListSize;}
	uint8_t getMaxCount(void) const {return DYN_LIST_SIZE;}
	uint8_t getNodeAddr(uint8_t idx) {return dynListNode[idx];}

//navigation support
	//uint8_t getJumpLevel() const {return jumpLevel;};
	//uint8_t getJumpItem() const {return jumpItem;};

};

#endif /* MENUCONTEXT_H_ */
