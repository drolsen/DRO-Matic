/*
*  DROMatic.ino
*  DROMatic OS Screens
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Screens.h"
#include "Core.h"
#include "Menus.h"
#include "DatesTime.h"
#include "Crops.h"
#include "Regimens.h"
#include "Timers.h"

String screenName;
vector<vector<vector<byte>>> matrix;
byte cursorX, cursorY;
byte currentAlphaIndex = 0;

byte upArrow[8] = {
	B00000,
	B00100,
	B01110,
	B11111,
	B00100,
	B00100,
	B00100,
	B00000
};

byte downArrow[8] = {
	B00000,
	B00100,
	B00100,
	B00100,
	B11111,
	B01110,
	B00100,
	B00000
};

void exitScreen(){
	cursorX = cursorY = 0;
	menusHistory.pop_back();
	menuIndex = 0;
	screenName = F("");
	tmpFile = SD.open("dromatic/" + cropName + "/" + getMenuHistory());
	getDirectoryMenus(tmpFile);
	lcd.clear();
	lcd.noBlink();
	tmpFile.close();
	printScreenNames(menus.front());
	printScrollArrows();
	delay(350);
}

void printHomeScreen(){
	captureDateTime();
	char monthsBuffer[8];
	
	int EC1Value = getECProbeValue(PLANTEC);
	float PH1Value = getPHProbeValue(PLANTPH);

	int RED = (PH1Value > maxPH) ? 255 : 0;
	int GREEN = (PH1Value >= minPH && PH1Value <= maxPH) ? 255 : 0;
	int BLUE = (PH1Value < minPH) ? 255 : 0;
	lcd.clear();

	//hour					//minute		  //AM/PM											//Month														//Day
	lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[rtc.getTime().mon-1]))) + F(" ") + tmpDisplay[1]);
	lcd.setCursor(0, 1);
	lcd.print(F("PPM:"));
	lcd.print(EC1Value);
	lcd.print(F(" PH:"));
	lcd.print(PH1Value);
	lcd.home();
	lcd.noBlink();

	byte i = NUMOFLEDS;
	while (i--){
		// pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
		pixels.setPixelColor(i, pixels.Color(RED, GREEN, BLUE)); // Moderately bright green color.
		pixels.show(); // This sends the updated pixel color to the hardware.
	}
}

void screenMatrix(){
	byte row0Size = matrix[0].size();
	byte row1Size = matrix[1].size();

	byte frontFrontFront = matrix.front().front().front();
	byte frontBackBack = matrix.front().back().back();

	byte backBackBack = matrix.back().back().back();
	byte backFrontFront = matrix.back().front().front();
	//Check Top Row
	if (cursorY == 0){
		if (cursorX < frontFrontFront){
			cursorX = backBackBack;
			cursorY = 1;
		}
		else if (cursorX > frontBackBack){
			cursorY = 1;
			cursorX = backFrontFront;
		}
		for (int i = 0; i < row0Size; i++){ //top columns
			if (cursorX < matrix.front()[i].front() && cursorX > matrix.front()[i - 1].back() && Key == 408 && cursorY == 0){
				cursorX = matrix.front()[i - 1].back();
			}
			if (cursorX > matrix.front()[i].back() && cursorX < matrix.front()[i + 1].front() && Key == 0 && cursorY == 0){
				cursorX = matrix.front()[i + 1].front();
			}
		}
	}
	else if (cursorY == 1){
		if (cursorX < backFrontFront){
			cursorX = frontBackBack;
			cursorY = 0;
		}
		else if (cursorX > backBackBack){
			cursorY = 0;
			cursorX = frontFrontFront;
		}
		for (int i = 0; i < row1Size; i++){ //top columns
			if (cursorX < matrix.back()[i].front() && cursorX > matrix.back()[i - 1].back() && Key == 408 && cursorY == 1){
				cursorX = matrix.back()[i - 1].back();
			}
			if (cursorX > matrix.back()[i].back() && cursorX < matrix.back()[i + 1].front() && Key == 0 && cursorY == 1){
				cursorX = matrix.back()[i + 1].front();
			}
		}
	}
	lcd.setCursor(cursorX, cursorY);
}

void printScrollArrows(){
	lcd.setCursor(15, 0);
	lcd.write(byte(0));
	lcd.setCursor(15, 15);
	lcd.write(byte(1));
	lcd.home();
}

void printScreenNames(String menu){
	lcd.home();
	bool hasMatch = false;
	const byte isPump = strstr(menu.c_str(), "SYSPMP") != NULL; //Pumps
	const byte isTimer = strstr(menu.c_str(), "RECEP") != NULL; //Timers
	if (isPump){
		lcd.print(F("SYSTEM"));
		lcd.setCursor(0, 1);
		lcd.print(F("PUMP "));
		lcd.print(String(menuIndex + 1));
		currentPumpIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	} else if (isTimer){
		lcd.print(F("TIMED"));
		lcd.setCursor(0, 1);
		lcd.print(F("RECEPTACLE "));
		lcd.print(String(menuIndex + 1));
		currentTimerIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}
	else {
		for (byte i = 0; i < menusBufferSize; i++){
			char match1Buffer[18];
			char match2Buffer[18];
			char match3Buffer[18];

			if (menu == strcpy_P(match1Buffer, (char*)pgm_read_word(&(screenNames[i][0])))){
				hasMatch = true;
				lcd.print(strcpy_P(match2Buffer, (char*)pgm_read_word(&(screenNames[i][1]))));
				lcd.setCursor(0, 1);
				lcd.print(strcpy_P(match2Buffer, (char*)pgm_read_word(&(screenNames[i][2]))));
				break;
			}
		}
	}
	if (hasMatch == false){
		lcd.print(menu);
	}
}
