/*
*  DROMatic.ino
*  DROMatic OS Channels
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Channels.h"
#include "Core.h"
#include "Regimens.h"
#include "Screens.h"
#include "Crops.h"

byte currentChannelIndex;

//Read & Write from SD
JsonObject& getChannelsData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/channel.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setChannelsData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channel.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}
JsonObject& getChannelData(JsonBuffer& b, byte channelIndex = currentChannelIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setChannelData(JsonObject& d, byte channelIndex = currentChannelIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_WRITE | O_TRUNC);
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
	displaySize = (tmpInts[0] >= 100)? "" : (tmpInts[0] >= 10)? "0" :  "";

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
	}
	lcd.print(String(tmpInts[0] * 60000) + F(" Min(s) Delay"));
	lcd.setCursor(0, 2);
	lcd.print(F("<back>      <ok>"));
	cursorX = 2;
	cursorY = 0;
	lcd.setCursor(cursorX, cursorY);
}

//Saves
void savePumpCalibration(){
	if (cursorX == 13 && cursorY == 1){ //single channel save
		StaticJsonBuffer<channelBufferSize> buffer;
		JsonObject& data = getChannelsData(buffer);
		data["calibration"] = pumpCalibration = tmpInts[0];
		setChannelsData(data);
		cursorX = cursorY = 0;
	}
	if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}
void savePumpDelay(){
	if (cursorX == 13 && cursorY == 1){ //single channel save
		StaticJsonBuffer<channelBufferSize> buffer;
		JsonObject& data = getChannelsData(buffer);
		data["delay"] = pumpDelay = tmpInts[0];
		setChannelsData(data);
		cursorX = cursorY = 0;
	}
	if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}

//Helpers
void channelCreate(String path, int totalSessions, JsonObject& regimenData){
	byte j;
	SD.mkdir(path);
	SD.mkdir(path + "/Prime");
	SD.mkdir(path + "/Amount");
	SD.mkdir(path + "/Delay");

	for (j = 0; j < totalSessions; j++){
		makeNewFile(path + "/chse" + j + ".dro", regimenData);
		Serial.flush();
	}
	Serial.flush();
}
void primeChannelPump(int dir){
	RelayToggle(currentChannelIndex, true);
	delay(1000);
	RelayToggle(currentChannelIndex, false);
}