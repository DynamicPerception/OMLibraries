/*

Common Line Handler Library

 OpenMoco MoCoBus Core Libraries

 See www.dynamicperception.com for more information

 (c) 2008-2012 C.A. Church / Dynamic Perception LLC

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

#include <avr/interrupt.h>
#include <avr/io.h>

#include "Arduino.h"
#include "OMComHandler_AT90USB.h"

//volatile uint8_t omc_pc_hist = 0xFF;
volatile uint8_t omc_slavetrip = 0;

/** Interrupt Service Routine for Pin Change on COM3
 */
void OMComHandler_AT90USB::_interrupt() {
    omc_slavetrip = 1;
}


void(*OMComHandler_AT90USB::f_isrCB)(uint16_t) = 0;
bool OMComHandler_AT90USB::m_which = false;
volatile unsigned long OMComHandler_AT90USB::m_isrUs = 0;

/** Class Constructor
  Creates a new instance of the class.  You may not create more
  than one of this class in any program.
  */

OMComHandler_AT90USB::OMComHandler_AT90USB() {
    m_isMaster = false;

    pinMode(OMC_COM1, INPUT);
    pinMode(OMC_COM2, INPUT);
    pinMode(OMC_COM3, INPUT);

    digitalWrite(OMC_COM1, HIGH);
    digitalWrite(OMC_COM2, HIGH);
    digitalWrite(OMC_COM3, HIGH);
}

void OMComHandler_AT90USB::_masterFollow() {
      // slave, attach an interrupt to the
      // common line, engage pull-up resistor

    pinMode(OMC_COM3, INPUT);
    digitalWrite(OMC_COM3, HIGH);

    // enable pin change interrupt for slave
    attachInterrupt(0, _interrupt, FALLING);
}

void OMComHandler_AT90USB::_masterLead() {
      // master, bring the common line high (bring low to signal slaves)

        // disable pcint
    detachInterrupt(0);

    pinMode(OMC_COM3, OUTPUT);
    digitalWrite(OMC_COM3, HIGH);
}

/** Watch A Common Input (COM1 or COM2)

 Enables the hardware interrupt for a given COM line.

 When a COM line is watched, it is set as an input and the
 internal pull-up is enabled.  If an external actor brings the pin LOW,
 a signal is tripped.  When the pin goes HIGH again the callback
 set by watchHandler() is called with the amount of time the (in uS)
 the signal was held low.

 @param p_which
 Which COM input to watch, 1 or 2.
 */

void OMComHandler_AT90USB::watch(uint8_t p_which) {

    p_which--;

    if( p_which > 1 )
        return;

    detachInterrupt(m_which);
    m_which = p_which;
    uint8_t pin = p_which == 0 ? OMC_COM1 : OMC_COM2;
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
    attachInterrupt(p_which, _isrFire, CHANGE);
}

/** Set Watch Callback Handler

 This handler will be called when a watched COM line makes a
 transition from high to low, and then back to high.  The
 provided function will be called and passed the length of
 the low pulse in microseconds.

 Your function must follow the prototype:

 @code
 void function(unsigned int)
 @endcode

 @param p_func
 A function pointer meeting the required prototype
 */

void OMComHandler_AT90USB::watchHandler( void(*p_func)(uint16_t) ) {
    f_isrCB = p_func;
}

/** Stop Watching a Common Input

 Stops watching a common input.

 */


void OMComHandler_AT90USB::stopWatch() {
    detachInterrupt(m_which);
}

/** Get Master Parameter

  Returns the current master parameter value.

  @return bool
  true if the instance is a master timing node, false if it is a slave
  */


bool OMComHandler_AT90USB::master() {
    return(this->m_isMaster);
}

/** Set Master Parameter

  Sets the current master parameter value.

  @param mast
  true if the instance is a master timing node, false if it is a slave
  */

void OMComHandler_AT90USB::master(bool p_mast) {
    this->m_isMaster = p_mast;

    if( this->m_isMaster) {
        this->_masterLead();
    }
    else {
        this->_masterFollow();
    }
}

/** Send Signal as Master

  Triggers the timing signal line (COM1), by bringing it LOW and
  signalling all slaves monitoring COM1. The line is held LOW for
  5mS.

  Note: if the instance is not set as master, this method will do
  nothing.

  Example:
  @code
    ComMgr.masterSignal();
    Serial.println("Sent Signal as Master!");
  @endcode

  */

void OMComHandler_AT90USB::masterSignal() {
    if( this->m_isMaster ) {
          // we're the master, send a 5mS low pulse on
          // common line
        digitalWrite(OMC_COM3, LOW);
        delay(5);
        digitalWrite(OMC_COM3, HIGH);
        }
}

/** Slave Clear to Go

  When the instance of the class is a slave, calling this method will
  indicate whether or not a master signal was received since the last
  time this method was called.

  The master signal flag is always reset to false after calling this method,
  and will not count how many times its triggered between calls to this method.

  Example:
  @code
  if( ComMgr.master() == false PCVECT) {
    if( ComMgr.slaveClear() == true ) {
        Serial.println("Received Signal From Master!");
    }
  }
  @endcode

  @return
  true if signal was received, false if not
  */

bool OMComHandler_AT90USB::slaveClear() {
    bool ret = omc_slavetrip;
    omc_slavetrip = 0;
    return(ret);
}

void OMComHandler_AT90USB::_isrFire() {

    uint8_t stat;

    if( m_which )
        stat = OMC_COM1_REG & (1 << OMC_COM1_PIN);
    else
        stat = OMC_COM2_REG & (1 << OMC_COM2_PIN);

     // if transitioned from high to low,
     // record start time
    if( ! stat ) {
        m_isrUs = micros();
    }
    else {
            // if a transition from low to high,
            // figure out diff and call callback (if set)
        unsigned long diff = micros() - m_isrUs;

        if( f_isrCB != 0 )
            f_isrCB(diff);
    }
}
