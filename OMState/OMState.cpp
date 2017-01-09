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

#include <stdlib.h>
#include "OMState.h"

/** Class Constructor

 Creates a new finite state machine (engine), takes a count of total states up
 to the specified max number of states. 
 
 note: states are zero-indexed, so specifying 8 creates states 0-7.
 
 If you request more than the maximum allowed state values, the engine will
 be initialized to maximum allowed.
 
 @param states
 Number of state transitions in the engine
 
 */
 
OMState::OMState(uint8_t states) {

    memset(m_transit, 0, sizeof(stateCB) * OM_STATE_MAX);
    m_curState = 0;
    
    states = states > OM_STATE_MAX ? OM_STATE_MAX : states;
    
    m_maxStates = states;
}


/** Get Current State

 Retrieves the current state of the engine.
 
 @return 
 Current state
 */
 
uint8_t OMState::state() {
    return(m_curState);
}

/** Set Current State

 Sets the current state of the engine, you must use this to transition the
 engine from one state to another.
 
 */
 
void OMState::state(uint8_t p_State) {
    m_curState = p_State;
}

/** Run Check Cycle

 Runs a Check Cycle against the state machine.  If there is a callback
 associated with the current state, that callback is executed.  If no
 callback is associated, no action is taken.
 
 */
 
void OMState::checkCycle() {
    if( m_curState >= m_maxStates )
        return;
    
    if( m_transit[m_curState] != 0 ) {
        stateCB ptr = m_transit[m_curState];
        ptr();
    }
}

/** Set State Handler

 Associates a callback with the given state. If you provide a state code
 that is higher than the maximum state, it fails silently.  The function
 must take no arguments and return void.

 Example:
 
 @code
 
 ...
 
 OMState Engine = OMState(2);
 
 ...
 
 void foo() {
   // .. do something
 }
 
 void setup() {
 
   Serial.begin(19200);
   
   Engine.setHandler(1, foo);
   
 }
 
 @endcode   
 
 @param p_State
 The state code to associate with
 
 @param p_cb
 A function pointer to a function that takes no arguments and returns void
 
 */
 
 
void OMState::setHandler(uint8_t p_State, stateCB p_cb) {
    if( p_State >= m_maxStates )
        return;
    
    m_transit[p_State] = p_cb;
}

/** Clear State Handler

 Remove callback associated with given state.
 
 @param p_State
 State code to remove association from.
 */

void OMState::clearHandler(uint8_t p_State) {
    if( p_State >= m_maxStates )
        return;
    
    m_transit[p_State] = 0;
}

