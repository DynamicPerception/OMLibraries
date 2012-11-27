/*
 * AnlgBtnInput.cpp
 *
 *  Created on: 11.09.2012
 *      Author: perepelitsa
 */

#include "AnlgBtnInput.h"

AnlgBtnInput::AnlgBtnInput(uint8_t a_pin)
:_an_pin(a_pin),last_but_rd(1013) {
	  // setup button input
	 pinMode(_an_pin, INPUT);
	   // enable internal pull-up
	 digitalWrite(_an_pin, HIGH);
}

AnlgBtnInput::~AnlgBtnInput() {

}

/** see which buttons are pressed
* buttons are on one analog pin, the value
* determines which button, if any is pressed
**/
uint8_t AnlgBtnInput::sampleButtons(int* read_value)
{
	uint8_t scan = IDLE_SCAN;
     // read analog input
	*read_value = analogRead(_an_pin); //they do adjust pin number

     // don't let it flip in a single read
    if( abs(last_but_rd - *read_value) > BUT_THRESH ) {
      last_but_rd = *read_value;
      return scan;
    }

 //window comparator logic (bit 0 is not used)
 if( *read_value > (BUT0_VAL - BUT_THRESH) && *read_value < (BUT0_VAL + BUT_THRESH) ) {
	 bitSet(scan, 1);
 }
 else if( *read_value > (BUT1_VAL - BUT_THRESH) && *read_value < (BUT1_VAL + BUT_THRESH) ) {
	 bitSet(scan, 2);
 }
 else if( *read_value > (BUT2_VAL - BUT_THRESH) && *read_value < (BUT2_VAL + BUT_THRESH) ) {
	 bitSet(scan, 3);
 }
 else if( *read_value > (BUT3_VAL - BUT_THRESH) && *read_value < (BUT3_VAL + BUT_THRESH) ) {
	 bitSet(scan, 4);
 }
 else if( *read_value > (BUT4_VAL - BUT_THRESH) && *read_value < (BUT4_VAL + BUT_THRESH) ) {
	 bitSet(scan, 5);
 }
 else {
	 scan = IDLE_SCAN;
 }

 return scan;
}
