/*
 * ScreenType.h
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#ifndef SCREENTYPE_H_
#define SCREENTYPE_H_

#define LCD_WIDTH 16 // max width of LCD screen
#define LCD_LINES 2

#define MAX_SCREEN_SEL 10 // max of menu items per menu

//represents one line on screen, including menu header
struct menuItem
{
  uint8_t cMsgNum;      //message to be displayed from resource
  uint8_t cProcNum;     //process to be done on item
  uint8_t cNextMenu;	//what to display when enter is pressed (not used for param entry)
  uint8_t cParamNum;	//parameter associated with item
};

//represents appearance of menu in per-line (items) basis
struct MenuScreen {
uint8_t cNumberOfItems;    //
uint8_t cArrowsRequired;   //need to display selection arrows
uint8_t cMenuType;         //
uint8_t cLoadProcess;      //specify callback called on enter menu, or shared param
const struct menuItem items[MAX_SCREEN_SEL];
};

#endif /* SCREENTYPE_H_ */
