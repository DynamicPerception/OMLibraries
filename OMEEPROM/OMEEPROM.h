
/* 
 
 EEPROM Storage Library
 
 OpenMoco MoCoBus Core Libraries 
 
 See www.dynamicperception.com for more information
 
 (c) 2010-2012 C.A. Church / Dynamic Perception LLC 
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


#ifndef	OM_EEPROM_H
#define OM_EEPROM_H

#include "Arduino.h"

#include <inttypes.h>
#include "EEPROM.h"

/** EEPROM Management Class
 
 This class allows easy access to common EEPROM activities, without polluting code
 with inline macros.  Additional features include common activities like indicating
 that data has been saved, and storing a version string for you.
 
 Writing any data to EPROM will automatically set the stored flag for you.
 
 An example:
 
 @code
 #include <EEPROM.h>
 #include "OMEEPROM.h"
 
 
 const int dataVersion = 100;
 
 byte  foo = 1;
 int   bar = 3500;
 float baz = 64.5;
 
 void setup() {
 
    if( OMEEPROM::saved() )
        loadvars();
    else
        savevars();
    
 }
 
 void loop() {
 
 }
 
 void loadvars() {
 
    using namespace OMEEPROM;
 
    read(1, foo); // first position, one byte
    read(2, bar); // 2nd position, two bytes
    read(4, baz); // fourth position (previous values used 1-3)
 }
 
 void savevars() {
 
    using namespace OMEEPROM;

    write(1, foo);
    write(2, bar);
    write(4, baz);
 
    version(dataVersion);
 
 }
 @endcode
 

 @author 
 C. A. Church
 
 */

namespace OMEEPROM {

    
     bool saved();
     void saved( bool saved );
    
     void write( int pos, uint8_t& val, byte len );
    
    template <typename T>
    void write(int pos, T& p_item) {
        byte* p = (byte*)(void*)&p_item;   
        write(pos, *p, sizeof(T));
    }

     void read( int pos, uint8_t& val, byte len );
    
     template <typename T>
     void read(int pos, T& p_item) {
        byte* p = (byte*)(void*)&p_item;
        read(pos, *p, sizeof(T));
     }
     
     unsigned int version();
     void version(unsigned int);
    
        // 0, 1, and 2 bytes are used for internal data (saved flag, version)
     const unsigned int s_EEPROMfirstUserPos = 3;
     static bool m_forcePos = true;
    
};


#endif //OM_EEPROM_H

