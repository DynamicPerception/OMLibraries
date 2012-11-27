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


#include "OMEEPROM.h"

// using namespace OMEEPROM;




bool OMEEPROM::saved() {
    
    // read eeprom saved status
    
    uint8_t saved = EEPROM.read(0);
    
    // EEPROM memory is by default set to 1, so we
    // set it to zero if we've written data to eeprom
    return( ! saved );
}

void OMEEPROM::saved( bool saved ) {
    // set eeprom saved status
    
    // EEPROM memory is by default set to 1, so we
    // set it to zero if we've written data to eeprom
    
    EEPROM.write(0, !saved);
}


void OMEEPROM::write( int pos, byte& val, byte len ) {
    byte* p = (byte*)(void*)&val;
    
    if( m_forcePos)
        pos += s_EEPROMfirstUserPos;

    for( byte i = 0; i < len; i++ )
        EEPROM.write(pos++, *p++);    
    
    // indicate that memory has been saved
    saved(true);
    
}



// read functions

void OMEEPROM::read( int pos, byte& val, byte len ) {
    byte* p = (byte*)(void*)&val;
    
    if( m_forcePos)
        pos += s_EEPROMfirstUserPos;
    
    for(byte i = 0; i < len; i++) 
        *p++ = EEPROM.read(pos++);
}

/** Return the Stored Version number from EEPROM
 
 @return 
 An unsigned integer
 */

unsigned int OMEEPROM::version() {
    
    unsigned int eeprom_ver = 0;
    m_forcePos = false;
    read(1, eeprom_ver);
    m_forcePos = true;
    return eeprom_ver;
    
}

/** Store Version Number to EEPROM
 
 @param p_ver
 The version to store
 */

void OMEEPROM::version(unsigned int p_ver) {
    m_forcePos = false;
    write(1, p_ver);
    m_forcePos = true;
}
