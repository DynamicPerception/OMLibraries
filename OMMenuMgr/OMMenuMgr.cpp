/*
 
 Generalized Menu Library
 
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

#include "OMMenuMgr.h"


/** Constructor
 
  Constructs an OMMenuMgr instance, with a specified root item, and a
 specified input type.
 
  @code
 #include "OMMenuMgr.h"
 
 ...
 
 OMMenuMgr Menu = OMMenuMgr(&rootItem, MENU_ANALOG);
 @endcode
 
 @param c_first
 A pointer to an OMMenuItem representing the root of the menu
 
 @param c_type
 The input type for the menu, either MENU_ANALOG or MENU_DIGITAL
 
 */

OMMenuMgr::OMMenuMgr(OMMenuItem* c_first, uint8_t c_type) {
 
    m_inputType  = c_type;
    m_curParent  = c_first;
    m_rootItem   = c_first;
    m_inEdit     = false;
    m_enable     = true;
    m_menuActive = false;
    m_curSel     = 0;
    m_draw       = 0;
    m_exit       = 0;
    m_holdMod    = 1;
    
    memset(m_hist, 0, sizeof(OMMenuItem*) * sizeof(m_hist));
        
}

/** Setup Analog Button Input
 
 For UI's with multiple buttons assigned to a single analog pin 
 (e.g. using a resistor network to report different values), this
 method allows you to specify how this pin shall be read.
 
 This method is NOOP if the menu type is not MENU_ANALOG
 
 The digital pin # is specified, along with an array of values and their meanings,
 and a threshold for those values.  The array of values and meanings should be two-dimensional
 with two values in the second dimension, and five rows in the first dimension.  The two values
 shall represent the integer value of analogRead() assigned to that button, and the button type. e.g.:
 
 @code
 const int[5][2] myButtons = { {150, BUTTON_FORWARD}, 
                         {250, BUTTON_INCREASE}, 
                         {350, BUTTON_DECREASE}, 
                         {450, BUTTON_BACK}, 
                         {550, BUTTON_SELECT} 
                        };
 
 Menu.setAnalogButtonPin(14, myButtons, 25);
 @endcode
 
 @param p_pin
 The Arduino Digital Pin to use
 
 @param p_values
 The list of button values and function
 
 @param p_thresh
 The threshold deviation from the specified value to match the button read. You must provide some
 threshold, as the analogRead() values will vary.  The range of value to be matched for each button
 is between (value - thresh) and (value + thresh). This threshold should be large enough to account
 for the variation in your circuit, but by no means should two button values overlap.
 
 
 */
void OMMenuMgr::setAnalogButtonPin(uint8_t p_pin, const int p_values[5][2], int p_thresh) {
    
    if( m_inputType != MENU_ANALOG )
        return;
    
    m_anaPin    = p_pin;
    m_anaThresh = p_thresh;
    
    for(uint8_t i = 0; i < 5; i++) {
        m_butVals[i][0] = p_values[i][0];
        m_butVals[i][1] = p_values[i][1];
    }
    
    pinMode(p_pin, INPUT);
    digitalWrite(p_pin, HIGH);
    
}

/** Setup Digital Button Input
 
 For UI's with digital button input (e.g. one digital input per button), this
 method allows you to specify the pins for digital input.
 
 An array is passed that defines, for each button, it's digital pin and function.
 There must be two dimensions, with five rows and two data elements.  The first data
 element indicates the Arduino digital pin, and the second element indicates the function.
 
 For example:
 
 @code
 const int[5][2] myButtons = { {4, BUTTON_FORWARD}, 
                         {5, BUTTON_INCREASE}, 
                         {6, BUTTON_DECREASE}, 
                         {7, BUTTON_BACK}, 
                         {8, BUTTON_SELECT} 
                        };
 
 Menu.setDigitalButtonPins(myButtons);
 @endcode
 
 @param p_values
 The list of button values and functions
  
 */
void OMMenuMgr::setDigitalButtonPins(const int p_values[5][2]) {
    
    if( m_inputType != MENU_DIGITAL )
        return;

        // set all pins as inputs, enable internal pullups
    
    for(uint8_t i = 0; i < 5; i++ ) {
        m_butVals[i][0] = p_values[i][0];
        m_butVals[i][1] = p_values[i][1];
        
        pinMode(m_butVals[i][0], INPUT);
        digitalWrite(m_butVals[i][0], HIGH);
        
    }
    
}

/** Set Draw Handler Callback
 
 Sets the draw handler callback, see \ref menudisplay "Managing Display" for more
 information
 
 @param p_func
 A function pointer for the draw handler
 */

void OMMenuMgr::setDrawHandler(void(*p_func)(char*, int, int, int)) {
    m_draw = p_func;
    
}

/** Set Exit Handler Callback
 
 Sets the exit handler callback, see \ref menudisplay "Managing Display" for more
 information
 
 @param p_func
 A function pointer for the exit handler
 */

void OMMenuMgr::setExitHandler(void(*p_func)()) {
    m_exit = p_func;
}


 /** Menu Shown State
  
   Whether or not the menu is currently being displayed.
  
  @return
  Whether or the menu is currently displayed (true), or not (false)
  */

bool OMMenuMgr::shown() {
    return m_menuActive;
}

/** Check for User Input and Handle
 
 Checks to see if any button has been pressed by the user, and reports back
 the button pressed, or BUTTON_NONE if none are pressed. 
 
 If the menu is enabled and drawn, and any button is pressed, normal handling of the
 menu is executed. If the menu is enabled, but not drawn, the menu will be drawn
 only if BUTTON_SELECT is pressed.  Otherwise, no activity will occur.
 
 @return
 The button pressed, one of: BUTTON_NONE, BUTTON_FORWARD, BUTTON_BACK, BUTTON_INCREASE, BUTTON_DECREASE, BUTTON_SELECT
 */

uint8_t OMMenuMgr::checkInput() {
    
    static unsigned long lastTm  = millis();
    static uint8_t       lastKey = BUTTON_NONE;
    static uint8_t       holdKey = BUTTON_NONE;
    static unsigned int  held    = 0;
    
        // returning to list display after being
        // disabled during an action
    if( m_forceReturn == true && m_enable ) {
        m_forceReturn = false;
        _displayList(m_curParent, m_curTarget);
    }
    
        // get which button is pressed
    int key = BUTTON_NONE;
    
    if( m_inputType == MENU_ANALOG )
        key = _checkAnalog();
    else
        key = _checkDigital();
    
 
    if( key != lastKey ) {
            // did not have two reads in the row with same key,
            // thus de-bouncing is not possible
        lastKey   = key;
        holdKey   = BUTTON_NONE;
        lastTm    = millis();
        held      = 0;
        m_holdMod = 1;
        return BUTTON_NONE;
    }
    
        // enough time for debounce?
    
    if( millis() - lastTm > OM_MENU_DEBOUNCE ) 
        lastTm = millis();
    else
        return BUTTON_NONE;
        
        // no button is pressed, or the menu display
        // is disabled
    if( key == BUTTON_NONE || ! m_enable ) {
        held      = 0;
        m_holdMod = 1;
        holdKey   = BUTTON_NONE;
        return key;
    }
    
        // if the menu hasn't been drawn, ignore 
        // interaction features

    if( key == BUTTON_SELECT )
        m_menuActive = true;
    
    if( ! m_menuActive )
        return key;
    
        // when a button is held, like up or down,
        // we increase the modifier over time to allow
        // the rate at which numbers go up/down to increase
    
    if( key == holdKey ) {
        
        held++;
        if( held % 10 )
            m_holdMod += 2; 
    }
    else {
        holdKey   = key;
        m_holdMod = 1;
    }
    
    
    _handleButton(key);
    
    return key;
    
    
}

/** Enable or Disable Handling of Menu
 
 You may disable handling of menu to allow the class instance to just poll and
 report back the currently pressed button.  This is particularly useful when 
 executing actions from menu items, where the user input is needed.
 
 @param p_en
 Enable (true), or disable (false) menu display
 
 */

void OMMenuMgr::enable(bool p_en) {
    m_enable = p_en;
}

/** Get Enable Status
 
 Returns the current enable status
 
 @return
 Enable status
 */

bool OMMenuMgr::enable() {
    return m_enable;
}

/** Set New Root Item
 
 Sets new root item as the base for future calls to checkInput();
 
 @param p_root
 A pointer to an OMMenuItem
 */

void OMMenuMgr::setRoot(OMMenuItem* p_root) {
    m_rootItem = p_root;
    m_curParent = p_root;
}



int OMMenuMgr::_checkAnalog() {
    
    int curVal = analogRead(m_anaPin);
    
    static int lastVal = 0;
    
        // isolate jitter in a single cycle
    if( abs(lastVal - curVal) > m_anaThresh ) {
        lastVal = curVal;
        return(BUTTON_NONE);
    }
    

        // check to see which analog button was read
    for(byte i = 0; i < 5; i++ ) {
        if( curVal > (m_butVals[i][0] - m_anaThresh) && curVal < (m_butVals[i][0] + m_anaThresh) )
            return m_butVals[i][1];
    }
    
    return BUTTON_NONE;
}

int OMMenuMgr::_checkDigital() {
   
    // check to see which analog button was read
    for(byte i = 0; i < 5; i++ ) {
        if( digitalRead(m_butVals[i][0]) == LOW )
            return m_butVals[i][1];
    }
    
    return BUTTON_NONE;
}



void OMMenuMgr::_display(char* p_str, int p_row, int p_col, int p_count) {
    if( m_draw != 0 )
        m_draw(p_str, p_row, p_col, p_count);
}


// What happens when a button is pressed? Handle this activity

void OMMenuMgr::_handleButton(uint8_t p_key) {
    

    if( p_key == BUTTON_SELECT || p_key == BUTTON_FORWARD ) {
                
        if( m_inEdit ) {
            _edit(m_curSel, CHANGE_SAVE);
        }
        else { 
            if( m_curSel != 0 )
                _activate(m_curSel);
             else 
                _activate(m_rootItem);
                
        } 
        
        return;
    }
    
        // this looks vicious compared to the code below, but it saves nearly 50 bytes of flash space by
        // converting three if statements to two using ITE operators
    
    p_key = (p_key == BUTTON_INCREASE ) ? CHANGE_UP : (p_key == BUTTON_DECREASE) ? CHANGE_DOWN : CHANGE_ABORT;
    
    if( m_inEdit )
        _edit(m_curSel, p_key);
    else
        _menuNav(p_key);
    
    
 /*   else if( p_key == BUTTON_INCREASE ) {
        
        if( m_inEdit )
            _edit(m_curSel, CHANGE_UP);
        else
            _menuNav(CHANGE_UP);
    }
    
    else if( p_key == BUTTON_DECREASE ) {
                
        if( m_inEdit )
            _edit(m_curSel, CHANGE_DOWN);
        else
            _menuNav(CHANGE_DOWN);        
    }
    
    else if( p_key == BUTTON_BACK ) {
            
        if( m_inEdit ) 
            _edit(m_curSel, CHANGE_ABORT);
        
        else 
            _menuNav(CHANGE_ABORT);  
        
    }*/
            
}


 // exiting the menu entirely

void OMMenuMgr::_exitMenu() {
    m_curParent  = m_rootItem;
    m_curTarget  = 0;
    m_menuActive = false;
    
    if( m_exit != 0 )
        m_exit();
    
}

 // navigating through menus

void OMMenuMgr::_menuNav(uint8_t p_mode) {
    
    uint8_t childCount = pgm_read_byte(&(m_curParent->targetCount));
    childCount--;
    
    if( p_mode == CHANGE_ABORT ) {
        
            // exiting this menu level
        
        m_curSel = 0;
        
            // get previous menu level
        OMMenuItem* newItem = _popHist();
        
        if( newItem == 0 ) {
            // aborting at root
            _exitMenu();
        }
        else {
            m_curParent = newItem;
            m_curTarget = 0;
            _activate(m_curParent, true);
        }
        
    
    }
    
    else if( p_mode == CHANGE_UP ) {
        
            // don't attempt to move above total list
        if( m_curTarget == 0 )
            return;
        
        m_curTarget--;
        _displayList(m_curParent, m_curTarget);
        
    }
    
    else if( p_mode == CHANGE_DOWN ) {
        
        if( m_curTarget >= childCount )
            return;
        
        m_curTarget++;
        _displayList(m_curParent, m_curTarget);
        
    }
        
    
}

// activating a menu item
void OMMenuMgr::_activate(OMMenuItem* p_item, bool p_return) {
    
    // get item type
    uint8_t type = pgm_read_byte(&(p_item->type));
    
    // process activation based on type
    if( type == ITEM_VALUE ) {
        m_inEdit = true;
        _displayEdit(p_item);
    }
    else if( type == ITEM_MENU ) { 
        // sub-menu
        
        if( ! p_return && p_item != m_curParent ) {
            _pushHist(m_curParent);
            m_curParent = p_item;
            m_curTarget = 0;
        }
        
        _displayList(p_item, m_curTarget);
    }
    else if( type == ITEM_ACTION ) {
        // this is gnarly, dig? We're pulling a function pointer
        // out of progmem
        f_valueHandler callback = reinterpret_cast<f_valueHandler>( reinterpret_cast<void*>(pgm_read_word(&(p_item->target)) ));
        
        if( callback != 0 )
            callback();
        
        if( m_enable )
            _displayList(m_curParent, m_curTarget);
        else 
            m_forceReturn = true;
    } 
}




// Display list rows on the screen

void OMMenuMgr::_displayList(OMMenuItem* p_item, uint8_t p_target) {
    

    uint8_t childCount     = pgm_read_byte(&(p_item->targetCount));
    childCount--;
    
    OMMenuItem** items = reinterpret_cast<OMMenuItem**>(reinterpret_cast<void*>(pgm_read_word(&(p_item->target))));
    m_curSel = reinterpret_cast<OMMenuItem*>(pgm_read_word(&(items[p_target])));
    
    
        // loop through display rows
    for(byte i = 0; i < OM_MENU_ROWS; i++ ) {
            // flush buffer
        memset(m_dispBuf, ' ', sizeof(char) * sizeof(m_dispBuf));
    
        int startX = 0;
        
            // display cursor on first row
        if( i == 0 ) {
            _display((char*) OM_MENU_CURSOR, i, 0, sizeof(char) * sizeof(OM_MENU_CURSOR) - 1);
            startX += ( sizeof(char) * sizeof(OM_MENU_CURSOR) - 1 );
        }
        else {
            _display((char*) OM_MENU_NOCURSOR, i, 0, sizeof(char) * sizeof(OM_MENU_NOCURSOR) - 1);
            startX += ( sizeof(char) * sizeof(OM_MENU_NOCURSOR) - 1 );
        }
        
            // if there's actually an item here, copy it to the display buffer
        if( childCount >= p_target + i ) {
            // OMMenuItem* item = reinterpret_cast<OMMenuItem*>( pgm_read_word(p_item->target.items[p_target + i]) );
           OMMenuItem* item = reinterpret_cast<OMMenuItem*>(pgm_read_word(&(items[p_target + i])));
           memcpy_P(m_dispBuf, &( item->label ), sizeof(char) * sizeof(m_dispBuf) );
        }
               
        _display(m_dispBuf, i, startX, OM_MENU_COLS);
    } 
                
}

 // display a value to be edited

void OMMenuMgr::_displayEdit(OMMenuItem* p_item) {
    
         // display label
    
         // copy data to buffer from progmem
    memcpy_P(m_dispBuf, &( p_item->label ), sizeof(char) * sizeof(m_dispBuf) );
    
    _display(m_dispBuf, 0, 0, OM_MENU_COLS);
    
    OMMenuValue* value = reinterpret_cast<OMMenuValue*>(reinterpret_cast<void*>(pgm_read_word(&(p_item->target))));
    
    uint8_t type  = pgm_read_byte(&(value->type));
    void* valPtr  = reinterpret_cast<void*>( pgm_read_word(&(value->targetValue)) );

        // get current value and squirrel it away for now
        // we use temporary variables to avoid all sorts of crazyness should
        // we want to port to another platform later, and endianness would
        // be a problem if we decided to use one large variable (ulong) and
        // then use specific bytes.  This takes more memory, but it seems
        // a viable trade
    
    if( type == TYPE_BYTE )
        m_temp     = *reinterpret_cast<uint8_t*>(valPtr);
    else if( type == TYPE_UINT || type == TYPE_INT )
        m_tempI   = *reinterpret_cast<int*>(valPtr);
    else if( type == TYPE_LONG || type == TYPE_ULONG )
        m_tempL   = *reinterpret_cast<long*>(valPtr);
    else if( type == TYPE_FLOAT || type == TYPE_FLOAT_10 || type == TYPE_FLOAT_100 || type == TYPE_FLOAT_1000 )
        m_tempF    = *reinterpret_cast<float*>(valPtr);
    else if( type == TYPE_SELECT ) {
            // select types are interesting...  We have a list of possible values and
            // labels - we need to work that back to an index in the list...
        OMMenuSelectValue*      sel = reinterpret_cast<OMMenuSelectValue*>(valPtr);
        OMMenuSelectListItem** list = reinterpret_cast<OMMenuSelectListItem**>(reinterpret_cast<void*>(pgm_read_word(&(sel->list))));
        uint8_t              curVal = *reinterpret_cast<uint8_t*>(pgm_read_word(&(sel->targetValue)));
        uint8_t              count  = pgm_read_byte(&(sel->listCount));
        
            // mark to first index by default
        m_temp = 0;
        
            // find index of current assigned value (if can be found)
        for(uint8_t i = 0; i < count; i++ ) {
            OMMenuSelectListItem* item = reinterpret_cast<OMMenuSelectListItem*>(pgm_read_word(&(list[i])));
            uint8_t tgt = pgm_read_byte(&(item->value));
            if( tgt == curVal )
                m_temp = i;
        }
        
        _displaySelVal(list, m_temp);
        return;
    }
    else if( type == TYPE_BFLAG ) {
        OMMenuValueFlag* flag = reinterpret_cast<OMMenuValueFlag*>(valPtr);
        uint8_t*       target = reinterpret_cast<uint8_t*>(pgm_read_word(&(flag->flag)));
        uint8_t           pos = pgm_read_byte(&(flag->pos));
        
        if( *target & (1 << pos) )
            m_temp = 1;
        else
            m_temp = 0;
        
        _displayFlagVal();
        return;
    }
    
    // throw number on-screen
    _displayVoidNum(valPtr, type, 1, 0);

                
}


 // rationalize a way to display any sort of number as a char*, rationalize it buddy, rationalize it good...

void OMMenuMgr::_displayVoidNum(void* p_ptr, uint8_t p_type, int p_row, int p_col) {
        
        // clear out display buffer
    memset(m_dispBuf, ' ', sizeof(char) * sizeof(m_dispBuf));
    
        // handle variable precision for float nums
    byte floatPrecision = 1;
    
    if( p_type == TYPE_FLOAT_100 ) 
        floatPrecision = 2;
    else if( p_type == TYPE_FLOAT_1000 )
        floatPrecision = 3;
    
    
    switch (p_type) {
        case TYPE_BYTE:
            utoa(*reinterpret_cast<uint8_t*>(p_ptr), m_dispBuf, 10);
            break;
        case TYPE_INT:
            itoa(*reinterpret_cast<int*>(p_ptr), m_dispBuf, 10);
            break;
        case TYPE_UINT:
            utoa(*reinterpret_cast<unsigned int*>(p_ptr), m_dispBuf, 10);
            break;
        case TYPE_LONG:
            ltoa(*reinterpret_cast<long*>(p_ptr), m_dispBuf, 10);
            break;
        case TYPE_ULONG:
            ultoa(*reinterpret_cast<unsigned long*>(p_ptr), m_dispBuf, 10);
            break;
        case TYPE_FLOAT:
        case TYPE_FLOAT_10:
        case TYPE_FLOAT_100:
        case TYPE_FLOAT_1000:
            dtostrf(*reinterpret_cast<float*>(p_ptr), floatPrecision + 2, floatPrecision, m_dispBuf);
            break;
    }
    
    _display(m_dispBuf, p_row, p_col, sizeof(char) * sizeof(m_dispBuf));

}

 // handle modifying temp values based on their type

void OMMenuMgr::_modifyTemp(uint8_t p_type, uint8_t p_mode, long p_min, long p_max) {

    void* tempNum;
    
    int mod    = ( p_mode == MODE_INCREMENT ) ? 1 : -1;    

        // apply holding rate mod
    mod *= m_holdMod;
    
        // handle float precision adjustment
    float fmod = (float) mod;
    
    if( p_type == TYPE_FLOAT_10 ) 
        fmod /= 10.0;
    else if( p_type == TYPE_FLOAT_100 )
        fmod /= 100.0;
    else if( p_type == TYPE_FLOAT_1000 )
        fmod /= 1000.0;
        
            
        // manage correct temporary variable change based on type
        // and apply floor/ceiling from min/max

    switch (p_type) {
        case TYPE_BYTE:
            m_temp += mod;
            if( p_min != 0 && p_max != 0 )
                m_temp = m_temp > p_max ? p_max : ( m_temp < p_min ? p_min : m_temp );
            tempNum = reinterpret_cast<void*>(&m_temp);
            break;
        case TYPE_INT:
            m_tempI += mod;
            if( p_min != 0 && p_max != 0 )
                m_tempI = m_tempI > p_max ? p_max : ( m_tempI < p_min ? p_min : m_tempI );
            tempNum = reinterpret_cast<void*>(&m_tempI);
            break;
        case TYPE_UINT:
            *reinterpret_cast<unsigned int*>(&m_tempI) += mod;
            if( p_min != 0 && p_max != 0 )
                m_tempI = m_tempI > p_max ? p_max : ( m_tempI < p_min ? p_min : m_tempI );
            tempNum = reinterpret_cast<void*>(&m_tempI);
            break;
        case TYPE_LONG:
            m_tempL += mod;
            if( p_min != 0 && p_max != 0 )
                m_tempL = m_tempL > p_max ? p_max : ( m_tempL < p_min ? p_min : m_tempL );
            tempNum = reinterpret_cast<void*>(&m_tempL);
            break;
        case TYPE_ULONG:
            *reinterpret_cast<unsigned long*>(&m_tempL) += mod;
            if( p_min != 0 && p_max != 0 )
                m_tempL = m_tempL > p_max ? p_max : ( m_tempL < p_min ? p_min : m_tempL ); 
            tempNum = reinterpret_cast<void*>(&m_tempL);
            break;
        case TYPE_FLOAT:
        case TYPE_FLOAT_10:
        case TYPE_FLOAT_100:
        case TYPE_FLOAT_1000:
            m_tempF += fmod;
            if( p_min != 0 && p_max != 0 )
                m_tempF = m_tempF > p_max ? p_max : ( m_tempF < p_min ? p_min : m_tempF );
            tempNum = reinterpret_cast<void*>(&m_tempF);
            break;
        default:
            tempNum = reinterpret_cast<void*>(&m_temp);
            break;
            
    }

        // display new temporary value
    _displayVoidNum(tempNum, p_type, 1, 0);

    
}

    // display the label value from a select list on the screen

void OMMenuMgr::_displaySelVal(OMMenuSelectListItem** p_list, uint8_t p_idx) {
    
        // clear out display buffer
    memset(m_dispBuf, ' ', sizeof(char) * sizeof(m_dispBuf));

        // get the actual select list item
    OMMenuSelectListItem* item = reinterpret_cast<OMMenuSelectListItem*>(pgm_read_word(&(p_list[p_idx])));
    
        // copy the label contents from flash to the buffer
    memcpy_P(m_dispBuf, &( item->label ), sizeof(char) * OM_MENU_LBLLEN );
    
        // display the buffer
    _display(m_dispBuf, 1, 0, sizeof(char) * sizeof(m_dispBuf));
    
}


 // modifying a select type value - we cycle through a list of
 // values by cycling list index...

void OMMenuMgr::_modifySel(OMMenuValue* p_value, uint8_t p_mode) {
    
    OMMenuSelectValue*      sel = reinterpret_cast<OMMenuSelectValue*>(pgm_read_word(&(p_value->targetValue)));
    uint8_t               count = pgm_read_byte(&(sel->listCount));
    OMMenuSelectListItem** list = reinterpret_cast<OMMenuSelectListItem**>(reinterpret_cast<void*>(pgm_read_word(&(sel->list))));
    
    count--;
    
    if( p_mode == MODE_DECREMENT ) {
        if(m_temp == 0)
            m_temp = count;
        else
            m_temp--;
    }
    else if( p_mode == MODE_INCREMENT ) {
        if(m_temp == count)
            m_temp = 0;
        else
            m_temp++;
    }
    
    _displaySelVal(list, m_temp);
    
}

// displaying flag parameters

void OMMenuMgr::_displayFlagVal() {
    
    // overwrite buffer
    memset(m_dispBuf, ' ', sizeof(char) * sizeof(m_dispBuf));
    
    if( m_temp == 1 )
        memcpy(m_dispBuf, (char*) OM_MENU_FLAG_ON, sizeof(char) * sizeof(OM_MENU_FLAG_ON) - 1);
    else
        memcpy(m_dispBuf, (char*) OM_MENU_FLAG_OFF, sizeof(char) * sizeof(OM_MENU_FLAG_OFF) - 1);
    
    _display(m_dispBuf, 1, 0, sizeof(char) * sizeof(m_dispBuf));
    
}


 // edit operations against a displayed value

void OMMenuMgr::_edit(OMMenuItem* p_item, uint8_t p_type) {
    
    OMMenuValue* thisValue = reinterpret_cast<OMMenuValue*>(pgm_read_word(&(p_item->target)));
    
    uint8_t type = pgm_read_byte(&(thisValue->type));
    long    min  = pgm_read_dword(&(thisValue->min));
    long    max  = pgm_read_dword(&(thisValue->max));
    
    uint8_t mode = (p_type == CHANGE_UP) ? MODE_INCREMENT : MODE_DECREMENT;
    

    if( p_type == CHANGE_ABORT ) {
        m_inEdit = false;
        _activate(m_curParent, true);
    }
    
    else if( p_type == CHANGE_UP || p_type == CHANGE_DOWN || p_type == MODE_NOOP ) {
        if( type == TYPE_SELECT ) {
            _modifySel(thisValue, mode);
        }
        else if( type == TYPE_BFLAG ) {
            m_temp = (m_temp == 1) ? 0 : 1;    
            _displayFlagVal();
        }
        else
            _modifyTemp(type, mode, min, max);
    }
    
    else if( p_type == CHANGE_SAVE ) {
        
        void* ptr = reinterpret_cast<void*>(pgm_read_word(&(thisValue->targetValue)));
        
        switch(type) {
            case TYPE_SELECT:
                    // select is more special than the rest, dig?
                {
                        // some what convoluted - we get the value stored in the current index (in m_temp) from the list,
                        // and store it in the byte pointer provided attached to the OMMenuSelectValue
                    OMMenuSelectValue* sel      = reinterpret_cast<OMMenuSelectValue*>(ptr);
                    OMMenuSelectListItem** list = reinterpret_cast<OMMenuSelectListItem**>(reinterpret_cast<void*>(pgm_read_word(&(sel->list))));
                    OMMenuSelectListItem* item  = reinterpret_cast<OMMenuSelectListItem*>(pgm_read_word(&(list[m_temp])));
                    uint8_t newVal              = pgm_read_byte(&(item->value));
                    uint8_t* real               = reinterpret_cast<uint8_t*>(pgm_read_word(&(sel->targetValue)));                
                    *real                       = newVal;
                    
                    _eewrite<uint8_t>(thisValue, newVal);
                }
                break;
            case TYPE_BFLAG:
                    // bflag is special too, we want to set a specific bit based on the current temp value
                {
                    OMMenuValueFlag* flag = reinterpret_cast<OMMenuValueFlag*>(ptr);
                    uint8_t*       target = reinterpret_cast<uint8_t*>(pgm_read_word(&(flag->flag)));
                    uint8_t           pos = pgm_read_byte(&(flag->pos));
                    
                    if( m_temp )
                        *target |= (1 << pos);
                    else
                        *target &= (0xFF ^ (1 << pos));
                    
                    _eewrite<uint8_t>(thisValue, *target);
                }
                break;
            case TYPE_BYTE:
                *reinterpret_cast<uint8_t*>(ptr) = m_temp;
                _eewrite<uint8_t>(thisValue, *reinterpret_cast<uint8_t*>(ptr));
                break;
            case TYPE_UINT:
                *reinterpret_cast<unsigned int*>(ptr) = *reinterpret_cast<unsigned int*>(&m_tempI);
                _eewrite<unsigned int>(thisValue, *reinterpret_cast<unsigned int*>(ptr));
                 break;
            case TYPE_INT:
                *reinterpret_cast<int*>(ptr) = m_tempI;
                _eewrite<int>(thisValue, *reinterpret_cast<int*>(ptr));
                break;
            case TYPE_ULONG:
                *reinterpret_cast<unsigned long*>(ptr) = *reinterpret_cast<unsigned long*>(&m_tempL);
                _eewrite<unsigned long>(thisValue, *reinterpret_cast<unsigned long*>(ptr));
                break;
            case TYPE_LONG:
                *reinterpret_cast<long*>(ptr) = m_tempL;
                _eewrite<long>(thisValue, *reinterpret_cast<long*>(ptr));
                break;
            case TYPE_FLOAT:
            case TYPE_FLOAT_10:
            case TYPE_FLOAT_100:
            case TYPE_FLOAT_1000:
                *reinterpret_cast<float*>(ptr) = m_tempF;
                _eewrite<float>(thisValue, *reinterpret_cast<float*>(ptr));
                break;
        }
        
        m_inEdit = false;
        _activate(m_curParent, true);
    }
}


 // add a menu level to the history

void OMMenuMgr::_pushHist(OMMenuItem* p_item) {
        // note that if you have no room left, you'll lose this
        // item - we only store up to MAXDEPTH
    for(uint8_t i = 0; i < OM_MENU_MAXDEPTH; i++) {
        if( m_hist[i] == 0 ) {
            m_hist[i] = p_item;
            return;
        }
    }
}

 // remove the latest menu item from the history and return it

OMMenuItem* OMMenuMgr::_popHist() {
        // work backwards, remove the first non-zero pointer
        // and return it
    for(uint8_t i = OM_MENU_MAXDEPTH; i > 0; i--) {
        if( m_hist[i-1] != 0 ) {
            OMMenuItem* item = m_hist[i-1];
            m_hist[i-1] = 0;
            return item;
        }
    }
    
    return 0;
}
