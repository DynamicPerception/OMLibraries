
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

#ifndef	OM_CAMERA_H
#define OM_CAMERA_H

#include <inttypes.h>





// default pin assignments (nanomoco)
#define OM_DEFSHUTTER1	 9		// Trigger 1 pin
#define OM_DEFSHUTTER2	 8		// Trigger 2 pin
#define OM_DEFFOCUS1     11		// Focus 1 pin
#define OM_DEFFOCUS2     10		// Focus 2 pin

// callback status codes

#define OM_CAMEXP		1	// Camera exposure has begun
#define OM_CAM_EFIN		2	// Camera exposure has completed
#define OM_CAMFOC		3	// Camera focus has begun
#define OM_CAM_FFIN		4	// Camera focus has completed
#define OM_CAMWAIT		5	// (Exposure) delay has begun
#define OM_CAM_WFIN		6	// (Exposure) delay has completed


#define OM_CAM_INCLR	0	// Camera action is cleared 
#define OM_CAM_INEXP	1	// Camera is currently exposing 
#define OM_CAM_INFOC	2	// Camera is currently focusing
#define OM_CAM_INDLY	3	// Camera is currently idle

/**

  @brief
  Camera Action Manager

  The Caamera Action Manager class controls a camera's focus and shutter
  lines, given that these lines are attached to an arduino device via
  an optocoupler or similar device.  The focus and shutter lines are
  driven HIGH to trigger the given action.

  All actions performed by the Camera Action Manager are non-blocking, and
  this library uses the MsTimer2 library to achieve this.  For this reason,
  you may only have one such object instantiated in your program, and under
  no circumstances should you attempt to use MsTimer2 in another part of
  your program while a camera action is being performed.

  Status reporting from the Camera Manager is handled via a callback function,
  which you may specify.  This function will be called at the beginning and
  completion of any action with a special code to indicate the status.  For
  more information on this, see setHandler().

  The default pins for the shutter and focus are 13 and 12, respectively.
  You may, however specify the shutter and focus pins via the constructor.

  Example:

  @code
   // note: You -always- have to include the MSTimer2 library in an arduino
   // sketch before including the OMCamera header

#include "MsTimer2.h"
#include "OMCamera.h"

OMCamera Camera = OMCamera();

void setup() {

  Camera.setHandler(camCallBack);

  Serial.begin(19200);

}

void loop() {

  if( ! Camera.busy()  ) {
      // camera is not busy doing anything, so we're ready to
      // start another cycle
    Serial.println("Round Begin, Triggering Focus");
    Camera.focus(1000);
  }

}

void camCallBack(byte code) {

  switch(code) {

    case OM_CAM_FFIN:  // focus finish code received

      Serial.println("Focus Done, Triggering Exposure");
        // trigger an exposure
      Camera.expose(1000);
      break;

    case OM_CAM_EFIN: // exposure finish code received

      Serial.println("Exposure Done, Waiting.");

        // wait (non-blocking wait implemented via timer2) for 10 seconds
      Camera.wait(10000);
      break;

    case OM_CAM_WFIN: // wait finish code received

        // we do nothing, because this is the end of a sshot cycle -
        // the main loop will pick up again witth focus, because the
        // camera will no longer register as busy.

      Serial.println("Wait Done, Round Complete");
      Serial.println("");
      break;
  }
}


  @endcode

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

class OMCamera {


private:

	unsigned long m_shotsFired;

	static uint8_t m_shutter1;
    static uint8_t m_shutter2;
	static uint8_t m_focus1;
	static uint8_t m_focus2;

	unsigned long m_interval;						// Camera interval in milliseconds
	unsigned long m_timeExp;
	unsigned int m_timeFoc;
	unsigned int m_timeWait;

	bool m_focusShut;

	static bool m_debug;

	static uint8_t m_curAct;

	static bool m_isBzy;
	static void(*f_camSignal)(uint8_t);

	void _init();

public:

	OMCamera();
	OMCamera(uint8_t c_shut, uint8_t c_foc);

	static bool busy();

	void setHandler(void(*)(uint8_t));

	unsigned long maxShots;
	
	bool repeat;
	bool enable;

	void interval(unsigned long p_interval);
	unsigned long interval();
	void expose();
	void expose(unsigned long p_time);
	void triggerTime(unsigned long p_tm);
	unsigned long triggerTime();

	void exposureFocus(bool);
	uint8_t exposureFocus();
		
	void focus();
	void focus(unsigned int p_time);
	void focusTime(unsigned int p_tm);
	unsigned int focusTime();

	void wait();
	void wait(unsigned int p_time);
	void delayTime(unsigned int p_tm);
	unsigned int delayTime();

	static void stop();
	static void debugOutput(bool p_state);
	static bool debugOutput();
};










#endif

