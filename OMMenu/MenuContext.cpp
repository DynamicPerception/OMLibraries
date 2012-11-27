/*
 * MenuContext.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#include "MenuContext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))


const uint32_t MenuContext::iParamMaxValue[NUMBER_OF_PARAMETERS] PROGMEM = {
	0,16,16,(99*3600UL+59*60UL+59UL),1, //0-4
	1,99999,65535,65535,65535, //5-9
	7,(99*3600UL+59*60UL+59UL),0,0,1, //10 - 14
	2,3000,400,0x2459,50, //15-19
	2,1,1,7,1, //20-24
	1,100,100,0,0,0,0 //25-31
	};

const uint16_t MenuContext::iParamMinValue[NUMBER_OF_PARAMETERS] PROGMEM = {
		0,0,0,0,0, //0-4
		0,0,0,0,0, //5-9
		1,60,0,0,0, //10-14
		0,0,0,0,0, //15-19
		0,0,0,0,0, //20-24
		0,0,0,0,0,0,0 //25-31

	};

const uint16_t MenuContext::iStepParameters[NUMBER_OF_PARAMETERS] PROGMEM = {
    1,1,1,1,1, //0-4
    1,1,1,1,1, //5-9
    1,1,1,1,1, //10-14
    1,1,1,1,1, //15-19
    1,1,1,1,1, //20-24
    1,1,1,1,1,1,1 //25-31
};

//--param20
const fixListEntry MenuContext::fixedList1[3] PROGMEM = {
		{"None", CREATE_FILM_WIZARD, 7},
		{"Camera", CREATE_FILM_WIZARD, 5},
		{"Bulb", CREATE_FILM_WIZARD, 4}
};//
//--param21
const fixListEntry MenuContext::fixedList2[2] PROGMEM = {
	  //1234567890123456
		{"Begin Film",INITIAL_DISPLAY_SCREEN , 1},
		{"Again", CREATE_FILM_WIZARD, 1}
};//
//--param22
const fixListEntry MenuContext::fixedList3[2] PROGMEM = {
		{"Yes", CREATE_FILM_WIZARD, 1},
		{"No", CREATE_FILM_WIZARD, 2}
};//
//param23
const char MenuContext::fixedList4[8][3] PROGMEM = {
		{"12"},//0
		{"24"},//1
		{"26"},//2
		{"28"},//3
		{"30"},//4
		{"36"},//5
		{"48"},//6
		{"60"},//7
};//
//param24
const char MenuContext::fixedList5[2][17] PROGMEM = {
	  //1234567890123456
		{"Shoot-mov-Sho"},
		{"Continuous"}
};

/****
 *
 * */
MenuContext::MenuContext()
  : iKeyboardIncrement(1), cFocusParameter(INVALID_IDX), iStatus(0)
{

	for (uint8_t i=0; i < NUMBER_OF_TIMERS; i++) {
		periodTimers[i]=0;
	}

	for (uint8_t i=0; i < NUMBER_OF_PARAMETERS; i++) {

		iParamValue[i] = i;
		}

	iParamValue[11] = (18*3600UL + 59*60UL + 59UL);
	iParamValue[25] = 0;

    resetList();

}


/**
 *   1. Lead-in time (in seconds]
 *   2. Lead-out time (in seconds]
 *   3. Acceleration time (in seconds]
 *   4. Deceleration time (in seconds]
 *   5. Exposure time (in milliseconds]
 *   6. "film length" of film
 *   Time:
 *   11. time1
 *   12.
 *   13.
 *   14. time2
 *   Lists:
 *   20
 * */
uint8_t MenuContext::formatParameterText(const uint8_t idx, uint8_t* cLineBuf, uint8_t* level, uint8_t* item )
{
   uint32_t value = readLiveParameter(idx);
   *level = 0;
   *item = 0;

   uint8_t len = 0;
   //temporary init as
   cLineBuf[0] = '[';
   cLineBuf[1] = '=';
   char* ptrStart = (char*)&cLineBuf[2];
	if ((idx > 0) && (idx < 10))  {
	//integer type
	   sprintf(ptrStart, "%lu", value );
   }
   /*dynamic list, very special case*/
   if (idx == 10) {
	   uint32_t listIdx = value;
	   checkParamByIdx(idx, &listIdx);
	   sprintf(ptrStart, "%s", &dynList[listIdx-1][0] );
	   if (listIdx == dynListSize) {
	     *item = 2;
	     *level = CREATE_FILM_WIZARD;
	   } else {
		 *item = 1;
		 *level = AXIS_WIZARD;
	   }
   }
  /* magic 11(12)(13) or 14:15:16 is time type*/
   if ((idx == 11) || (idx == 14)){
	   uint32_t tmp = value;
	   uint8_t hour = tmp/3600;
	   //tmp = tmp - ((uint32_t)hour * 3600UL); // may be
	   tmp = tmp%3600;
	   uint8_t minutes = tmp/60;
	   //tmp = tmp - ((uint32_t)minutes * 60UL); //
	   tmp = tmp%60;
	   uint8_t secs = tmp;
	   sprintf(ptrStart, "%02d:%02d:%02d", hour, minutes, secs);
   }
   /*magic 20,21... is list type*/
   if ((idx >= 20) && (idx < NUMBER_OF_PARAMETERS))
   {
	   uint32_t listIdx = value;
	   checkParamByIdx(idx, &listIdx);
       if (idx == 20){
         strcpy_P(ptrStart, &fixedList1[listIdx].caption[0]);
         *item = pgm_read_byte(&fixedList1[listIdx].jumpItem);
         *level = pgm_read_byte(&fixedList1[listIdx].jumpLevel);

       } else if (idx == 21){
    	 strcpy_P(ptrStart, &fixedList2[listIdx].caption[0]);
    	 *item = pgm_read_byte(&fixedList2[listIdx].jumpItem);
    	 *level = pgm_read_byte(&fixedList2[listIdx].jumpLevel);

       } else if (idx == 22){
      	 strcpy_P(ptrStart, &fixedList3[listIdx].caption[0]);
      	 *item = pgm_read_byte(&fixedList3[listIdx].jumpItem);
      	 *level = pgm_read_byte(&fixedList3[listIdx].jumpLevel);

       } else if (idx == 23){
   	     strcpy_P(ptrStart, &fixedList4[listIdx][0]);

       } else if (idx == 24){
      	 strcpy_P(ptrStart, &fixedList5[listIdx][0]);
       } else {
    	 //strcpy_P((char*)cLineBuf, PSTR("--noList--"));
       }
   }
   len = strlen((char*)cLineBuf);
   return len;
}

/**
 *
 * */
uint32_t MenuContext::readLiveParameter(uint8_t idxParam)
{
/* take stored parameter or working copy*/
	if (cFocusParameter == INVALID_IDX) {
	   return iParamValue[idxParam];
   } else {
	   return iModifiableValue;
   }
}
/**
 *
 * */
void  MenuContext::setKeyboardCode(const uint8_t key)
{
	keyCode = key;
}

/**
 *
 * */
void MenuContext::openParameter(const uint8_t idx){
	cFocusParameter = idx;
	iModifiableValue = iParamValue[idx];
}

/***
 *
 * */
bool MenuContext::isParamEdit() const
{
	return (cFocusParameter != INVALID_IDX);
}

/**
 *
 * */
void MenuContext::incParameter(){
	//we have gap so we can just increment and check after
	uint16_t step;
	memcpy_P(&step, &iStepParameters[cFocusParameter], sizeof(step));
	iModifiableValue += step*iKeyboardIncrement;
	checkParamByIdx(cFocusParameter, &iModifiableValue);
}

/****
 *
 * */
void MenuContext::decParameter(){
	uint16_t step;
	memcpy_P(&step, &iStepParameters[cFocusParameter], sizeof(step));
	//check for roll over zero
	uint32_t tmp = iModifiableValue;
	tmp -= step*iKeyboardIncrement;
	if (checkParamByIdx(cFocusParameter, &tmp)) {
	  iModifiableValue -= step*iKeyboardIncrement;
	}
}
/***
 *
 * */
void MenuContext::closeParameter(bool saveFlag){
	if (saveFlag) {
		iParamValue[cFocusParameter] = iModifiableValue;
	}
	cFocusParameter = INVALID_IDX;
}

/**
 *
 * */
void MenuContext::setTimer(const uint8_t idx, const uint16_t value){
	periodTimers[idx] = value;
}

/**
 *
 * */
void MenuContext::decTimer(const uint8_t idx){
	periodTimers[idx]--;
}


uint8_t MenuContext::checkParamByIdx(const uint8_t idx, uint32_t* pParam)
{
	uint16_t paramMinValue;
	uint32_t paramMaxValue;
	//copy across FLASH space
	memcpy_P(&paramMinValue, &iParamMinValue[idx], sizeof(paramMinValue));
	memcpy_P(&paramMaxValue, &iParamMaxValue[idx], sizeof(paramMaxValue));

	return checkParamRange(paramMinValue, paramMaxValue, pParam);
}

/**
 *
 * */
uint8_t MenuContext::checkParamRange(uint32_t min, uint32_t max, uint32_t* pParam)
{
	uint8_t result = 0;
	if (*pParam < min) {
			*pParam = min;
		} else if (*pParam > max) {
			*pParam = max;
		} else {
			result = 1;
		}
	return result;
}

/**
 *
 * */
void MenuContext::raiseKeyboardIncrement()
{
    if (iKeyboardIncrement < 10000)
	 {
		iKeyboardIncrement *=10;
	}
}

/**
 *
 * */
void MenuContext::resetList(void)
{
//init it here
	dynListSize = 0;
	//--debug
	//for (uint8_t i=0; i < 6; i++) {
	//	sprintf(&dynList[i][0], "entry_%d", i+1);
	//	dynListSize++;
	//}

	sprintf(&dynList[dynListSize][0], "NEXT");
	dynListSize++;
}


/**
 *
 * */
void MenuContext::addToList(uint8_t addr, const char* name)
{
	if (dynListSize < DYN_LIST_SIZE) {
	  dynListNode[dynListSize] = addr;
	  sprintf(&dynList[dynListSize][0], "%d_%s", addr, name);
	  dynListSize++;
	}
}

