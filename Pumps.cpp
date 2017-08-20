/*
*  DROMatic.ino
*  DROMatic OS Pumps
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Pumps.h"
#include "Core.h"
#include "Regimens.h"
#include "Screens.h"
#include "Crops.h"

byte currentPumpIndex;

//Read & Write from SD
JsonObject& getPumpsData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/pump.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setPumpsData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/pump.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}
JsonObject& getPumpData(JsonBuffer& b, byte pumpIndex = currentPumpIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/pumps/syspmp" + pumpIndex + "/pump.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setPumpData(JsonObject& d, byte pumpIndex = currentPumpIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/pumps/syspmp" + pumpIndex + "/pump.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		printHomeScreen();
	}
}

//Prints
void printPumpCalibration(int dir = 0){
	lcd.clear();
	String displaySize;
	displaySize = F("00");
	if (dir != 0){
		tmpInts[0] += (dir * 10);
		if (tmpInts[0] < 10) { tmpInts[0] = 10; }
		if (tmpInts[0] > 900) { tmpInts[0] = 900; }
	}
	displaySize = (tmpInts[0] >= 100)? F("") : (tmpInts[0] >= 10)? F("0") :  F("");

	lcd.home();
	lcd.print(displaySize + tmpInts[0] + F("(ml) per min"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);
}
void printPumpDelay(int dir = 0){
	if (dir != 0){
		tmpInts[0] += dir;
		if (tmpInts[0] <= 0){
			tmpInts[0] = 1;
		}
		if (tmpInts[0] >= 100){
			tmpInts[0] = 100;
		}
	}
	lcd.clear();
	if (tmpInts[0] >= 10){
		lcd.print("0");
	}else{
		lcd.print("00");
	}
	lcd.print(tmpInts[0]);
	lcd.print(F(" Min(s) Delay"));
	lcd.setCursor(0, 2);
	lcd.print(F("<back>      <ok>"));
	cursorX = 2;
	cursorY = 0;
	lcd.setCursor(cursorX, cursorY);
}

//Saves
void savePumpCalibration(){
	if (cursorX == 13 && cursorY == 1){ //single pump save
		StaticJsonBuffer<pumpBufferSize> buffer;
		JsonObject& data = getPumpsData(buffer);
		data["calibration"] = pumpCalibration = tmpInts[0];
		setPumpsData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}
void savePumpDelay(){
	if (cursorX == 13 && cursorY == 1){ //single pump save
		StaticJsonBuffer<pumpBufferSize> buffer;
		JsonObject& data = getPumpsData(buffer);
		data["delay"] = pumpDelay = tmpInts[0];
		setPumpsData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}

//Helpers
void pumpCreate(String path, byte pumpIndex, int totalRegimens, JsonObject& regimenData){
	SD.mkdir(path);
	SD.mkdir(path + "/Prime");
	if (pumpIndex <= 7){
		SD.mkdir(path + "/Amount");
		for (byte j = 1; j <= totalRegimens; j++){
			makeNewFile(path + "/pmpse" + j + ".dro", regimenData);
			Serial.flush();
		}
	}
	Serial.flush();
}

void primePump(int dir){
	RelayToggle(currentPumpIndex, true);
	delay(1000);
	RelayToggle(currentPumpIndex, false);
}

void pumpSpin(float setAmount, int pumpNumber){
	//Cast a float to avoid implicit int rounding
	float mlPerSec = ((float)pumpCalibration) / 60; //100ml per min / 60 seconds = 1.6ml per second
	//Now it is ok to rounding to whole number
	int pumpTimeLength = (setAmount / mlPerSec); //amount / mlPerSec = total ml time in seconds
	while (pumpTimeLength--){
		RelayToggle(pumpNumber, true); //keep pump turning
		delay(1000);
	}
	RelayToggle(pumpNumber, false); //turn pump off
}