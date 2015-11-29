/**  Example OMMenuMgr Sketch

 @author
 C. A. Church

 */

#include "OMMenuMgr.h"
#include <LiquidCrystal.h>


 // ui setup
 
 // ui setup

    // lcd pins
const byte LCD_RS  = 8;
const byte LCD_EN  = 9;
const byte LCD_D4  = 4;
const byte LCD_D5  = 5;
const byte LCD_D6  = 6;
const byte LCD_D7  = 7;


const byte LCD_ROWS = 2;
const byte LCD_COLS = 16;

 // button values

 // which input is our button
const byte BUT_PIN = 14;

  // analog button read values
const int BUTFWD_VAL  = 13;
const int BUTINC_VAL  = 122;
const int BUTDEC_VAL  = 284;
const int BUTREV_VAL  = 439;
const int BUTSEL_VAL  = 650;

const byte BUT_THRESH  = 60;

  // mapping of analog button values for menu
int BUT_MAP[5][2] = {
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
MENU_ITEM item_barme    = { {"UInt Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&bar_value) };
MENU_ITEM item_bazme    = { {"Long Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&baz_value) };
MENU_ITEM item_bakme    = { {"Float Edit"},   ITEM_VALUE,  0,        MENU_TARGET(&bak_value) };
MENU_ITEM item_state    = { {"Select Input"}, ITEM_VALUE,  0,        MENU_TARGET(&sel_value) };
MENU_ITEM item_testme   = { {"Test Action"},  ITEM_ACTION, 0,        MENU_TARGET(uiQwkScreen) };
MENU_LIST param_list[]  = { &item_checkme, &item_barme, &item_bazme, &item_bakme, &item_state };
MENU_ITEM menu_param    = { {"Parameters"},   ITEM_MENU,   MENU_SIZE(param_list),   MENU_TARGET(&param_list) };

// Analog pin menu
MENU_ITEM item_a0       = { {"Analog 0"},     ITEM_ACTION,  0,        MENU_TARGET(analog0) };
MENU_ITEM item_a1       = { {"Analog 1"},     ITEM_ACTION,  0,        MENU_TARGET(analog1) };
MENU_ITEM item_a2       = { {"Analog 2"},     ITEM_ACTION,  0,        MENU_TARGET(analog2) };
MENU_ITEM item_a3       = { {"Analog 3"},     ITEM_ACTION,  0,        MENU_TARGET(analog3) };
MENU_ITEM item_a4       = { {"Analog 4"},     ITEM_ACTION,  0,        MENU_TARGET(analog4) };
MENU_ITEM item_a5       = { {"Analog 5"},     ITEM_ACTION,  0,        MENU_TARGET(analog5) };
MENU_LIST analog_list[] = { &item_a0, &item_a1, &item_a2, &item_a3, &item_a4, &item_a5 };
MENU_ITEM menu_analog   = { {"Analog Read"},  ITEM_MENU,   MENU_SIZE(analog_list),   MENU_TARGET(&analog_list) };

// Digital pin menu (comment out your LCD pins unless you want a real adventure!)
MENU_ITEM item_d0       = { {"Digital 0"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR0) };
MENU_ITEM item_d1       = { {"Digital 1"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR1) };
MENU_ITEM item_d2       = { {"Digital 2"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR2) };
MENU_ITEM item_d3       = { {"Digital 3"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR3) };
//MENU_ITEM item_d4       = { {"Digital 4"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR4) };
//MENU_ITEM item_d5       = { {"Digital 5"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR5) };
//MENU_ITEM item_d6       = { {"Digital 6"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR6) };
//MENU_ITEM item_d7       = { {"Digital 7"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR7) };
//MENU_ITEM item_d8       = { {"Digital 8"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR8) };
//MENU_ITEM item_d9       = { {"Digital 9"},    ITEM_ACTION,  0,        MENU_TARGET(digitalR9) };
MENU_ITEM item_d10      = { {"Digital 10"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR10) };
MENU_ITEM item_d11      = { {"Digital 11"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR11) };
MENU_ITEM item_d12      = { {"Digital 12"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR12) };
MENU_ITEM item_d13      = { {"Digital 13"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR13) };
MENU_ITEM item_d14      = { {"Digital 14"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR14) };
MENU_ITEM item_d15      = { {"Digital 15"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR15) };
MENU_ITEM item_d16      = { {"Digital 16"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR16) };
MENU_ITEM item_d17      = { {"Digital 17"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR17) };
MENU_ITEM item_d18      = { {"Digital 18"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR18) };
MENU_ITEM item_d19      = { {"Digital 19"},   ITEM_ACTION,  0,        MENU_TARGET(digitalR19) };
MENU_LIST digital_list[]= { &item_d0, &item_d1, &item_d2, &item_d3, /*&item_d4, &item_d5, &item_d6, &item_d7, &item_d8, &item_d9,*/
    &item_d10, &item_d11, &item_d12, &item_d13, &item_d14, &item_d15, &item_d16, &item_d17, &item_d18, &item_d19 };
MENU_ITEM menu_digital  = { {"Digital Read"}, ITEM_MENU,   MENU_SIZE(digital_list),   MENU_TARGET(&digital_list) };

// Digital write pin menu (comment out your LCD pins unless you want a real adventure!)
MENU_ITEM item_d0w      = { {"Digital 0"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW0) };
MENU_ITEM item_d1w      = { {"Digital 1"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW1) };
MENU_ITEM item_d2w      = { {"Digital 2"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW2) };
MENU_ITEM item_d3w      = { {"Digital 3"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW3) };
//MENU_ITEM item_d4w      = { {"Digital 4"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW4) };
//MENU_ITEM item_d5w      = { {"Digital 5"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW5) };
//MENU_ITEM item_d6w      = { {"Digital 6"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW6) };
//MENU_ITEM item_d7w      = { {"Digital 7"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW7) };
//MENU_ITEM item_d8w      = { {"Digital 8"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW8) };
//MENU_ITEM item_d9w      = { {"Digital 9"},    ITEM_ACTION,  0,        MENU_TARGET(digitalW9) };
MENU_ITEM item_d10w     = { {"Digital 10"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW10) };
MENU_ITEM item_d11w     = { {"Digital 11"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW11) };
MENU_ITEM item_d12w     = { {"Digital 12"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW12) };
MENU_ITEM item_d13w     = { {"Digital 13"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW13) };
MENU_ITEM item_d14w     = { {"Digital 14"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW14) };
MENU_ITEM item_d15w     = { {"Digital 15"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW15) };
MENU_ITEM item_d16w     = { {"Digital 16"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW16) };
MENU_ITEM item_d17w     = { {"Digital 17"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW17) };
MENU_ITEM item_d18w     = { {"Digital 18"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW18) };
MENU_ITEM item_d19w     = { {"Digital 19"},   ITEM_ACTION,  0,        MENU_TARGET(digitalW19) };
MENU_LIST digitalW_list[]= { &item_d0w, &item_d1w, &item_d2w, &item_d3w, /*&item_d4w, &item_d5w, &item_d6w, &item_d7w, &item_d8w, &item_d9w,*/
    &item_d10w, &item_d11w, &item_d12w, &item_d13w, &item_d14w, &item_d15w, &item_d16w, &item_d17w, &item_d18w, &item_d19w };
MENU_ITEM menu_digitalW = { {"Digital Write"},ITEM_MENU,   MENU_SIZE(digitalW_list),  MENU_TARGET(&digitalW_list) };

// Root menu
MENU_LIST root_list[]   = { &menu_param, &item_testme, &menu_analog, &menu_digital };
MENU_ITEM menu_root     = { {"Root"},         ITEM_MENU,   MENU_SIZE(root_list),    MENU_TARGET(&root_list) };


 // initialize LCD object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

OMMenuMgr Menu(&menu_root);



void setup() {

  lcd.begin(LCD_COLS, LCD_ROWS);

  uiClear();

  Menu.setDrawHandler(uiDraw);
  Menu.setExitHandler(uiClear);
  Menu.setAnalogButtonPin(BUT_PIN, BUT_MAP, BUT_THRESH);
  Menu.enable(true);
  
  
}

void loop() {
 Menu.checkInput();
 
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
  lcd.print("Enter to return");

  while( Menu.checkInput() != BUTTON_SELECT ) {
    ; // wait!
  }

  Menu.enable(true);
  lcd.clear();
}

void analog0() { analog(0);}
void analog1() { analog(1);}
void analog2() { analog(2);}
void analog3() { analog(3);}
void analog4() { analog(4);}
void analog5() { analog(5);}

void analog( uint8_t pin) {
  unsigned long sampleTime;
  unsigned long displayTime;
  unsigned long timeNow;
  int sample;
  int average;
  char s[10];

  lcd.clear();
  Menu.enable(false);

  lcd.setCursor(0, 0);
  lcd.print("Analog ");
  lcd.print(pin);
  lcd.print(": ");

  lcd.setCursor(0, 1);
  lcd.print("Enter to return");
  sampleTime = millis() + 20;    //20ms samples (over sample)
  displayTime = millis() + 200;  //200ms display update
  sample = analogRead(pin);
  average = sample;
  while( Menu.checkInput() != BUTTON_SELECT) {
    timeNow = millis();

    if ((int)(displayTime - timeNow) < 0) {    //less than comparison with overflow
      displayTime += 200;
      if(timeNow > displayTime) {
        //Missed a display update
        displayTime = timeNow + 200;
      }
      sprintf(s, "%4d", average);
      lcd.setCursor(15-strlen(s),0);
      lcd.print(s);
    }

    if ((int)(sampleTime - timeNow) < 0) {    //less than comparison with overflow
      sample = analogRead(pin);
      sampleTime += 20;
      if(timeNow > sampleTime) {
        //Missed a sample period
        sampleTime = millis() + 20;
      }
      //Average using IIR low pass filter
      //Use powers of 2 for speed and size (range 1-infinity)
      average = average + (sample - average) / 2;
    }
  }

  Menu.enable(true);
  lcd.clear();
}

void digitalR0() { digitalR(0);}
void digitalR1() { digitalR(1);}
void digitalR2() { digitalR(2);}
void digitalR3() { digitalR(3);}
void digitalR4() { digitalR(4);}
void digitalR5() { digitalR(5);}
void digitalR6() { digitalR(6);}
void digitalR7() { digitalR(7);}
void digitalR8() { digitalR(8);}
void digitalR9() { digitalR(9);}
void digitalR10() { digitalR(10);}
void digitalR11() { digitalR(11);}
void digitalR12() { digitalR(12);}
void digitalR13() { digitalR(13);}
void digitalR14() { digitalR(14);}
void digitalR15() { digitalR(15);}
void digitalR16() { digitalR(16);}
void digitalR17() { digitalR(17);}
void digitalR18() { digitalR(18);}
void digitalR19() { digitalR(19);}

void digitalR( uint8_t pin) {
  lcd.clear();
  Menu.enable(false);

  lcd.setCursor(0, 0);
  lcd.print("Digital ");
  lcd.print(pin);
  lcd.print(": ");

  lcd.setCursor(0, 1);
  lcd.print("Enter to return");

  pinMode(pin, INPUT);           // set pin to input
  digitalWrite(pin, LOW);        // turn off pullup resistors

  while( Menu.checkInput() != BUTTON_SELECT) {
    lcd.setCursor(12,0);
    if(digitalRead(pin)) {
      lcd.print("HIGH");
    } else {
      lcd.print(" LOW");
    }
  }

  Menu.enable(true);
  lcd.clear();
}

void digitalW0() { digitalW(0);}
void digitalW1() { digitalW(1);}
void digitalW2() { digitalW(2);}
void digitalW3() { digitalW(3);}
void digitalW4() { digitalW(4);}
void digitalW5() { digitalW(5);}
void digitalW6() { digitalW(6);}
void digitalW7() { digitalW(7);}
void digitalW8() { digitalW(8);}
void digitalW9() { digitalW(9);}
void digitalW10() { digitalW(10);}
void digitalW11() { digitalW(11);}
void digitalW12() { digitalW(12);}
void digitalW13() { digitalW(13);}
void digitalW14() { digitalW(14);}
void digitalW15() { digitalW(15);}
void digitalW16() { digitalW(16);}
void digitalW17() { digitalW(17);}
void digitalW18() { digitalW(18);}
void digitalW19() { digitalW(19);}

void digitalW( uint8_t pin) {
  lcd.clear();
  Menu.enable(false);

  lcd.setCursor(0, 0);
  lcd.print("Digital ");
  lcd.print(pin);
  lcd.print(": ");

  lcd.setCursor(0, 1);
  lcd.print("Enter to return");

  pinMode(pin, OUTPUT);          // set pin to input

  while( Menu.checkInput() != BUTTON_SELECT) {
  lcd.setCursor(12,0);
    if(digitalRead(pin)) {
      lcd.print("HIGH");
    } else {
      lcd.print(" LOW");
    }
  }

  Menu.enable(true);
  lcd.clear();
}

