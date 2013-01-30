/*
 * MenuScreens.h
 *
 *  Created on: 12.06.2012
 *      Author: perepelitsa
 */

#ifndef MENUSCREENS_H_
#define MENUSCREENS_H_

#include <avr/pgmspace.h>
#include "ScreenType.h"


#define BLANK_MESSAGE 25 //index
#define NUMBER_OF_STRINGS 26


/* menu processing levels (states)
 * defines how to display screen from resources
 * */
enum {INITIATE_LEVEL= 1,
	  DIALOG_LEVEL_ENTRY, /* allowed, individual lines from resources, position have no mean*/
	  MENU_LEVEL_ENTRY, /* allowed, first item as header */
	  PAGES_LEVEL_ENTRY, /* wizard - like two side and escape*/
	  WIZARD_LEVEL_ENTRY, /* allowed, no header, derivative states from here */
	  PARAM_ITEM_EDIT, /* derivative state, not allowed for resource*/
	  ACTION_SCREEN  /* derivative state, not allowed for resource */
};

/* operations with menu*/
enum {
	OP_NONE,
	OP_FILL_ITEM_LIST
};

/** enumeration with screen numbers in array */
enum MenuOrder {
	INITIAL_DISPLAY_SCREEN, /* mandatory */
	MENU_SCREEN,            /* user defined */
	CREATE_FILM_WIZARD,     /* user defined */
	RESCAN_DEVICES,         /* user defined */
	SETTINGS_SCREEN,        /* user defined */
	AXIS_WIZARD,            /* user defined */
	MAX_SCREENS,            /* mandatory, end of real entries */
	OPEN_EDIT,              /* mandatory, fake entry: opens parameter edit */
	OPEN_ACTION             /* mandatory */
};

/**
 *
 * */
class Resources {
	protected:
	  const static MenuScreen screens[MAX_SCREENS];
	  const static char glyphs[8][8];
	  //string index maintained manually
	  const static char resources[NUMBER_OF_STRINGS][LCD_WIDTH+1];
public:
 /**
  * buffer must be 8 bytes long
  * */
 static void readGlyph(uint8_t glyphNumber, uint8_t* buffer) {
	 memcpy_P((void*)buffer, &Resources::glyphs[glyphNumber][0], 8);
 };
 /**
  *
  * */
 static uint8_t readMsgNum(uint8_t screen, uint8_t item){
	 return (pgm_read_byte(&Resources::screens[screen].items[item].cMsgNum));
 };
 /**
  *
  * */
 static uint8_t readProcNum(uint8_t screen, uint8_t item){
	 return (pgm_read_byte(&Resources::screens[screen].items[item].cProcNum));
 };


 static uint8_t readNextScreen(uint8_t screen, uint8_t item) {
	 return (pgm_read_byte(&Resources::screens[screen].items[item].cNextMenu));
 };

 static uint8_t readParamNum(uint8_t screen, uint8_t item) {
	 return (pgm_read_byte(&Resources::screens[screen].items[item].cParamNum));
 };

 static uint8_t readNumberOfItems(uint8_t screen) {
	 return (pgm_read_byte(&Resources::screens[screen].cNumberOfItems));
 };

 static uint8_t readArrowsReq(uint8_t screen) {
     return (pgm_read_byte(&Resources::screens[screen].cArrowsRequired));
 };

 static uint8_t readMenuType(uint8_t screen){
 	 return (pgm_read_byte(&Resources::screens[screen].cMenuType));
  };

 static uint8_t readSharedParam(uint8_t screen){
  	 return (pgm_read_byte(&Resources::screens[screen].cLoadProcess));
   };

 /**
  * Transfer message from FLASH stored resource to normal memory.
  * Do not change array access. This variant is better.
  * */
 static void readMsgToBuf(uint8_t messageNumber, void* destination) {
	 strcpy_P((char*)destination, &Resources::resources[messageNumber][0]);
 }

// const static char d[LCD_WIDTH+1];
};



#endif /* MENUSCREENS_H_ */
