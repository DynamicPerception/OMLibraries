/*

State Manager Library

OpenMoco nanoMoCo Core Engine Libraries 

See www.openmoco.org for more information

(c) 2008-2011 C.A. Church / Dynamic Perception LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/


#ifndef	OM_COMSTATE_H
#define OM_COMSTATE_H


#include <inttypes.h>
#include "Arduino.h"

#define OM_STATE_MAX 16

/**

  @brief
  Simple State Machine
  
  The Simple State Machine class provides the ability to manage transitions
  between states of a program to allow for easy development of Finite State
  Machines based on simple numeric state codes.
  
  It provides all means necessary for a code author to describe a series
  of states in which the engine may exist, callbacks to be executed when the
  engine is in a given state, and the ability to change or read the current
  state of the engine.
  
  Basic Example:
  
   @code
  
	#include "OMState.h"
	
	 // three possible states: 0, 1, and 2
	OMState State = OMState(3);
	
	void setup() {
	  
	  State.setHandler(1, state1);
	  State.setHandler(2, state2);
	  State.state(1);
	  
	  Serial.begin(19200);
	}
	
	void loop() {
	  State.checkCycle();
	}
	
	void state1() {
	  
	  Serial.println("I'm in state 1!");
	  State.state(2);
	}
	
	void state2() {
	  
	  Serial.println("I'm in state 2!");
	  
	  	// put state machine into "idle" by setting a state
	  	// with no callback associated
	  	
	  State.state(0);
	}
  
  @endcode
  
  The core concepts are as follows:
  
  You must define a series of states by creating a function to be called
  when in that particular state, and you should always have at least
  one state that has no callback (0 is recommended).  The maximum number
  of states allowed is defined by OM_STATE_MAX - you may not specify
  a higher number of states than this.
  
  Every time checkCycle() is called, if the current state has a callback
  associated, that callback will be executed.  If no callback is associated
  with the current state, the engine will remain idle - that is, nothing will
  be executed. 
  
  It is the responsibility of the author to ensure that transitions to
  subsequent states are handled in their callbacks!

  @author C. A. Church

  (c) 2011 C. A. Church / Dynamic Perception
  
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

  */
  
class OMState {

private:

	uint8_t m_curState;
	uint8_t m_maxStates;
	typedef void (*stateCB)(void);
	stateCB m_transit[OM_STATE_MAX];	
	
public:

	OMState(uint8_t);
	
	void state(uint8_t);
	uint8_t state();
	
	void checkCycle();
	
	void setHandler(uint8_t, void (*) (void));
	void clearHandler(uint8_t);
};

#endif

