/*

 Camera Functionality Library

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


/*

  ========================================
  Camera Control Library
  ========================================

*/


#include "OMCamera.h"
#include "Arduino.h"
#include <MsTimer2.h>

 // initialize static members

 bool OMCamera::m_isBzy = false;
 void(*OMCamera::f_camSignal)(uint8_t) = 0;			// Pointer to camera callback function, camCallBack()

 uint8_t OMCamera::m_shutter1 = OM_DEFSHUTTER1;
 uint8_t OMCamera::m_shutter2 = OM_DEFSHUTTER2;
 uint8_t OMCamera::m_focus1 = OM_DEFFOCUS1;
 uint8_t OMCamera::m_focus2 = OM_DEFFOCUS2;
 uint8_t OMCamera::m_curAct = OM_CAM_INCLR;

 bool	 OMCamera::m_debug = false;

/** Constructor

 Constructs a new instance of the class, with the default pin assignments
 for Shutter and Focus pins (13, 12).

 */

OMCamera::OMCamera() {
	this->_init();
}

/** Constructor

 Constructs a new instance of the class, with the specified pin assignments

 @param c_shut
 The arduino digital pin for the shutter line

 @param c_foc
 The arduino digital pin for the focus line

 */

OMCamera::OMCamera(uint8_t c_shut, uint8_t c_foc) {
	m_shutter1 = c_shut;
	m_focus1   = c_foc;
	this->_init();
}

void OMCamera::_init() {
	pinMode(m_shutter1, OUTPUT);
	pinMode(m_shutter2, OUTPUT);
	pinMode(m_focus1, OUTPUT);
	pinMode(m_focus2, OUTPUT);

	m_shotsFired = 0;

	m_focusShut = true;

	m_interval = 1;
	enable = false;
	maxShots = 0;
	repeat = 0;

	m_timeExp  = 100;
	m_timeFoc  = 100;
	m_timeWait = 0;

}




/** Camera Busy

 Returns whether or not the camera is currently busy (in a non-blocking timer
 such as an expose, focus, or wait action).  Do not attempt to trigger a
 camera action when the camera is busy, otherwise you may cause some actions
 to not stop.

 @return
 true if busy, false if not

 */

bool OMCamera::busy() {
	return(m_isBzy);
}

/** Set Callback Handler

 As actions start and complete, a callback is used to signal what is happening,
 without having to query the method to see if a given action is still occuring.

 By passing a function pointer to this method, that function will be executed
 at each one of these moments, and passed a single, one byte argument that
 matches the signal which is to be sent.  The template for this function is:

 @code
 void function(byte val) { }
 @endcode

 The following codes are defined:

 @code
 Start Signals:

   OM_CAMEXP
      - The camera exposure has begun
   OM_CAMFOC
      - The camera focus has begun
   OM_CAMWAIT
      - The delay has begun

 End Signals:

   OM_CAM_EFIN
     - The camera exposure has completed
   OM_CAM_FFIN
     - The camera focus has completed
   OM_CAM_WFIN
     - The delay has completed

 @endcode

 An example of an appropriate callback handler:

 @code

 #include "OMCamera.h"

 OMCamera Camera = OMCamera();

 void setup() {

   Camera.setHandler(camSignal);
 }

 void loop() {

    // shoot one shot after another
  if( ! Camera.busy() )
  	Camera.expose(2000);


 }

 void camSignal(byte signal) {

   if( signal == OM_CAM_EFIN)
   	Serial.println("Exposure Done!");

 }

 @endcode

 @param p_Func
 A pointer to a function taking a single byte argument

 */

void OMCamera::setHandler( void(*p_Func)(uint8_t) ) {
	f_camSignal = p_Func;
}



/** Stop Current Action Immediately

 Stops an ongoing action immediately.  Sends an action complete signal when
 done.

 Any action, whether a focus, expose, or wait action, can be stopped using
 this method.  When the action is stopped, the current callback is executed
 and passed the correct complete signal for the action.  See setHandler() for
 more information on this callback.

 */

void OMCamera::stop() {

	// stop timer (always)
	// we do this before bringing the pin low in case
	// a VERY short action time was set to prevent the
	// timer from continuing to trigger and getting stuck in
	// a loop.

	if (m_debug)
		USBSerial.println("OMCamera::stop() - Entering function");

  MsTimer2::stop();

  uint8_t code = 0;

  // If expose state is set
  if( m_curAct == OM_CAM_INEXP ) {
	  
	  if (m_debug)
		USBSerial.println("OMCamera::stop() - Stopping exposure");
  	  
	  // expose complete
  	  digitalWrite(m_shutter1, LOW);
  	  digitalWrite(m_shutter2, LOW);

	    // we do this every time, because
	    // it's possible that the flag
	    // that controls whether or not to
	    // trip focus w. shutter may have
	    // been reset during our exposure,
	    // and failing to do so would keep
	    // the focus pin high for a long
	    // time.

	  digitalWrite(m_focus1, LOW);
	  digitalWrite(m_focus2, LOW);
	  code = OM_CAM_EFIN;
  }
  // If focus state is set
  else if( m_curAct == OM_CAM_INFOC ) {

	  if (m_debug)
		  USBSerial.println("OMCamera::stop() - Stopping focus");

	  // focus complete
	  digitalWrite(m_focus1, LOW);
	  digitalWrite(m_focus2, LOW);
	  code = OM_CAM_FFIN;
  }
  // If not ending exposure or focus state
  else {
	  if (m_debug)
		  USBSerial.println("OMCamera::stop() - Setting wait state");
	  code = OM_CAM_WFIN;
  }

  // reset current action
  m_curAct = OM_CAM_INCLR;

  // update camera currently engaged
  m_isBzy = false;


  if( code != 0 ) {
  	  // if we got here because of stop() called from a timer, then
  	  // let's send a proper signal (if callback set)
	  if (f_camSignal != 0) {
		  f_camSignal(code);
	  }
  }

}

/** Set Interval

This method sets the camera's interval setting

*/

void OMCamera::intervalTime(unsigned long p_interval) {
	this->m_interval = p_interval;
}

/** Get Interval

This method returns the camera's current interval setting

*/

unsigned long OMCamera::intervalTime() {
	return this->m_interval;
}

/** Trigger Exposure

This method triggers an exposure for the amount of time as set via triggerTime().
Sends an expose begin signal.

**/

void OMCamera::expose() {
	this->expose(m_timeExp);
}

/** Trigger Exposure

 This method triggers an exposure for the amount of time specified.
 Sends an expose begin signal.

 @param p_Time
 Length (in milliseconds) of the exposure.

*/

void OMCamera::expose(unsigned long p_time) {

	// do not expose if exposure time is zero
  if( p_time == 0 ) {
  	  if( f_camSignal != 0 )
  	  	  f_camSignal(OM_CAM_EFIN);

  	  return;
  }
  	// indicate (for stop()), that we are in an exposure
  	// to properly set output states when timer2 completes

    m_curAct = OM_CAM_INEXP;


    // determine if focus pin should be brought high
    // w. the shutter pin (for some nikons, etc.)

  if( m_focusShut == true ){
    digitalWrite(m_focus1, HIGH);
    digitalWrite(m_focus2, HIGH);
  }

  digitalWrite(m_shutter1, HIGH);
  digitalWrite(m_shutter2, HIGH);

    // start timer to stop camera exposure
  MsTimer2::set(p_time, OMCamera::stop);
  MsTimer2::start();


    // update camera currently engaged
  m_isBzy = true;


    // if there is a pointer to a function
    // to be called when the camera is done,
    // do it now

  if( f_camSignal != 0 )
  	  f_camSignal(OM_CAMEXP);

  return;
}


/** Trigger Focus

 This method triggers a focus for the amount of time as set via focusTime().
 Sends a focus begin signal.

*/

void OMCamera::focus() {
	this->focus(m_timeFoc);
}

/** Trigger Focus

 This method triggers a focus for the amount of time specified.
 Sends a focus begin signal.

 @param p_Time
 Length (in milliseconds) of the focus.

*/

void OMCamera::focus(unsigned int p_time) {

	// do not focus if focus time is 0
  if( p_time == 0 ) {
  	  if( f_camSignal != 0 )
  	  	  f_camSignal(OM_CAM_FFIN);

  	  return;
  }

  digitalWrite(m_focus1, HIGH);
  digitalWrite(m_focus2, HIGH);

    // start timer to stop focus engage

  m_curAct = OM_CAM_INFOC;

  // Set interrupt timer with focus time, call stop() when complete
  MsTimer2::set(p_time, OMCamera::stop);
  MsTimer2::start();
    // update camera currently engaged
  m_isBzy = true;
    // report focus in progress
  if( f_camSignal != 0 )
  	  f_camSignal(OM_CAMFOC);

}

/** Set Default Focus Time

 @param tm
 Focus time, in milliseconds
 */


void OMCamera::focusTime(unsigned int p_tm) {
	m_timeFoc = p_tm;
}

/** Set Default Exposure Time

 @param tm
 Exposure time, in milliseconds
 */

void OMCamera::triggerTime(unsigned long p_tm) {
	m_timeExp = p_tm;
}

/** Set Default Delay Time

AKA: Exposure delay

 @param tm
 Delay time, in milliseconds
 */

void OMCamera::delayTime(unsigned int p_tm) {
	m_timeWait = p_tm;
}

/** Get Default Delay Time

AKA: Exposure delay

 @return
 Delay time, in milliseconds
 */

unsigned int OMCamera::delayTime() {
	return(m_timeWait);
}

/** Get Default Focus Delay Time

 @return
 Focus time, in milliseconds
 */

unsigned int OMCamera::focusTime() {
	return(m_timeFoc);
}

/** Get Default Post-Exposure Delay Time

 @return
 Delay time, in milliseconds
 */

unsigned long OMCamera::triggerTime() {
	return(m_timeExp);
}

/** Enable Focus with Exposure

 Toggles whether or not the focus line should be triggered when the exposure
 line is triggered.  Thiss is required by some camera manufacturers.

 @param en
 Enable (true) or disable (false) this setting.
 */

void OMCamera::exposureFocus(bool en) {
	m_focusShut = en;
}

/** Get Focus with Exposure

 Gets whether or not the focus line is triggered when the exposure
 line is triggered.

 Enable (true) or disable (false) this setting.
 */

uint8_t OMCamera::exposureFocus() {
	return (m_focusShut);
}


/** Trigger Delay

 This method triggers a delay action for the amount of time as set via delayTime().
 Sends a delay begin signal.

*/


void OMCamera::wait() {
	this->wait(m_timeWait);
}

/** Trigger Delay

 This method triggers a delay action for the amount of time specified.
 Sends a delay begin signal.

 @param p_Time
 Length (in milliseconds) of the delay.

*/

void OMCamera::wait(unsigned int p_Time) {

	// do not wait, if wait time is 0
  if( p_Time == 0 ) {
	  if( f_camSignal != 0 )
		  f_camSignal(OM_CAM_WFIN);

	  return;
  }

  m_curAct = OM_CAM_INDLY;

  MsTimer2::set(p_Time, OMCamera::stop);
  MsTimer2::start();

    // update camera currently engaged
  m_isBzy = true;

  if( f_camSignal != 0 )
  	  f_camSignal(OM_CAMWAIT);
}

/** void debugOutput(bool p_switch)

This method handles turning on and off the USB debug output flag.

@param 
p_switch = (true, false)

*/

void OMCamera::debugOutput(bool p_state) {
	
	m_debug = p_state;
}

/** bool debugOutput()

This method handles retuning the state of the USB debug output flag.

*/

bool OMCamera::debugOutput() {

	return m_debug;
}



