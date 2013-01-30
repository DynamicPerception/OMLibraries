/*
 * Menu Library v.2
 * OpenMoco MoCoBus Core Libraries
 * See www.dynamicperception.com for more information
 * File: MenuEngine.h
 *
 *  Created on: Jun 4, 2012
 *      Author: perepelitsa
 */

#ifndef MENUENGINE_H_
#define MENUENGINE_H_

#include <inttypes.h>

#include "AnlgBtnInput.h"
#include "HorizMenu.h"
#include "MenuContext.h"
#include "Commander.h"

#define HUNDRED_MS 100

#define MS_TIMER 0



/** OpenMoCo OMMenu Class

 @brief

 The OpenMoCo OMenu provides flexible customizing menu system.  This
 class is designed to allow for rapid development of menuing systems on Arduino devices,
 which make use of:
 - five input controls (buttons),
 - and character displays of at least two rows.

 Designed to make it easy to implement menu systems, the Menu Manager requires the developer
 to do the following:
  - specify the structure of the menu,
  - allocate parameters to menu items and types,
  - provide code for action screen if any.

 Key features:<br>
 <ul>
    <li>Menu resources stored in program memory instead of SRAM</li>
    <li>Configurable using of either analog or digital button inputs</li>
    <li>Associate type of user inputs for numeric or select list or another by index</li>
    <ul>
        <li>In-place editing with ability for user to abort</li>
        <li>Write-back on user save to original variables</li>
        <li>Specify lists of display values for users to select between</li>
        <li>Control over user input precision in floats, and more</li>
    </ul>
    <li>Ability to trigger code actions from menu items</li>
    <li>One-method polling automatically handles and executes menu as-needed</li>
    <li>All menu resources indirected by index, to reduce memory footprint </li>
    <li>Menu depth is adjustable as define</li>
    <li>Flexible menu screen building allows reading resources from any memory type (ext. EEPROM)</li>
 </ul>

 @author
 S. Perepelitsa

 @section menuoverview Overview

 The Menu Manager displays a character-based menu when triggered by the user.  Five user
 inputs are used: Enter, Up, Down, Back, and Forward.
 These inputs may either be buttons tied to digital pins, or up
 to five buttons tied to a single analog pin using a series of resistors as voltage divider
 to present different voltage when a button is pressed.

 Upon start menu engine begins in Idle state, displaying idle screen.
 The menu is triggered by hitting the Enter key by the user when the menu is idling.
 When it is displayed, the user is presented with all available items at the current
 menu level, but only one per screen, with a cursor indicating availability of next and previous items.
 The left and right keys scroll the list through all available items at the current menu level.
 If the user presses enter on the current item, the item will be presented as
 either a value edit, a new menu, or an action will be executed as defined by the user
 based on the type of the menu entry.  The back button will cancel out of the current input,
 go back one menu level, or exit the menu to the Idle if the user is at the root menu level.
 Menu engine exits to idle on user inactivity as well.

 When a value is being edited, the user may adjust the value within the range specified
 by the developer non-destructively using the up and down buttons.  The user may cancel
 the edit of the value by using the back button, or may save the value using either of
 enter or forward.  When the user saves the value, it is written directly to the associated variable
 specified in resource by the developer.
 All numeric types are supported; individual bit flags, time type and special select lists
 are supported as well.
 For select lists, the user is presented with strings defined by the
 developer for the numeric values stored in the target variable, providing the ability to
 give the user a more complete and obvious solution. Strings may be fixed or generated in runtime,
 but neither type is allowed to edit strings.

 Menu items may also be sub-menus of two kind - wizard like and same as main.
 In wizard mode one can navigate thru wizard screens using left and right keys.
 It is possible to limit navigation to forward step only.

 Menu items may be actions - you may wish to draw special screens and allow for use-case
 driven inputs.  In this manner, you can trigger callbacks when menu items are triggered
 and disable the menu easily while interacting with the user, and then seamlessly place
 the user back into the menu where they left off when they are done.

 Finally, the Menu Manager can be used to easily poll the user inputs without drawing the
 menu as needed by the developer.

 @section menuitembasic The Basics of Menu Resources

 To create menu resources, you'll need to collect all textual labels and declare them using special
 datatypes which will keep them in program memory until needed.
 All textual labels resources stored in Resources::resources array.
 It is also depends on LCD_WIDTH and NUMBER_OF_STRINGS constants.
 For example:
 @code
 const char Resources::resources [NUMBER_OF_STRINGS][LCD_WIDTH+1] PROGMEM = {
  //1234567890123456
  {"  Stopped..     "},//0
  {" <OK> for Menu  "},//1
  {" Running [%d/%d]"},//2
  {"  More axis?    "},//3
        ....
  { ... }
 };
@endcode

 Next, the menu is defined via the MenuScreen data type, and specifies the (optional) label
 to give to the sub-menu, menu items (up to MAX_SCREEN_SEL).
 Sub-menu items ties together label for item, process for item, next item and the target parameter
 of the item.  For example:

 @code
 ...
   {
		 4,							//4 items
		 1,							//arrows required
		 MENU_LEVEL_ENTRY,          //type
		 0,                         //no dynamic content
		 {
		    {24, 0, 0, 0},	//item 1 (label)
		    {18, 0, MenuOrder::CREATE_FILM_WIZARD, 0},	//item 2
		    {20, 0, MenuOrder::RESCAN_DEVICES, 0},	//item 3
		    {19, 0, MenuOrder::SETTINGS_SCREEN, 0},	//item 4
		  }
		},
		...
  @endcode

 There are four types of sub-menus:
 <ul>
    <li>DIALOG_LEVEL_ENTRY</li>
    <ul>
        <li>Used to display some content, usually provided dynamically</li>
    </ul>
    <li>MENU_LEVEL_ENTRY</li>
    <ul>
        <li>>A sub-menu with label to be displayed, parameter value field should be 0</li>
    </ul>
    <li>PAGES_LEVEL_ENTRY</li>
    <ul>
        <li>Same as previous, but dynamic content is allowed</li>
    </ul>
    <li>WIZARD_LEVEL_ENTRY</li>
    <ul>
        <li>>Almost same, no label and allows to control navigation in wizard-like style</li>
    </ul>
   </ul>

 To define a value parameter, we use index which tells the menu system
 how to process the item. All values stored as 32-bit variables.

 @code
class MenuContext {
    //
	int32_t iParamValue[NUMBER_OF_PARAMETERS]; //parameters value array to edit
	//
	const static int32_t iParamMaxValue[NUMBER_OF_PARAMETERS];
	const static int16_t iParamMinValue[NUMBER_OF_PARAMETERS];
	const static uint16_t iStepParameters[NUMBER_OF_PARAMETERS];

 @endcode

 Here, we see that we can specify a maximum value, a minimum
 value, and the step size used when the parameter is about to be edited.
 Maximum value is limited to the range of a signed long,
 while Minimum is limited to signed int.

 @section menulists The Basics of creating Lists
 To create a list of labels in menu we have to fill some container which represents the list.
 For this, again, we have a special data type that allows us to easily create them and support
 navigation for fixed list type:

 @code
    struct fixListEntry {
      const char caption [LCD_WIDTH];
      uint8_t jumpLevel; // sub-menu index to jump to
      uint8_t jumpItem;  // item index within sub-menu
    };
 .......
    //dynamic list variables
	uint8_t dynListNode[DYN_LIST_SIZE]; //store bus_addr
	char dynList[DYN_LIST_SIZE][LCD_WIDTH + 1]; //store display name
	uint8_t dynListSize; //list size

	const static fixListEntry fixedList1[3];//None,Camera,Bulb
    const static fixListEntry fixedList2[2];//Begin,StartOver
	const static fixListEntry fixedList3[2];//Yes,No
	const static char fixedList4[8][3];//
	const static char fixedList5[2][17];//
 @endcode

 Current selection in list stored in parameter mapped by the same structure as for non-list entries.
 Fixed list contains pre-defined labels and allows navigation.
 Dynamic list filled each time before use by the means of some function.
 So one space for all dynamic lists is enough.
 We may, of course, create more fixed lists and use the same list in multiple items as well.



 @section menuaction The Basics of Creating Actions

 An Action screen is a special mode of MenuEngine where user takes responsibility for all duties.
 This can be used to display special screens, or call some other behavior more complex than just
 setting a parameter.

 Four functions are important openAction, closeAction, Left and Right.

 @code
  class ActionScreen {

	uint8_t moveDir;
	uint8_t markerPos;
	Commander& cmd;
	MenuContext& status;

public:
	ActionScreen(Commander & _cmd);
	void openAction(const uint8_t idxProc, const uint8_t idxParam);
	void closeAction(bool closeFlag);
	void Left(void);
	void Right(void);

	.....

 @endcode

 Commander is a helper class containing functions to be called from ActionScreen context.

 @section menuinit Initializing the Menu

 After we have created all of our sub-menu, lists, selects, and so forth, we need to
 give names to sub-menu and order of sub-menus in array.
 There are several mandatory entries. Do not edit or move those.
 @code
 // enumeration with screen numbers in array
   enum MenuOrder {
	INITIAL_DISPLAY_SCREEN, //* mandatory
	MENU_SCREEN,            //* user defined
	CREATE_FILM_WIZARD,     //* user defined
	RESCAN_DEVICES,         //* user defined
	SETTINGS_SCREEN,        //* user defined
	AXIS_WIZARD,            //* user defined
	MAX_SCREENS,            //* mandatory, end of real entries
    OPEN_EDIT,              //* mandatory, fake entry: opens parameter edit
	OPEN_ACTION             //* mandatory
  };
 @endcode

 @section menuinput User Input

 The button definition is always one of the following:

 BUTTON_FORWARD, BUTTON_BACK, BUTTON_INCREASE, BUTTON_DECREASE, BUTTON_SELECT

 The how to read button instruction will differ based on whether you are doing analog
 or digital input.  For digital inputs, the first element will be the Arduino digital
 pin # of the button, and for analog inputs, the first element will be analogRead()
 value associated with the button.

 For digital input, we simply need to pass digital pin numbers to PbtnInput constructor.
 Also note, that for digital inputs the input pins will be pulled HIGH internally
 and you are expected to swing them to LOW when pressed.

 For analog input, we must specify the pin to read the value from, and the threshold
 for reading the values.  The threshold is very important, as analog inputs are rarely exact;
 this threshold is the variance that will be applied to each value during comparison.

 @code
 #define BUT0_VAL  70
 #define BUT1_VAL  250
 #define BUT2_VAL  450
 #define BUT3_VAL  655
 #define BUT4_VAL  830

  // button variance range
 #define BUT_THRESH  60
 AnlgBtnInput(uint8_t a_pin);
 or
 PbtnInput(uint8_t up, uint8_t down, uint8_t enter, uint8_t menu);
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
  void ProcessUI(void);
 @endcode

*/
/**
 * Main class of menu library.
 * method processUI() should be called in endless loop.
 * Provides finite state machine for menu system.
 * Interaction with outer system via MenuContext parameters.
 * Notification update is not implemented yet.
 * */
class MenuEngine: public HorizMenu
{
	//time stamps to calculate time
	uint32_t mSTimer0;
	uint32_t mSTimer0_Freeze;

	uint8_t iEnginePhase; //former iPhase
	uint8_t iInterPhase;
	uint8_t cActivity;

	uint8_t _led;

	uint8_t mode;

	struct StackType {
		unsigned char cStackPointer;
		unsigned char cScreenCreatedToGetHere[MAX_SCREEN_SEL];
		unsigned char cTopLine[MAX_SCREEN_SEL];
		unsigned char cPointerLine[MAX_SCREEN_SEL];
		unsigned int iCallingInterPhase[MAX_SCREEN_SEL];
		};

	StackType ProcessStack;

protected:

    AnlgBtnInput keyboard;

public:
	MenuEngine(LiquidCrystal& _lcd, Commander& _cmd);

	void ProcessUI(void);

	//async functions

protected:
	void InitialiseDisplay(void) {ResetDisplay();}
    //
	unsigned char mSecTimeup (void *data1,void *data2);
	void mSecInitialise(void *data1, void *data2, unsigned long timerValue);
	//
	void PopStack (void);
	void FlattenStack (void);
	void PushStack (void);

	void OverallProcess(void);
	uint8_t Keyboard(void);
	void DisplayInterfaceManager(void);
	void TimeoutNoKeyEntry (void);

	//
	void DisplaySelectedItem(void);
	void NavigateNext(void);

	//
	uint8_t ChangeMode();
};

#endif /* MENUENGINE_H_ */
