#include "Channels.h"
#include "Core.h"
#include "Sessions.h"
#include "Screens.h"

JsonObject& getChannelData(JsonBuffer& b, int channelIndex = currentChannelIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setChannelData(JsonObject& d, int channelIndex = currentChannelIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		openHomeScreen();
	}
}

void setPHChannels(int dir){
	String up, down;
	if (cursorX == 6 && cursorY == 0){
		tmpInts[0] = (tmpInts[0] + dir < 1) ? 1 : (tmpInts[0] + dir > 10) ? 10 : tmpInts[0] + dir;
	}
	if (cursorX == 14 && cursorY == 0){
		tmpInts[1] = (tmpInts[1] + dir < 1) ? 1 : (tmpInts[1] + dir > 10) ? 10 : tmpInts[1] + dir;
	}
	up = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	down = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);

	lcd.clear();
	lcd.home();
	lcd.print(F("PH"));
	lcd.write(byte(1));
	lcd.print(F("CH"));
	lcd.print(up);
	lcd.print(F(" PH"));
	lcd.write(byte(0));
	lcd.print(F("CH"));
	lcd.print(down);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void setChannelNumber(int dir){
	String totalDisplay;
	byte min, max;
	min = 2; max = 10;
	if (cursorX == 1 && cursorY == 0){
		//Min / max checking
		tmpInts[0] = (tmpInts[0] + dir > max) ? max : (tmpInts[0] + dir < min) ? min : tmpInts[0] + dir;
	}
	lcd.clear();
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	lcd.print(totalDisplay + F(" CHANNELS"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}

void makeChannel(String path, int numberOfSessions, JsonObject& channelData, JsonObject& sessionData){
	byte j;
	String channelName = path;
	SD.mkdir(channelName);
	SD.mkdir(channelName + "/ChConf");
	SD.mkdir(channelName + "/ChConf/ChDoses");
	SD.mkdir(channelName + "/ChConf/ChSize");
	SD.mkdir(channelName + "/ChConf/ChCalib");
	makeNewFile(channelName + "/channel.dro", channelData);

	for (j = 0; j < numberOfSessions; j++){
		sessionData["id"] = j;
		sessionData["outof"] = numberOfSessions;
		makeSession(channelName + "/Sessions/ChSes" + String(j + 1), sessionData);
		Serial.flush();
	}
	Serial.flush();
}

void removeChannel(String path) {
	byte i, sessionTotal;
	String loopedPath;
	StaticJsonBuffer<256> buffer;

	tmpFile = SD.open(path + "/channel.dro");
	JsonObject& data = buffer.parseObject(tmpFile.readString());
	tmpFile.close();

	sessionTotal = data["sessionsTotal"];
	SD.rmdir(path + "/ChConf/ChCalib");
	SD.rmdir(path + "/ChConf/ChDoses");
	SD.rmdir(path + "/ChConf/ChSize");
	SD.rmdir(path + "/ChConf");
	SD.remove(path + "/Channel.dro");

	for (i = 0; i < (sessionTotal + 1); i++){
		loopedPath = path + "/Sessions/ChSes" + String(i + 1);
		SD.rmdir(loopedPath + "/Amt");
		SD.rmdir(loopedPath + "/Dly");
		SD.rmdir(loopedPath + "/Rpt");
		SD.rmdir(loopedPath + "/Str");
		SD.remove(loopedPath + "/Session.dro");
		SD.rmdir(loopedPath);
	}

	SD.rmdir(path + "/Sessions");
	SD.rmdir(path);
}

void trimChannels(int currentSize, int trimAmount){
	byte length, i;
	String path = "dromatic/" + cropName + "/channels/sysch";

	for (i = 0; i < currentSize; i++){
		if (i > trimAmount){
			removeChannel(path + i);
		}
		Serial.flush();
	}
}

void addChannels(int currentSize, int addAmount){
	byte j, index, loopAmount;
	loopAmount = addAmount - currentSize;

	//Build Channels Settings File
	StaticJsonBuffer<64> channelObjBuffer;
	JsonObject& channelData = channelObjBuffer.createObject();
	channelData["id"] = currentChannelIndex;
	channelData["size"] = 80;
	channelData["sessionsTotal"] = 3;
	channelData["calibration"] = 0;

	//Build Session's settings file
	StaticJsonBuffer<185> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	sessionData.createNestedArray("date");
	sessionData.createNestedArray("time");
	sessionData["channel"] = currentChannelIndex;
	sessionData["amount"] = 80;
	sessionData["date"].asArray().add(rtc.getTime().year);
	sessionData["date"].asArray().add(rtc.getTime().mon + 1);
	sessionData["date"].asArray().add(rtc.getTime().date);
	sessionData["date"].asArray().add(rtc.getTime().dow);
	sessionData["time"].asArray().add(rtc.getTime().hour);
	sessionData["time"].asArray().add(rtc.getTime().min);
	sessionData["delay"] = sessionData["repeatBy"] = sessionData["repeat"] = 0;

	for (j = 1; j < loopAmount; j++){
		index = currentSize + j;
		sessionData["channel"] = index;
		makeChannel("dromatic/" + cropName + "/channels/sysch" + String(index), 2, channelData, sessionData);
		Serial.flush();
	}
}

void setChannelSize(int dir){
	if (cursorY == 0){
		if (cursorX == 2){
			tmpInts[0] = tmpInts[0] + dir;
			lcd.clear();
			tmpInts[0] = (tmpInts[0] < -1) ? -1 : (tmpInts[0] > 500) ? 500 : tmpInts[0]; //we must cap channel max size to 500ml
			String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? (tmpInts[0] == -1) ? "Infinte" : "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
			lcd.print(channelSize + F(" (ml) volume"));
			lcd.setCursor(0, 1);
			lcd.print(F("<back>  <ok|all>"));
			lcd.setCursor(cursorX, cursorY);
		}
	}
}

void setCalibrationSize(int dir){
	if (cursorX == 10){
		lcd.clear();
		String displayRots, displaySize;
		displayRots = displaySize = "00";

		tmpInts[1] = tmpInts[1] + dir;

		if (tmpInts[1] < 0) { tmpInts[1] = 0; }
		if (tmpInts[1] >= 10 && tmpInts[1] < 100) { displayRots = "0"; }
		if (tmpInts[1] >= 100) { displayRots = ""; }
		displayRots = displayRots + String(tmpInts[1]);

		if (tmpInts[0] < 0) { tmpInts[0] = 0; }
		if (tmpInts[0] >= 10 && tmpInts[0] < 100) { displaySize = "0"; }
		if (tmpInts[0] >= 100) { displaySize = ""; }
		displaySize = displaySize + String(tmpInts[0]);

		lcd.home();
		lcd.print(displaySize + F("(ml) ") + displayRots + F(" rots"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>  <ok|all>"));
		lcd.setCursor(cursorX, cursorY);
		float timeTurning = 31.5;

		RelayToggle(currentChannelIndex, true); //turn replay gate power on
		myStepper.step((dir == -1) ? 800 : -800);
		RelayToggle(currentChannelIndex, false); //turn replay gate power off

	}
}