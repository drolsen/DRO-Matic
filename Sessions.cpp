/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Sessions.h"
#include "Core.h"
#include "Screens.h"
#include "Crops.h"
#include "Channels.h"

byte currentSessionIndex;

JsonObject& getSessionData(JsonBuffer& b, byte channelIndex = currentChannelIndex, byte sessionIndex = currentSessionIndex){
	tmpFile = SD.open("DROMATIC/" + cropName + "/CHS/SYSCH" + channelIndex + "/CHSE" + sessionIndex + ".DRO", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setSessionData(JsonObject& d, byte channelIndex = currentChannelIndex, byte sessionIndex = currentSessionIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("DROMATIC/" + cropName + "/CHS/SYSCH" + channelIndex + "/CHSE" + sessionIndex + ".DRO", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		printHomeScreen();
	}
}

void setSessionNumber(int dir){
	char instructions[15] = " REGIMEN WEEKS";
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	lcd.clear();
	if (tmpInts[0] < 10){ lcd.print(0); }

	lcd.print(tmpInts[0]);
	lcd.print(instructions);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}

void setSessionAmount(int dir){
	if (cursorX == 12){
		lcd.clear();
		String amountDisplay = F("0000.0");
		if (dir > 0){
			tmpFloats[0] += .1;
		}else{
			tmpFloats[0] -= .1;
		}

		if (tmpFloats[0] < 0){
			tmpFloats[0] = 0;
		}

		if (tmpFloats[0] > 100){
			amountDisplay = F("0");
		}
		else if (tmpFloats[0] > 10){
			amountDisplay = F("00");
		} else{
			amountDisplay = F("000");
		}

		lcd.print(F("REGI "));
		lcd.print(tmpInts[0]);
		lcd.print(F(" "));
		lcd.print(amountDisplay);
		lcd.print(String(tmpFloats[0]));
		lcd.print(F("ml"));
		lcd.setCursor(0, 1);
		if (tmpInts[0] == 1){
			lcd.print(F("<back>    <next>"));
		}
		else if (tmpInts[0] < tmpInts[1]){
			lcd.print(F("<prev>    <next>"));
		}
		else{
			lcd.print(F("<prev>    <done>"));
		}
		lcd.setCursor(cursorX, cursorY);
	}
}

void makeSession(String path, JsonObject& data, int index){
	char buffer[64];
	tmpFile = SD.open(path + "/chse" + index + ".dro", FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}

void addSessions(int currentSize, int addAmount){
	byte i, j;
	String path;
	StaticJsonBuffer<regimenSessionBufferSize> buffer;
	JsonObject& data = buffer.createObject();
	data["ml"] = 80;
	data["expired"] = false;

	for (i = 1; i <= 8; i++){
		path = "dromatic/" + cropName + "/chs/sysch" + String(i);
		for (j = 0; j < addAmount; j++){
			if (j > currentSize){
				makeSession(path, data, j);
			}
			Serial.flush();
		}
	}
}

void trimSessions(int currentSize, int trimAmount){
	byte i, j;
	String path;
	i = 8; //number of channels
	for (i = 1; i <= 8; i++){
		path = "dromatic/" + cropName + "/chs/sysch" + String(i);
		for (j = 0; j <= currentSize; j++){
			if (j > trimAmount-1){
				SD.remove(path + "/chse" + String(j) + ".dro");
			}
			Serial.flush();
		}
	}
}