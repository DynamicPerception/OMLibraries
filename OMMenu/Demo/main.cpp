#include <inttypes.h>
#include "Arduino.h"
#include "LiquidCrystal.h"
#include "MenuEngineMock.h"
#include "BusMaster.h"

#define LCD_RS 17
#define LCD_EN 18
#define LCD_D4 11
#define LCD_D5 9
#define LCD_D6 7
#define LCD_D7 4

#define DIR_PIN 10

void test1(MenuEngineMock& eng);
void binOnLcd(uint8_t key, LiquidCrystal& lcd);
void testKbd(MenuEngineMock& eng, LiquidCrystal& lcd);
void lowLevelKbd(MenuEngineMock& eng, LiquidCrystal& lcd);

int main() {
	//init Arduino core lib;
	init();

	BusMaster bm(Serial, DIR_PIN);
	// initialize the library with the numbers of the interface pins
	LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
	//lcd.print("hello1");
	MenuEngineMock eng(lcd, bm);

//	lcd.print((char)0);
//	lcd.print((char)1);
//	lcd.print((char)2);
//	lcd.print((char)3);
//	lcd.print((char)4);
//	lcd.print((char)5);
//	lcd.print((char)6);
//	lcd.print((char)7);


	while (1) {
		eng.ProcessUI();
		//testKbd(eng, lcd);
		//lowLevelKbd( eng,  lcd);
	}

    //test1(eng);


	return 0;
}

//-----------------------------------------
//
//
void test1(MenuEngineMock& eng) {
	uint8_t a[21];
	eng.TransferMsg(0,(void*)&a[0]);
    eng.WriteLine(a,0,0);
    eng.TransferMsg(1,(void*)&a[0]);
	eng.WriteLine(a,1,0);
	eng.TransferMsg(2,(void*)&a[0]);
	eng.WriteLine(a,2,0);
	eng.TransferMsg(3,(void*)&a[0]);
	eng.WriteLine(a,3,0);
}

//----------------------------------------
//
//
void test2 (MenuEngineMock& eng) {
	 eng.CreateDisplay(0);
	    eng.TransferDisplay();
	    eng.UpdateDisplay();
	    eng.UpdateDisplay();
	    eng.UpdateDisplay();
	    eng.UpdateDisplay();
	    eng.UpdateDisplay();
}


//-------------------------------------------------
//
//
void lowLevelKbd(MenuEngineMock& eng, LiquidCrystal& lcd) {
	delay(20);
	eng.Keyboard();
	int val = eng.getKeyboardReads();
	lcd.print(val);
	lcd.println();
}



//------------------------------------------------
//
//
void testKbd(MenuEngineMock& eng, LiquidCrystal& lcd) {
	delay (20);
	eng.Keyboard();
	MenuContext& cont = eng.getContext();
	//uint8_t key = 0x83;
	if ((cont.getContext() & KEYBOARD_VALID)) {
		binOnLcd(cont.getKeyboardCode(), lcd);
		cont.clrContextBits(KEYBOARD_VALID);
	}
}



void binOnLcd(uint8_t key, LiquidCrystal& lcd) {
	char ch[9];
	//uint8_t key = 0x83;
	if (bitRead(key, 7)) {
		ch[0] = '*';
	} else {
		ch[0] = '-';

	}
	if (bitRead(key, 6)) {
		ch[1] = '*';
	} else {
		ch[1] = '-';

	}
	if (bitRead(key, 5)) {
		ch[2] = '*';
	} else {
		ch[2] = '-';

	}
	if (bitRead(key, 4)) {
		ch[3] = '*';
	} else {
		ch[3] = '-';
	}
	if (bitRead(key, 3)) {
		ch[4] = '*';
	} else {
		ch[4] = '-';
	}
	if (bitRead(key, 2)) {
		ch[5] = '*';
	} else {
		ch[5] = '-';
	}
	if (bitRead(key, 1)) {
		ch[6] = '*';
	} else {
		ch[6] = '-';
	}
	if (bitRead(key, 0)) {
		ch[7] = '*';
	} else {
		ch[7] = '-';
	}
	ch[8] = 0;
	lcd.setCursor(0, 0);
	lcd.write(ch);
}
