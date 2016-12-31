#include "Sessions.h"
#include "Screens.h"

JsonObject& getSessionData(JsonBuffer& b, int channelIndex = currentChannelIndex, int sessionIndex = currentSessionIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/sessions/chses" + sessionIndex + "/session.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setSessionData(JsonObject& d, int channelIndex = currentChannelIndex, int sessionIndex = currentSessionIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/sessions/chses" + sessionIndex + "/session.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		openHomeScreen();
	}
}

void setSessionNumber(int dir){
	String totalDisplay;
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);

	lcd.clear();
	lcd.print(totalDisplay + F(" # OF SESSIONS"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}

void setSessionAmount(int dir){
	if (cursorX == 2){
		lcd.clear();
		String prefix, displayAmount;
		prefix = "00";
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 10){
			tmpInts[0] == 10;
		}
		if (tmpInts[0] >= 10 && tmpInts[0] < 100){
			prefix = "0";
		}
		if (tmpInts[0] >= 100){
			prefix = "";
		}
		displayAmount = prefix + String(tmpInts[0]);
		lcd.print(displayAmount + F("(ml) volume"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionDelay(int dir){
	if (cursorX == 2){
		lcd.clear();
		String prefix, displayDelay;

		prefix = "00";
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 10){
			tmpInts[0] == 10;
		}
		if (tmpInts[0] >= 10 && tmpInts[0] < 100){
			prefix = "0";
		}
		if (tmpInts[0] >= 100){
			prefix = "";
		}
		displayDelay = prefix + String(tmpInts[0]);
		lcd.print(displayDelay + F("(sec)  delay"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionRepeat(int dir) {
	char repeatsBuffer[8];
	String displayRepeat, displayRepeatBy;
	lcd.clear();
	lcd.home();

	if (cursorX == 13 && cursorY == 0){
		tmpInts[1] = tmpInts[1] + dir;
		if (tmpInts[1] < 0 || tmpInts[1] > 5){
			tmpInts[1] = 0;
		}
	}
	if (cursorX == 2 && cursorY == 1){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < -1 || tmpInts[0] >= 1000){
			tmpInts[0] = -1;
		}
	}

	displayRepeat = (tmpInts[0] == -1) ? "Inf." : (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) + "x" : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) + "x" : String(tmpInts[0]) + "x";
	displayRepeatBy = strcpy_P(repeatsBuffer, (char*)pgm_read_word(&(displayRepeats[tmpInts[1]])));

	lcd.print(F("Repeats: "));
	lcd.print(displayRepeatBy);
	lcd.setCursor(0, 1);
	lcd.print(displayRepeat + F(" <back> <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void makeSession(String path, JsonObject& data){
	SD.mkdir(path);
	SD.mkdir(path + "/Amt");
	SD.mkdir(path + "/Str");
	SD.mkdir(path + "/Dly");
	SD.mkdir(path + "/Rpt");

	char buffer[512];
	tmpFile = SD.open(path + "/session.dro", FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}

void addSessions(int currentSize, int addAmount){
	byte i;
	//Build Session's settings file
	StaticJsonBuffer<165> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	JsonArray& date = sessionData.createNestedArray("date");
	JsonArray& time = sessionData.createNestedArray("time");
	sessionData["channel"] = currentChannelIndex;
	sessionData["amount"] = 80;
	int currentYear = rtc.getTime().year;
	int nextYear = rtc.getTime().year + 1;
	int currentMonth = rtc.getTime().mon;
	int nextMonth = (rtc.getTime().mon + 1 > 11) ? 0 : rtc.getTime().mon + 1;

	date.add((currentMonth > nextMonth) ? nextYear : currentYear);		//year
	date.add(nextMonth);												//month
	date.add(rtc.getTime().date);		//day
	date.add(rtc.getTime().dow);		//day of week

	time.add(0); //hour
	time.add(0); //min

	sessionData["delay"] = sessionData["repeat"] = sessionData["repeatBy"] = sessionData["repeated"] = 0;

	//repeatBy developer key
	//0 none, 1 = hourly, 2 = daily, 3 = weekly, 4 = monthly, 5 = yearly
	for (i = 0; i <= addAmount; i++){
		if (i > currentSize){
			makeSession("dromatic/" + cropName + "/channels/sysch" + String(currentChannelIndex) + "/sessions/chses" + String(i), sessionData);
		}
		Serial.flush();
	}
}

void trimSessions(int currentSize, int trimAmount){
	byte i;
	String path = "dromatic/" + cropName + "/CHANNELS/SYSCH" + String(currentChannelIndex) + "/SESSIONS/CHSES";
	for (i = 0; i <= currentSize; i++){
		if (i > trimAmount){
			SD.rmdir(path + i + "/AMT");
			SD.rmdir(path + i + "/DLY");
			SD.rmdir(path + i + "/RPT");
			SD.rmdir(path + i + "/STR");
			SD.remove(path + i + "/SESSION.DRO");
			SD.rmdir(path + i);
		}
		Serial.flush();
	}
}