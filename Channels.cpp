/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Channels.h"
#include "Core.h"
#include "Sessions.h"
#include "Screens.h"
#include "Crops.h"

byte currentChannelIndex;

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

void makeChannel(String path, int totalSessions, JsonObject& sessionData){
	byte j;
	SD.mkdir(path);
	SD.mkdir(path + "/Prime");
	SD.mkdir(path + "/Amount");
	SD.mkdir(path + "/Calib");
	SD.mkdir(path + "/Delay");

	for (j = 0; j < totalSessions; j++){
		makeSession(path, sessionData, j);
		Serial.flush();
	}
	Serial.flush();
}

void setCalibrationSize(int dir){
	lcd.clear();
	String displaySize;
	displaySize = F("00");

	tmpInts[0] = tmpInts[0] + dir;

	if (tmpInts[0] < 0) { tmpInts[0] = 0; }
	if (tmpInts[0] > 5) { tmpInts[0] = 5; }
	displaySize = String(tmpInts[0]) + displaySize;

	lcd.home();
	lcd.print(displaySize + F("(ml) per min"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>  <ok|all>"));
	lcd.setCursor(cursorX, cursorY);
}

void primeChannelPump(int dir){
	RelayToggle(currentChannelIndex, true);
	delay(1000);
	RelayToggle(currentChannelIndex, false);
}