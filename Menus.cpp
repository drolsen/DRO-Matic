/*
*  DROMatic.ino
*  DROMatic OS Menus
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Menus.h"
#include "Core.h"
#include "Screens.h"
#include "DatesTime.h"

byte menuIndex;
vector<String> menus, menusHistory;

String getMenuHistory(){
	byte i, size;
	String join;
	size = menusHistory.size();
	for (i = 0; i < size; i++){
		join = join + "/" + menusHistory.at(i);
	}
	return join;
}

void scrollAlpha(int dir){
	currentAlphaIndex = currentAlphaIndex + dir;
	if (currentAlphaIndex > 36){
		currentAlphaIndex = 0;
	}
	else if (currentAlphaIndex < 0){
		currentAlphaIndex = 36;
	}
	char alphaBuffer[2];
	lcd.print(strcpy_P(alphaBuffer, (char*)pgm_read_word(&(alphabet[currentAlphaIndex]))));
	lcd.setCursor(cursorX, cursorY);
	delay(50);
}

void scrollMenus(int dir){
	menuIndex = menuIndex + dir;
	int currentSize = menus.size() - 1;
	if (menuIndex < 0){
		menuIndex = currentSize;
	}
	else if (menuIndex > currentSize){
		menuIndex = 0;
	}
	lcd.clear();
	printScreenNames(menus[menuIndex]);
	printScrollArrows();
}

void getDirectoryMenus(File directory) {
	menus.clear();
	while (true) {
		tmpFile = directory.openNextFile();
		if (!tmpFile) { // no more files
			break;
		}
		if (tmpFile.isDirectory()){
			menus.push_back(tmpFile.name());
		}
		tmpFile.close();
	}
}
