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

#ifndef	OM_COMHANDLER_H
#define OM_COMHANDLER_H


#include <inttypes.h>

 // pins for COM lines on nanoMoCo Board

#ifndef OMC_COM1
    #define OMC_COM1 2
#endif
#ifndef OMC_COM2
    #define OMC_COM2 3
#endif
#ifndef OMC_COM3
    #define OMC_COM3 19
#endif

    // defines for pin change interrupt handling
#ifndef OMC_PCVECT
    #define OMC_PCVECT PCINT1_vect
#endif
#ifndef OMC_PCFLAG
    #define OMC_PCFLAG  PCICR
#endif
#ifndef OMC_PCENABLE
    #define OMC_PCENABLE  PCIE1
#endif
#ifndef OMC_PCMASK
    #define OMC_PCMASK    PCMSK1
#endif
#ifndef OMC_PCINT
    #define OMC_PCINT     PCINT13
#endif
#ifndef OMC_PCREG
    #define OMC_PCREG    PINC
#endif
#ifndef OMC_PCPIN
    #define OMC_PCPIN     PINC5
#endif

    // defines for port registers for com1 and com2
#ifndef OMC_COM1_REG
    #define OMC_COM1_REG PIND
#endif
#ifndef OMC_COM1_PIN
    #define OMC_COM1_PIN PIND2   
#endif
#ifndef OMC_COM2_REG
    #define OMC_COM2_REG PIND
#endif
#ifndef OMC_COM2_PIN
    #define OMC_COM2_PIN PIND3   
#endif


/**

  @brief
  Common Line Handler
  
  The Common Line Handler class allows for handling of the three common
  lines designed for triggering different behaviors between nodes, and 
  for synchronizing timing between a 'master'-timing node and 'slave' nodes
  in a network of Motion Axis devices.
  
  This class handles the required functionalitty for both the slave and the
  master node.
  
  There must only be one instance of this class in existance at a time, you
  cannot use two instances of this class.
  
  
  Please note that OMComHandler uses Arduino pins 2, 3, and 19 (analog 5) by default.  See
  Pin Assignments below for information about (possibly) changing this.
  
  There are three common lines available to the MoCoBus: Common 1 (2), Common 2 (3), and
  Common 3 (19).  Common 3 is used for the timing signal between nodes, and is implemented
  in this library using a pin change interrupt.  If Common 1 and 2 are monitored, a hardware
  interrupt is used for these pins.
 
  Master Node Example:
  
   @code
  
  // Master Node
  
  
  OMComHandler ComMgr;
  
  void setup() {
    ComMgr = OMComHandler();
    	// configure line handler as master
    ComMgr.master(true);
  }
  
  void loop() {
  
    // do something
    
    ..
    
    // trigger a complete signal
    ComMgr.masterSignal();
    
  }
  @endcode
  
  Slave Node Example:
  
  @code
  
  // Slave Node
  
  
  OMComHandler ComMgr;
  
  void setup() {
    ComMgr = OMComHandler();
    	// configure line handler as slave
    ComMgr.master(false);
  }
  
  void loop() {

    // received a complete signal
    if( ComMgr.slaveClear() == true ) {
  
      // do something
    
      ..
    }
    
    
  }
  @endcode
  
 @section omcmonitor Monitoring Common 1 and Common 2
 
 It is possible to monitor either Common 1 or Common 2 for a signal.  In this
 case, a callback handler is required, and this callback handler is called when a signal is
 brought LOW and then back to HIGH, and is passed an unsigned integer with the number of microseconds
 the pin was brought low.
 
 Note that Common 1 and Common 2 are expected to be hardware interrupts, and the core Arduino attachInterrupt()
 function is used for Com 1 and Com 2.
 
 For example:
 
 @code
 
 OMComHandler ComMgr;
 
 volatile unsigned int trip_time = 0;
 
 void setup() {
    ComMgr = OMComHandler();
        // watch COM1
    ComMgr.watch(1);
    ComMgr.watchHandler(tripped);
 }
 
 void loop() {
 
 
    if( trip_time > 0 ) {
        
        if( trip_time > 100 ) {
            digitalWrite(13, HIGH);
            delay(2000);
            digitalWrite(13, LOW);
        }
        else {
            digitalWrite(13, HIGH);
            delay(1000);
            digitalWrite(13, LOW);
        }
    
        trip_time = 0;
    }
  
 
 }
 
 void tripped(unsigned int p_us) {
    trip_time = p_us;
 }
 @endcode
 
 @section omcpinassign Pin Assignments
 
 It is, generally speaking, possible to re-define the pins used for common line handling.  The
 most likely candidate is COM3, the pin used for timing synchronization and for which a pin
 change register is defined.  
 
 Althernatively, the COM 1 and COM 2 pins may be changed, by changing all associated defines, however
 you must ensure that the built-in arduino attachInterrupt() function can handle these with the interrupt
 numbers 0 and 1.
 
 To re-define pins in-use by this library, define any of the relevant values from the list below <b>before</b>
 #include'ing this library.  Default values are listed:
 
  Arduino Digital Pin # Defines
 
 OMC_COM1   2
 OMC_COM2   3
 OMC_COM3   19
 
  Defines for pin change interrupt handling
 
 OMC_PCVECT     PCINT1_vect
 OMC_PCFLAG     PCICR
 OMC_PCENABLE   PCIE1
 OMC_PCMASK     PCMSK1
 OMC_PCINT      PCINT13
 OMC_PCREG      PINC
 OMC_PCPIN      PINC5
 
  Defines for port registers for com1 and com2
 
 OMC_COM1_REG   PIND
 OMC_COM1_PIN   PIND2   
 OMC_COM2_REG   PIND
 OMC_COM2_PIN   PIND3   

 
  @author C. A. Church

  (c) 2011-2012 C. A. Church / Dynamic Perception
  
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
  
class OMComHandler
{
	
private:
	
	bool m_isMaster;

    static bool m_which;
    volatile static unsigned long m_isrUs;
    
	void _masterFollow();
	void _masterLead();
    
    static void _isrFire();
    
    static void(*f_isrCB)(uint16_t);
	
public:
	
	OMComHandler(); // constructor
	
	bool master();
	void master(bool p_mast);
	void masterSignal();
	static bool slaveClear();
    
    static void watch(uint8_t p_which);
    static void watchHandler(void(*p_func)(uint16_t));
    static void stopWatch();
	
	
};


#endif
