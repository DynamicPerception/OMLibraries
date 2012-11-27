/*
 * HorizMenu.cpp
 *
 *  Created on: 04.09.2012
 *      Author: perepelitsa
 */

#include "HorizMenu.h"

HorizMenu::HorizMenu(LiquidCrystal& _lcd, Commander& _cmd)
 : disp(_lcd), status(_cmd.GetWrContext()), actionScr(_cmd)
{
	cUpdateViewPhase = 0;
}

/**
 * Reset display.
 * Called from child class to reset display.
 * */
void HorizMenu::ResetDisplay(){
	disp.begin(LCD_WIDTH, LCD_LINES);
    Resources::readGlyph(3,cLineBuf2);
	disp.createChar(LEFT_ARROW_GLYPH,cLineBuf2);
	Resources::readGlyph(2,cLineBuf2);
	disp.createChar(RIGHT_ARROW_GLYPH,cLineBuf2);
    disp.clear();
    disp.home();
}

/**
 * Update the display based on several requests in status.
 * Called by framework.
 * Updates one line per call for smooth look.
 * */
void HorizMenu::UpdateView (void)
{
	switch (cUpdateViewPhase)
	{
		case 0:
		//check if any update is needed
		//including the flashing of the colon in the time display
			iLatchedStatus = status.getContext();
			status.clrContextBits(WRITE_LINE1 | WRITE_LINE2);
			//so a request is not lost when this process is writing a line
			//and a new request is made.
			if ((iLatchedStatus & (WRITE_LINE1 | WRITE_LINE2)) ||
			     ((status.getContext() & (TIME_DISABLE | COLON_REQUEST)) == COLON_REQUEST)) {
				cUpdateViewPhase++;
			}
	    break;
		case 1:
			if (iLatchedStatus & WRITE_LINE1)
			{
				WriteLine(cLineBuf1, 0, 0);
				iLatchedStatus &= ~WRITE_LINE1;
			}
			cUpdateViewPhase++;
		break;
		case 2:
			if (iLatchedStatus & WRITE_LINE2)
			{
				WriteLine(cLineBuf2, 1, 0);
				iLatchedStatus &= ~WRITE_LINE2;
				status.clrContextBits(COLON_REQUEST);
			}
			cUpdateViewPhase = 0;

		break;

		default:
			cUpdateViewPhase=0;
			break;
	}
}


/**
 * Copy from buffer to frame buffer with offset.
 * Draw arrows.
 * Set update flags for all lines.
 *
 * */
void HorizMenu::TransferMenu(void)
{//starting at the top line
  if (status.getContext() & TRANSFER_DISPLAY_ENABLE)
	{
	  //clear so doesn't continuously execute
	  //every time it is called
	  status.clrContextBits(TRANSFER_DISPLAY_ENABLE);

	  //set navigation goes normal way
	  levelToJump = 0;
	  itemToJump = 0;
	  uint8_t msgNum = Resources::readMsgNum(displayBuffer.cCurrentScreen, cPointerPos);
	  uint8_t paramNum = Resources::readParamNum(displayBuffer.cCurrentScreen, cPointerPos);
	  if (displayBuffer.cScreenType == MENU_LEVEL_ENTRY) {
		   //copy the associated message across PGM space
		   Resources::readMsgToBuf(msgNum, cLineBuf2);
	  } else if (displayBuffer.cScreenType == DIALOG_LEVEL_ENTRY) {
		  uint8_t procNum = Resources::readSharedParam(displayBuffer.cCurrentScreen);
		  uint32_t idx = status.readLiveParameter(procNum);
		  //copy two lines together
		  msgNum = Resources::readMsgNum(displayBuffer.cCurrentScreen, idx);
		  Resources::readMsgToBuf(msgNum, cLineBuf1);
		  msgNum = Resources::readMsgNum(displayBuffer.cCurrentScreen, idx+1);
		  Resources::readMsgToBuf(msgNum, cLineBuf2);
	  } else {
			Resources::readMsgToBuf(msgNum, cLineBuf1);
			// load parameter value to second line
			// cFocusParameter set in GoToDisplaySelected()
			//uint8_t procNum = Resources::readProcNum(displayBuffer.cCurrentScreen, cPointerPos);
			if (displayBuffer.cNextScreenNum[cPointerPos] == OPEN_EDIT) {
			    //may affect navigation
				ReadParameter(paramNum);
			} else if (displayBuffer.cNextScreenNum[cPointerPos] == OPEN_ACTION) {
			  AnimateAction();
			} else {
			  //not
			}

	  }

		cLineBuf1[LCD_WIDTH] = 0;
		cLineBuf2[LCD_WIDTH] = 0;

		//decoration with arrows or another glyph
		if (displayBuffer.cArrowsRequired)
		{
			uint8_t* p_left = 0;
			uint8_t* p_right = 0;
			if (displayBuffer.cScreenType == MENU_LEVEL_ENTRY) {
				p_left = &cLineBuf2[0];
				p_right = &cLineBuf2[LCD_WIDTH - 1];
			} else if ((displayBuffer.cScreenType == WIZARD_LEVEL_ENTRY)||
					   (displayBuffer.cScreenType == PAGES_LEVEL_ENTRY))
			  {
				p_left = &cLineBuf1[0];
				p_right = &cLineBuf1[LCD_WIDTH - 1];
			  }
			//left - right arrows,
			// position 0 is for menu header
			if (cPointerPos > 1)
			{
				if (displayBuffer.cScreenType == WIZARD_LEVEL_ENTRY) {
					*p_left = '#';
				} else {
				    *p_left = LEFT_ARROW_GLYPH;
				}
			}
			if (cPointerPos < cItemsLimit - 1)
			{
				*p_right = RIGHT_ARROW_GLYPH;

			}
		}
		status.setContextBits(WRITE_LINE1 | WRITE_LINE2 );
	}
}


/***
 *
 **/
void HorizMenu::CreateMenu (uint8_t In_msgNum)
{
	//transferring all relevant information from the screen
	//to the display buffer
	displayBuffer.cArrowsRequired = Resources::readArrowsReq(In_msgNum);
	displayBuffer.cCurrentScreen = In_msgNum;
    //uint8_t msgNum[4] = {0,1,2,3};
	uint8_t cActualNumberOfItems = Resources::readNumberOfItems(In_msgNum);
	displayBuffer.cScreenType = Resources::readMenuType(In_msgNum);
	for (uint8_t i=0; i < cActualNumberOfItems; i++)
	{//do for number of lines in the particular display
		//so we know what to do next
		displayBuffer.cNextScreenNum[i] = Resources::readNextScreen(In_msgNum,i);
		//so we know what and display
	}

	//copy the 0th message across PGM space as menu header
	uint8_t msgNum = Resources::readMsgNum(In_msgNum, 0);
	Resources::readMsgToBuf(msgNum, cLineBuf1);

	//do not count header
	cItemsLimit = cActualNumberOfItems;

	//and force update when transfer message next seen
	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}



/**
 * Navigate menu items up direction
 * 0th element is header
 * */
void HorizMenu::PositionMoveLeft (void)
{
	if (cPointerPos > 1)
		cPointerPos--;

	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}

/**
 * Navigate menu items down direction
 * */
void HorizMenu::PositionMoveRight (void)
{
	if (cPointerPos < cItemsLimit-1) {
		cPointerPos++;
	}
	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}


void HorizMenu::IncreaseParameter (void)
{
	status.incParameter();
	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}


void HorizMenu::DecreaseParameter (void)
{
	status.decParameter();
	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}
/**
 * Open menu item.
 * Set cFocusParameter
 * */
uint8_t HorizMenu::GoToDisplaySelected(void)
{
	uint8_t phase = INITIATE_LEVEL;
	uint8_t cNextScreen = displayBuffer.cNextScreenNum[cPointerPos];
	uint8_t focusParam = Resources::readParamNum(displayBuffer.cCurrentScreen, cPointerPos);

	switch (cNextScreen) {
	case OPEN_EDIT:
	case OPEN_ACTION:
		 //take the parameter to use later.
		 //if this is not a parameter entry, then no harm is done.
		 status.openParameter(focusParam);
		 status.setContextBits(TRANSFER_DISPLAY_ENABLE);
		 if (cNextScreen == OPEN_EDIT) {
		  phase = PARAM_ITEM_EDIT;
		 } else {
		  phase = ACTION_SCREEN;
		 }
	    break;

	default:
		 CreateMenu(cNextScreen);
	     //must be done separately to allow correct display creation
		 clearPointerPos();
		 phase = displayBuffer.cScreenType;
		break;
	}
	return phase;
}

/**
 * Sets initial positions of frame and pointer in new menu screen.
 * */
void HorizMenu::clearPointerPos(void)
{
	//must be done separately to allow correct display creation
	cPointerPos = 1;
	status.setContextBits(TIME_DISABLE);
	//prevent any further time update at deeper levels
}

/**
 *
 * */
void HorizMenu::ReadParameter(uint8_t idx)
{
	uint8_t len = status.formatParameterText(idx, &cLineBuf2[0],&levelToJump,&itemToJump);
	//fill blanks to the end
	if (LCD_WIDTH - len > 0) {
	 memset(&cLineBuf2[len+1], ' ', LCD_WIDTH - len );
	 cLineBuf2[len] = ']';
	}
	cLineBuf2[0] = '[';
	if (status.isParamEdit()) {
	  cLineBuf2[1] = '*';
	} else {
	  cLineBuf2[1] = '=';
	}
}

/**
 * display animations
 * and call for callback
 * */
void HorizMenu::AnimateAction(void)
{
	if (actionScr.isAction()) {
	  //fill blanks to the end
	  uint8_t pos = actionScr.getMarkerPos();
	  memset(&cLineBuf2[2], ' ', LCD_WIDTH - 5 );
	  if (actionScr.getMoveDir()) {
	   cLineBuf2[0] = '>';
	   cLineBuf2[1] = '>';
	   cLineBuf2[pos] = '>';
	  } else {
	   cLineBuf2[0] = '<';
	   cLineBuf2[1] = '<';
	   cLineBuf2[pos] = '<';
	  }
	} else {
	  memset(&cLineBuf2[0], '-', LCD_WIDTH );
	  cLineBuf2[8] = '|';
	}
}

/**
 * Writes line from buffer to given position on LCD
 * buffer is zero terminated
 * May use String class
 *
 * */
int HorizMenu::WriteLine (const uint8_t *bufferStart, uint8_t startLine, uint8_t lineOffset)
{
 disp.setCursor(lineOffset, startLine);
 disp.print((const char*)bufferStart);
 return 0;
}
