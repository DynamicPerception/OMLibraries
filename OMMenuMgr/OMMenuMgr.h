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

#ifndef	OM_MENUMGR_H
#define OM_MENUMGR_H

#include "Arduino.h"
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "OMEEPROM.h"



#ifndef OM_MENU_ROWS
    #define OM_MENU_ROWS    2
#endif

#ifndef OM_MENU_COLS
    #define OM_MENU_COLS    16
#endif

#ifndef OM_MENU_LBLLEN
    #define OM_MENU_LBLLEN 15
#endif

#ifndef OM_MENU_MAXDEPTH
    #define OM_MENU_MAXDEPTH    3
#endif

#ifndef OM_MENU_DEBOUNCE
    #define OM_MENU_DEBOUNCE 120
#endif

#ifndef OM_MENU_CURSOR
    #define OM_MENU_CURSOR ">"
#endif

#ifndef OM_MENU_NOCURSOR
    #define OM_MENU_NOCURSOR " "
#endif

#ifndef OM_MENU_FLAG_ON
    #define OM_MENU_FLAG_ON "On"
#endif

#ifndef OM_MENU_FLAG_OFF
    #define OM_MENU_FLAG_OFF "Off"
#endif



#define MENU_ITEM           PROGMEM OMMenuItem
#define MENU_LIST           PROGMEM OMMenuItem*
#define MENU_VALUE          PROGMEM OMMenuValue
#define MENU_FLAG           PROGMEM OMMenuValueFlag
#define MENU_SELECT_ITEM    PROGMEM OMMenuSelectListItem
#define MENU_SELECT_LIST    PROGMEM OMMenuSelectListItem*
#define MENU_SELECT         PROGMEM OMMenuSelectValue
#define MENU_SELECT_SIZE(x) sizeof(x) / sizeof(OMMenuSelectListItem*)
#define MENU_SIZE(x)        sizeof(x) / sizeof(OMMenuItem*)
#define MENU_TARGET(x)      reinterpret_cast<void*>(x)


/** Select-Type Item

 Item to be added to a list for OMSelectValue
 */
struct OMMenuSelectListItem {
    uint8_t   value;
    prog_char label[OM_MENU_LBLLEN];
};



/** Select-Type Target Value
 */

struct OMMenuSelectValue {
        /** Pointer to target variable */
    uint8_t* targetValue;
    uint8_t listCount;
        /** Void Pointer to list of select items (OMMenuSelectListItem**) */
    void* list;
};



struct OMMenuValueFlag {
    uint8_t  pos;
    uint8_t* flag;
};

/** Menu Item Type

  Defines a menu item. Stored in PROGMEM

 label is the label to be displayed for the item.

 type can be one of:

  ITEM_MENU, ITEM_VALUE, ITEM_ACTION

 If type is ITEM_MENU, then targetCount will be the
 count of menu items in the menu, and target will be
 a pointer to an array of OMMenuItems

 If type is ITEM_ACTION, then targetCount should be zero
 and target should resolve to a function pointer with the prototype
 void(*)(void)

 If type is ITEM_VALUE, then targetCount should be zero, and
 target should be a pointer to an OMMenuValue.
 */

struct OMMenuItem {
    prog_char     label[OM_MENU_COLS];
    uint8_t       type;
    uint8_t       targetCount;
    void*         target;
};


/** Menu Value

 A value to be edited by the user.  The type specifies the type of value
 pointed at by the targetValue pointer.  Min and Max specify a minimum and
 maximum value (within the range of a signed long).  These are unused for the
 TYPE_SELECT types

 */
struct OMMenuValue {
    uint8_t type;
    long    max;
    long    min;
    void*   targetValue;
    int     eepromLoc;
};


enum  { ITEM_MENU, ITEM_VALUE, ITEM_ACTION };
enum  { MENU_ANALOG, MENU_DIGITAL };
enum  { BUTTON_NONE, BUTTON_FORWARD, BUTTON_BACK, BUTTON_INCREASE, BUTTON_DECREASE, BUTTON_SELECT, BUTTON_HELD };
enum  { CHANGE_DISPLAY, CHANGE_UP, CHANGE_DOWN, CHANGE_SAVE, CHANGE_ABORT };
enum  { TYPE_BYTE, TYPE_INT, TYPE_UINT, TYPE_LONG, TYPE_ULONG, TYPE_FLOAT, TYPE_FLOAT_10, TYPE_FLOAT_100, TYPE_FLOAT_1000, TYPE_SELECT,
                 TYPE_BFLAG };
enum  { MODE_INCREMENT, MODE_DECREMENT, MODE_NOOP };





/** OpenMoCo Menu Manager Class

 @brief

 The OpenMoCo Menu Manager provides a nearly complete automation of a menuing system.  This
 class is designed to allow for rapid development of menuing systems on Arduino devices, using
 five input controls (buttons), and character displays of at least two rows.

 Designed to make it easy to implement menu systems, the Menu Manager requires the developer to do little
 more than specify the structure of the menu, and how to draw on the display.

 Key features:<br>
 <ul>
    <li>Menu structures automatically and easily stored in program memory instead of SRAM</li>
    <li>Automatic handling of either analog or digital button inputs</li>
    <li>Automatic management of user inputs for numeric and select list types</li>
    <ul>
        <li>In-place editing with ability for user to abort</li>
        <li>Write-back on user save to original variables</li>
        <li>Specify lists of display values for users to select between</li>
        <li>Control over user input precision in floats, and more</li>
    </ul>
    <li>Ability to trigger code actions from menu items</li>
    <li>One-method polling automatically handles and executes menu as-needed</li>
    <li>Streamlined setup for complex and even recursive menus</li>
    <li>Support for any width screen</li>
    <li>Reduced memory footprint</li>
    <li>No fixed limitations on menu depths, have as many sub-menus as you require</li>
    <li>Flexible drawing methodology allows the user of any character-based (or even graphic) display</li>
 </ul>

 @author
 C. A. Church
 Dynamic Perception LLC

 See <http://www.dynamicperception.com> for more information

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


 @section menuoverview Overview

 The Menu Manager displays a character-based menu when triggered by the user.  Five user
 inputs are used (although only four are required): Enter, Up, Down, Back, and Forward.
 Forward is optional.  These inputs may either be buttons tied to digital pins, or up
 to five buttons tied to a single analog pin using a series of resistors to present
 different analog values when a button is pressed.

 The menu is triggered by hitting the Enter key by the user when the menu is enabled.
 When it is displayed, the user is presented with all available options at the current
 menu level, one per row, with a cursor indicating the currently selected item.  The up
 and down keys scroll the list through all available items at the current menu level.
 If the user presses enter, or forward on the current item, the item will be presented as
 either a value edit, a new menu, or an action will be executed as defined by the user
 based on the type of the menu entry.  The back button will cancel out of the current input,
 go back one menu level, or exit the menu entirely if the user is at the root menu level.

 When a value is being edited, the user may adjust the value within the range specified
 by the developer non-destructively using the up and down buttons.  The user may cancel
 the edit of the value by using the back button, or may save the value using either of
 enter or forward.  When the user saves the value, it is written directly to the variable
 specified by the developer.  All numeric types are supported; individual bit flags, and special select lists
 are supported as well.  For select lists, the user is presented with strings defined by the
 developer for the numeric values stored in the target variable, providing the ability to
 give the user a more complete and obvious solution.

 Menu items may also be sub-menus.  You can even re-use the same sub-menus under different
 menus with minimal impact to flash size and SRAM usage, letting you use the same menus
 under different contexts.

 Menu items may be actions - you may wish to draw special screens and allow for use-case
 driven inputs.  In this manner, you can trigger callbacks when menu items are triggered
 and disable the menu easily while interacting with the user, and then seamlessly place
 the user back into the menu where they left off when they are done.

 Finally, the Menu Manager can be used to easily poll the user inputs without drawing the
 menu as needed by the developer.

 @section menuitembasic The Basics of Menu Items

 To create menu items, you'll need to specify their contents and create them using special
 datatypes which will keep them in program memory until needed.  The basic item is defined
 via the MENU_ITEM data type, and specifies the label to give to the item, the type of item
 it is, the length (for certain types), and the target of the item.  For example:

 @code
                //        LABEL           TYPE        LENGTH    TARGET
 MENU_ITEM item_foo  = { {"Foo Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&value_foo) };
 @endcode

 There are three cores types of items:
 <ul>
    <li>ITEM_VALUE</li>
    <ul>
        <li>A value to be edited, which will either be a numeric type or a select list</li>
    </ul>
    <li>ITEM_ACTION</li>
    <ul>
        <li>An action to be executed when the user hits enter or forward on the item</li>
    </ul>
    <li>ITEM_MENU</li>
    <ul>
        <li>A sub-menu to be displayed when the user hits enter or forward on the item</li>
    </ul>
 </ul>

 To create a value item, we must also create a special variables which tells the menu system
 how to process the item.  This special variable will be constructed again from a new type,
 with several parameters:

 @code

 byte foo = 0;

                 //       Data Type        Max    Min    Target
 MENU_VALUE value_foo = { TYPE_BYTE,       100,   0,     MENU_TARGET(&foo) };

                 //        LABEL           TYPE        LENGTH    TARGET
 MENU_ITEM item_foo  = { {"Foo Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&value_foo) };
 @endcode

 Here, we see that we can specify the type of data the item points to, a maximum value, a minimum
 value, and the target variable (by address) to be edited.  Minimum and Maximum values are limited
 to the range of a signed long, no matter the data type, and are not used for select lists.

 The following Data Type values are allowed:

 <ul>
    <li>TYPE_BYTE</li>
    <li>TYPE_INT</li>
    <li>TYPE_UINT</li>
    <li>TYPE_LONG</li>
    <li>TYPE_ULONG</li>
    <li>TYPE_FLOAT</li>
    <ul>
        <li>Whole integer changes, zero decimal precision per change</li>
    </ul>
    <li>TYPE_FLOAT_10</li>
    <ul>
        <li>Change by tenths</li>
    </ul>
    <li>TYPE_FLOAT_100</li>
    <ul>
        <li>Change by hundredths</li>
    </ul>
    <li>TYPE_FLOAT_1000</li>
    <ul>
        <li>Change by thousandths</li>
    </ul>
    <li>TYPE_SELECT</li>
    <li>TYPE_BFLAG</li>
 </ul>

 @section menulists The Basics of Creating Menus

 Of course, items on their own are of little use - we wouldn't typically go through all of this
 work to just have a single input! To create a menu, we need to create a list of items in that
 menu, and then create the item which represents the menu.  For this, again, we have a special data
 type that allows us to easily create them and store in program space:

 @code
 MENU_LIST root_list[] = { &item_foo, &item_bar };

 MENU_ITEM menu_root = { {"Root Menu"},  ITEM_MENU,  MENU_SIZE(root_list),  MENU_TARGET(&root_list) };
 @endcode

 We may, of course, create multiple lists, use the same list in multiple items, and use the same items
 in multiple lists.

 @section menuselect The Basics of Creating Select Lists

 Creating a select list is a little more complex than a normal item value.  Since we're displaying the
 user a list of strings, and mapping them back to numeric values, we need to create this mapping.

 To do so, we need to create the items that will appear in lists, and the lists which use them:

 @code
 MENU_SELECT_ITEM  sel_ign = { 2, {"Ignore"} };
 MENU_SELECT_ITEM  sel_on  = { 1, {"On"} };
 MENU_SELECT_ITEM  sel_off = { 0, {"Off"} };

 MENU_SELECT_LIST  state_list[] = { &sel_ign, &sel_on, &sel_off };
 @endcode

 Note here, for each item we specify the numeric value associated with the string, and the string to
 display to the user.  We can use these select items in any select list, but it is important to note
 that you should not duplicate a numeric value in one list.

 The numeric values are specified as a byte type, and are limited to 0-255.

 Finally, to create the target for a menu item to use this select list, we must create a special
 variable that specifies how to handle the select, where to store the target value, the length
 of the list, and the list to use:

 @code
 byte myVar = 0;

                           // TARGET VAR        LENGTH                          TARGET SELECT LIST
 MENU_SELECT state_select = { &myVar,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };

 MENU_VALUE  value_sel = { TYPE_SELECT, 0, 0, MENU_TARGET(&state_select) };
 MENU_ITEM   item_sel  = { {"Select It"}, ITEM_VALUE, 0, MENU_TARGET(&value_sel) };
 @endcode

 @section menubitflag The Basics of Creating Bit Flag Inputs

 A bit flag input allows you to change only one bit in a target variable, for example, in the case where you want
 to store up to 8 on/off flags in a single byte of data to preserve RAM.  The Menu Manager supports doing this by
 creating a special target for the MENU_VALUE you're creating:

 @code

 byte theseFlags = 0;

 MENU_FLAG    my_flag    = { 3, &theseFlags };
 MENU_VALUE   my_value   = { TYPE_BFLAG, 0, 0, MENU_TARGET(&my_flag) };
 MENU_ITEM    my_item    = { {"Flag Edit"}, ITEM_VALUE, 0, MENU_TARGET(&my_value) };
 @endcode

 We'll note that the new MENU_FLAG type has two parts: the bit position, and the address of the byte containing the bit.  The bit
 position is from the right and is of the range 0-7, so this item would modify the bit marked with 'x' in the following pattern:
 B1111x111

 In this example, when the user interacts with the "Flag Edit" menu item, the user will be displayed the OM_MENU_FLAG_ON and
 OM_MENU_FLAG_OFF strings based on the value of bit 3 in the 'theseFlags' variable.  When saved by the user, the correct bit
 value will be written back to bit 3 in 'theseFlags'.

 If you want to change the strings used for On and Off, see the \ref menuparam "Setting Control Parameters" section below.


 @section menuaction The Basics of Creating Actions

 An action is a function that will be called when a user presses enter or forward on that item.  This can be
 used to display special screens, or call some other behavior more complex than just setting a parameter.

 All functions being called by actions must take no parameters, and return void.  When an action is called, your
 callback handler is called.  If the menu is still enabled when your callback handler exits, the menu will be re-drawn
 where it was when the action was initiated, otherwise the menu will not be displayed again until the menu is re-enabled
 and the checkInput() method is called again.  (For more info on disabling the menu, and polling inputs, see the
 \ref menupolling "Polling the Menu" section below.)

 For example, we can create a special screen that is drawn when an action is triggered, which waits until the user presses
 a button before returning to the menu:

 @code
 MENU_ITEM item_testme   = { {"Test Action"},  ITEM_ACTION, 0,        MENU_TARGET(uiQwkScreen) };

 ...

 void uiQwkScreen() {
    lcd.clear();
    Menu.enable(false);

    lcd.print("Action!");
    lcd.setCursor(0, 1);
    lcd.print("Enter 2 return");

    while( Menu.checkInput() != BUTTON_SELECT ) {
        ; // wait!
    }

    Menu.enable(true);
    lcd.clear();
 }
 @endcode


 @section menuinit Initializing the Menu

 After we have created all of our menu items, lists, selects, and so forth, we need to initialize the
 OMMenuMgr object with a root item.  This root item will be the starting point when the user
 triggers the menu, and should be an ITEM_MENU in most cases.

 Additionally, we need to specify whether our input is analog (MENU_ANALOG, default), or digital
 (MENU_DIGITAL).

 @code
 OMMenuMgr Menu = OMMenuMgr(&menu_root, MENU_ANALOG);
 @endcode

 Note that we always pass the root item by address, and not by value!  You can always change the root item
 later, using the setRoot() method.


 @section menuinput User Input

 After we have created our menu structure, we must specify how to handle user input.  To do this, we must
 tell the Menu Manager how to read our buttons.

 In both analog and digital cases, we must create a two-dimensional list of how to read buttons, and their
 meanings.  This list will always have 5 lists of 2 elements.  The format is:

 @code
 int map = {
             { button 1 read, button 1 definition },
             { button 2 read, button 2 definition },
            ...
           };
 @endcode

 The button definition is always one of the following:

 BUTTON_FORWARD, BUTTON_BACK, BUTTON_INCREASE, BUTTON_DECREASE, BUTTON_SELECT

 The how to read button instruction will differ based on whether you are doing analog
 or digital input.  For digital inputs, the first element will be the Arduino digital
 pin # of the button, and for analog inputs, the first element will be analogRead()
 value associated with the button.

 For digital input, we simply need to pass this array to the setDigitalButtonPins()
 method.  Also note, that for digital inputs the input pins will be pulled HIGH internally
 and you are expected to swing them to LOW when pressed.

 For analog input, we must also specify the pin to read the value from, and the threshold
 for reading the values.  The threshold is very important, as analog inputs are rarely exact;
 this threshold is the variance that will be applied to each value during comparison.  These
 values and the button definition array are sent to the setAnalogButtonPin() method.

 The following is a complete example of initializing an analog input:

 @code
 // which input is our button (use the digital pin#, not the analog pin #!)
 const byte BUT_PIN = 14;

 // analog button read values
 const int BUTSEL_VAL  = 70;
 const int BUTFWD_VAL  = 250;
 const int BUTREV_VAL  = 450;
 const int BUTDEC_VAL  = 655;
 const int BUTINC_VAL  = 830;

 const byte BUT_THRESH  = 60;

 // mapping of analog button values for menu
 const int BUT_MAP[5][2] = {
                                {BUTSEL_VAL, BUTTON_SELECT},
                                {BUTINC_VAL, BUTTON_INCREASE},
                                {BUTDEC_VAL, BUTTON_DECREASE},
                                {BUTREV_VAL, BUTTON_BACK},
                                {BUTFWD_VAL, BUTTON_FORWARD}
                            };



 ...

 void setup() {

   ...

     Menu.setAnalogButtonPin(BUT_PIN, BUT_MAP, BUT_THRESH);
 }
 @endcode

 If you do not wish to use the forward button, it is not required.  However, you must
 still specify five buttons in the array.  To ignore the forward button (if you want
 to use only four, instead of five, buttons), simply make sure that BUTTON_FORWARD is the
 last element in the array, and re-use one of the earlier read values.

 @section menupolling Polling the Menu

 The menu must be polled regularly, such as during your loop() function, to check for input,
 display the menu, or interact with it.  A single method is all that needs to be called for this
 activity.

 @code
 void loop() {

    Menu.checkInput();

 }
 @endcode

 This method returns the button pressed or BUTTON_NONE if none were, so you may also use it to handle user input
 if you need, and disable the menu if required, see:

 @code
 void loop() {

    if( hideMenu ) {
        Menu.enable(false);
        hideMenu = false;
    }

    byte button = Menu.checkInput();

    if( ! Menu.enable() ) {
        // menu is disabled

        if( button == BUTTON_SELECT ) {
            // do something!
        }
        else if( button == BUTTON_FORWARD ) {
            // re-enable Menu
            Menu.enable(true);
        }
    }

 }
 @endcode


 @section menudisplay Managing Display

 While the Menu Manager creates the content for display, and manages what data should be
 displayed and when, it does not have any built-in display methodology.  This allows you
 to use nearly any display you like, since you will be controlling how the data is sent
 to the display.

 To achieve this, a callback model is used - you specify a function for each required
 display activity, and pass them as arguments to the set...Handler() methods.

 Three functions are required: Draw, and Exit.

 Draw handles displaying strings to the screen.  It is passed four arguments: a character pointer,
 a row, a column, and the length of the characters stored at the address in the character pointer.

 <b>The character pointer will not always be null-terminated, and cannot be relied upon to be
 null-terminated.  Always use the length argument, which can be relied upon.</b>

 The following shows a model handler for drawing using a standard LiquidCrystal object named 'lcd':

 @code
 void setup() {

   ...

     Menu.setDrawHandler(uiDraw);
 }


 ...

 void uiDraw(char* p_text, int p_row, int p_col, int len) {
    lcd.setCursor(p_col, p_row);

    for( int i = 0; i < len; i++ ) {
        if( p_text[i] < '!' || p_text[i] > '~' )
            lcd.write(' ');
        else
            lcd.write(p_text[i]);
    }
 }
 @endcode

 Note that in the above function, we handle characters outside of the normal ascii range,
 as the MenuMgr cannot gaurantee that every character is displayable.

 Exit is called when the user exits the menu, and it is cleared.  It should bring you to
 whatever state you need to be when the menu isn't displayed.

 The following is a model example of a clear callback handler:

 @code
 void uiExit() {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter for Menu");
 }
 @endcode

 @section menuparam Setting Control Parameters

 Of course, not every display is the same.  Display parameters are set at compile time, and controlled
 via define's.

 Unfortunately, the Arduino IDE does not allow you to specify defines to the compiler, so you cannot effectively change these
 from the Arduino IDE.  For other IDE users, you may re-define them via your build process.  For Arduino IDE users, you will need to
 edit the library directly.

 <ul>
    <li>OM_MENU_ROWS</li>
    <ul>
        <li>Default = 2</li>
        <li>Number of rows in the display, minimum is 2</li>
    </ul>
    <li>OM_MENU_COLS</li>
    <ul>
        <li>Default = 16</li>
        <li>Number of columns (character width) in the display.  This also controls the maximum length of any label for a menu item</li>
    </ul>
    <li>OM_MENU_LBLLEN</li>
    <ul>
        <li>Default = OM_MENU_COLS</li>
        <li>Maximum length of a select label, this can be used to reduce flash usage when only short labels are needed</li>
    </ul>
    <li>OM_MENU_MAXDEPTH</li>
    <ul>
        <li>Default = 3</li>
        <li>Maximum nested depth of menus.  You can go deeper than this, but menu history will not be retained below this level.</li>
    </ul>
    <li>OM_MENU_DEBOUNCE</li>
    <ul>
        <li>Default = 120</li>
        <li>Debounce time for button input, in mS</li>
    </ul>
    <li>OM_MENU_CURSOR</li>
    <ul>
        <li>Default = ">"</li>
        <li>Cursor to show for current item selection</li>
    </ul>
    <li>OM_MENU_NOCURSOR</li>
    <ul>
        <li>Default = " "</li>
        <li>Cursor to show for displayed items not currently selected</li>
    </ul>
    <li>OM_MENU_USE_EEPROM</li>
    <ul>
        <li>Default = Not Defined</li>
        <li>Support automatic writing of variables to EEPROM via OMEEPROM library.  For more info, see the \ref menueeprom "Automating EEPROM Writes" section below.</li>
    </ul>
    <li>OM_MENU_FLAG_ON</li>
    <ul>
        <li>Default = "On"</li>
        <li>The string to be displayed to the user for bit flag values where the flag is turned on</li>
    </ul>
    <li>OM_MENU_FLAG_OFF</li>
    <ul>
        <li>Default = "Off"</li>
        <li>The string to be displayed to the user for bit flag values where the flag is turned off</li>
    </ul>
 </ul>



 @section menueeprom Automating EEPROM Writes

 The Menu Manager can automatically store changed variables to EEPROM for you, using the OMEEPROM library.  You can retrieve the data, do EEPROM format versioning, and
 more with the OMEEPROM library.

 Each menu value has a final parameter, with the EEPROM byte address to store the data.  If you do not want to store the data in EEPROM,
 use 0 as the address, or simply not pass that parameter at all.  For example:

 @code

 #include "OMEEPROM.h"
 #include "OMMenuMgr.h"

 ...

 const int EEPROM_NOADDR    = 0;
 const int EEPROM_FOO       = 10;

 unsigned int foo = 1;
 unsigned int bar = 1;
 unsigned int baz = 1;

 ...

 MENU_VALUE value_foo = { TYPE_UINT, 0, 0, MENU_TARGET(&foo), EEPROM_FOO };

 MENU_VALUE value_bar = { TYPE_UINT, 0, 0, MENU_TARGET(&foo), EEPROM_NOADDR };
 MENU_VALUE value_baz = { TYPE_UINT, 0, 0, MENU_TARGET(&baz) }; // equivalent to providing no address at the end
 @endcode

 Now, when these values are exercised by the user, foo will be stored to EEPROM, starting at byte 10, and bar will not be stored in EEPROM.


 @section menuexample A Complete Example

 The following is a complete example of a sketch with a menu:

 @code
#include "OMMenuMgr.h"
#include <LiquidCrystal.h>


// lcd pins
const byte LCD_RS  = 17;
const byte LCD_EN  = 18;
const byte LCD_D4  = 11;
const byte LCD_D5  = 8;
const byte LCD_D6  = 7;
const byte LCD_D7  = 4;


const byte LCD_ROWS = 2;
const byte LCD_COLS = 16;

// button values

// which input is our button
const byte BUT_PIN = 14;

// analog button read values
const int BUTSEL_VAL  = 70;
const int BUTFWD_VAL  = 250;
const int BUTREV_VAL  = 450;
const int BUTDEC_VAL  = 655;
const int BUTINC_VAL  = 830;

const byte BUT_THRESH  = 60;

// mapping of analog button values for menu
const int BUT_MAP[5][2] = {
    {BUTFWD_VAL, BUTTON_FORWARD},
    {BUTINC_VAL, BUTTON_INCREASE},
    {BUTDEC_VAL, BUTTON_DECREASE},
    {BUTREV_VAL, BUTTON_BACK},
    {BUTSEL_VAL, BUTTON_SELECT}
};



// ====== Test Menu ===========

byte foo = 0;
byte sel = 0;
unsigned int bar = 1;
long baz  = 0;
float bak = 0.0;

// Create a list of states and values for a select input
MENU_SELECT_ITEM  sel_ign = { 2, {"Ignore"} };
MENU_SELECT_ITEM  sel_on  = { 1, {"On"} };
MENU_SELECT_ITEM  sel_off = { 0, {"Off"} };

MENU_SELECT_LIST  state_list[] = { &sel_ign, &sel_on, &sel_off };

// the special target for our state input

// TARGET VAR   LENGTH                          TARGET SELECT LIST
MENU_SELECT state_select = { &sel,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };

// values to use

//    TYPE            MAX    MIN    TARGET
MENU_VALUE foo_value = { TYPE_BYTE,       100,   0,     MENU_TARGET(&foo) };
MENU_VALUE bar_value = { TYPE_UINT,       10000, 100,   MENU_TARGET(&bar) };
MENU_VALUE baz_value = { TYPE_LONG,       10000, 1,     MENU_TARGET(&baz) };
MENU_VALUE bak_value = { TYPE_FLOAT_1000, 0,     0,     MENU_TARGET(&bak) };
MENU_VALUE sel_value = { TYPE_SELECT,     0,     0,     MENU_TARGET(&state_select) };

//        LABEL           TYPE        LENGTH    TARGET
MENU_ITEM item_checkme  = { {"Byte Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&foo_value) };
MENU_ITEM item_barme    = { {"UInt Edit"},     ITEM_VALUE,  0,        MENU_TARGET(&bar_value) };
MENU_ITEM item_bazme    = { {"Long Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&baz_value) };
MENU_ITEM item_bakme    = { {"Float Edit"},   ITEM_VALUE,  0,        MENU_TARGET(&bak_value) };
MENU_ITEM item_state    = { {"Select Input"}, ITEM_VALUE,  0,        MENU_TARGET(&sel_value) };
MENU_ITEM item_testme   = { {"Test Action"},  ITEM_ACTION, 0,        MENU_TARGET(uiQwkScreen) };

//        List of items in menu level
MENU_LIST root_list[]   = { &item_checkme, &item_barme, &item_bazme, &item_bakme, &item_state, &item_testme };

// Root item is always created last, so we can add all other items to it
MENU_ITEM menu_root     = { {"Root"},        ITEM_MENU,   MENU_SIZE(root_list),    MENU_TARGET(&root_list) };


// initialize LCD object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

OMMenuMgr Menu(&menu_root);



void uiMenuSetup() {

    lcd.begin(LCD_COLS, LCD_ROWS);

    uiClear();

    Menu.setDrawHandler(uiDraw);
    Menu.setExitHandler(uiClear);
    Menu.setAnalogButtonPin(BUT_PIN, BUT_MAP, BUT_THRESH);
    Menu.enable(true);


}

void uiCheck() {
    Menu.checkInput();

}

void testAction() {

    digitalWrite(5, HIGH);
}


void uiDraw(char* p_text, int p_row, int p_col, int len) {
    lcd.setCursor(p_col, p_row);

    for( int i = 0; i < len; i++ ) {
        if( p_text[i] < '!' || p_text[i] > '~' )
            lcd.write(' ');
        else
            lcd.write(p_text[i]);
    }
}


void uiClear() {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter for Menu");
}


void uiQwkScreen() {
    lcd.clear();
    Menu.enable(false);

    lcd.print("Action!");
    lcd.setCursor(0, 1);
    lcd.print("Enter 2 return");

    while( Menu.checkInput() != BUTTON_SELECT ) {
        ; // wait!
    }

    Menu.enable(true);
    lcd.clear();
}
 @endcode

*/

class OMMenuMgr {


public:

    OMMenuMgr(OMMenuItem* c_first, uint8_t c_type = MENU_ANALOG);

    void setAnalogButtonPin(uint8_t p_pin, const int p_values[5][2], int p_thresh);
    void setDigitalButtonPins(const int p_pins[5][2]);

    uint8_t checkInput();

    void enable(bool p_en);
    bool enable();

    void setDrawHandler(void(*p_func)(char*, int, int, int));
    void setExitHandler(void(*p_func)());

    void setRoot(OMMenuItem* p_root);

    bool shown();

    unsigned int holdModifier();

private:

    typedef void(*f_valueHandler)();
    typedef void(*f_drawHandler)(char*, int, int, int);
    typedef void(*f_seekHandler)(int, int);

    uint8_t        m_inputType;
    uint8_t        m_anaPin;
    int            m_anaThresh;
    bool           m_enable;
    bool           m_inEdit;
    bool           m_menuActive;
    bool           m_forceReturn;
    OMMenuItem*    m_curSel;
    OMMenuItem*    m_curParent;
    OMMenuItem*    m_rootItem;
    OMMenuItem*    m_hist[OM_MENU_MAXDEPTH];
    int            m_butVals[5][2];
    uint8_t        m_curTarget;
    f_drawHandler  m_draw;
    f_valueHandler m_exit;
    char           m_dispBuf[OM_MENU_COLS];
    unsigned int   m_holdMod;

    uint8_t       m_temp;
    long          m_tempL;
    int           m_tempI;
    float         m_tempF;


    int         _checkAnalog();
    int         _checkDigital();
    void        _handleButton(uint8_t p_key);
    void        _activate(OMMenuItem* p_item, bool p_return = false);
    void        _edit(OMMenuItem* p_item, uint8_t p_type);
    void        _displayList(OMMenuItem* p_item, uint8_t p_target = 0);
    void        _displayEdit(OMMenuItem* p_item);
    void        _menuNav(uint8_t p_mode);
    void        _pushHist(OMMenuItem* p_item);
    OMMenuItem* _popHist();
    void        _display(char* p_str, int p_row, int p_col, int p_count);
    void        _displayVoidNum(void* p_ptr, uint8_t p_type, int p_row, int p_col);
    void        _modifyTemp(uint8_t p_type, uint8_t p_mode, long p_min, long p_max);
    void        _exitMenu();
    void        _modifySel(OMMenuValue* p_value, uint8_t p_mode);
    void        _displaySelVal(OMMenuSelectListItem** p_list, uint8_t p_idx);
    void        _displayFlagVal();


    // Handle templates for EEPROM writing of different data types

    template <typename T>
    void _eewrite(OMMenuValue* p_target, T p_item) {
        int loc = pgm_read_word(&(p_target->eepromLoc));
        if( loc != 0 )
            OMEEPROM::write(loc, p_item);
    }

};

#endif //OM_MENUMGR_H
