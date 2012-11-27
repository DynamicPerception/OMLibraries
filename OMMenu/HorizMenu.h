/*
 * HorizMenu.h
 *
 *  Created on: 04.09.2012
 *      Author: perepelitsa
 */

#ifndef HORIZMENU_H_
#define HORIZMENU_H_

#include <inttypes.h>

#include "MenuContext.h"
#include "MenuScreens.h"
#include "ActionScreen.h"
#include "LiquidCrystal.h"

#define LEFT_ARROW_GLYPH 3
#define RIGHT_ARROW_GLYPH 4
#define OPER_MAX 2
#define LIST_ITEMS_MAX 6

/****
 *
 * */
class HorizMenu {

	struct MenuBufferType {
		//This is the memory for the displays sized
		//for the maximum number of rows
		uint8_t cArrowsRequired;	//need to display selection arrows
		uint8_t cCurrentScreen; //in order to recreate later
		uint8_t cScreenType;
		//uint8_t cParamNum[MAX_SCREEN_SEL];	//parameter associated with item
		uint8_t cNextScreenNum[MAX_SCREEN_SEL];	//screen to draw when enter is pressed
	};

    /*callbacks for special actions, like load menu items*/
	//typedef void (*loadCB)(void);
	//loadCB operations[OPER_MAX];

	uint8_t cListBuf[LIST_ITEMS_MAX][LCD_WIDTH + 1];

	uint8_t cLineBuf1[LCD_WIDTH + 1];
	uint8_t cLineBuf2[LCD_WIDTH + 1];

	/* */
	uint8_t cItemsLimit;

	uint8_t levelToJump;
	uint8_t itemToJump;
	/* */
	unsigned int iLatchedStatus;

	/* reduce flicker by performing per-line update*/
	uint8_t cUpdateViewPhase;

	/* active menu item */
	uint8_t cPointerPos;


protected:
	//
	LiquidCrystal& disp;
	//
	MenuBufferType displayBuffer;
	//
	MenuContext& status;
	//
	ActionScreen actionScr;

protected:
	HorizMenu(LiquidCrystal& _lcd, Commander& _cmd);

protected:
	void CreateMenu (uint8_t menuIndex);
	void UpdateView (void);
	void TransferMenu(void);

	//navigation
	void PositionMoveLeft (void);
	void PositionMoveRight (void);
	uint8_t GoToDisplaySelected(void);
	void InvalidAction(void){};

	void IncreaseParameter (void);
	void DecreaseParameter (void);

	//
	void ReadParameter(uint8_t idx);
	void AnimateAction (void);

//inner interface
protected:
	void clearPointerPos(void);
	void setPointerPos(uint8_t lineNum){cPointerPos = lineNum;};
	uint8_t getPointerPos(void) const {return cPointerPos;};
	void ResetDisplay();
	void setHandler(uint8_t, void (*) (void));

	//navigation support
	uint8_t getJumpLevel() const {return levelToJump;}
	uint8_t getJumpItem() const {return itemToJump;}

protected:
	int WriteLine (const uint8_t *bufferStart, uint8_t startLine, uint8_t lineOffset);

};

#endif /* HORIZMENU_H_ */
