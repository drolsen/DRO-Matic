/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Timers.h"
#include "Core.h"
#include "Crops.h"
#include "Screens.h"
#include "DatesTime.h"

byte currentTimerIndex, currentTimerSessionIndex, currentTimerSessionDayIndex;

//Get and set timer data
JsonObject& getTimerData(JsonBuffer& b, int timerIndex = currentTimerIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/Timer.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setTimerData(JsonObject& d, int timerIndex = currentTimerIndex){
	char b[128];
	tmpFile = SD.open("dromatic/" + cropName + "/Timers/Recep0" + timerIndex + "/Week" + currentTimerSessionIndex + ".dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

//Get and set timer sessions
JsonObject& getTimerSessionData(JsonBuffer& b, int timerIndex = currentTimerIndex, int sessionIndex = currentTimerSessionIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/Timers/Recep0" + timerIndex + "/Week" + sessionIndex + ".dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setTimerSessionData(JsonObject& d, int timerIndex = currentTimerIndex, int sessionIndex = currentTimerSessionIndex){
	char b[128];
	tmpFile = SD.open("dromatic/" + cropName + "/Timers/Recep0" + timerIndex + "/Week" + sessionIndex + ".dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

void setTimerWeeks(int dir) {
	tmpInts[0] += dir;
	String weeksDisplay;
	if (tmpInts[0] > 99){ //max
		tmpInts[0] = 99;
	}else if (tmpInts[0] < 0) {
		tmpInts[0] = 0;
	}

	if (tmpInts[0] >= 10){
		weeksDisplay = F("");
	}else if (tmpInts[0] >= 0){ //min
		weeksDisplay = F("0");
	}
	lcd.clear();
	lcd.print(F("TIMED WEEKS "));
	lcd.print(weeksDisplay);
	lcd.print(tmpInts[0]);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);

}

void setTimerStartEnd(int dir){

	String DOW[7] = { "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" };
	String DOWDisplay, startDisplay, endDisplay = "";
	String AMPM1 = "AM";
	String AMPM2 = "PM";
	if (cursorY == 0){
		if (cursorX == 14 || cursorX == 11){
			StaticJsonBuffer<timerSessionBufferSize> saveBuffer;
			JsonObject& saveData = getTimerSessionData(saveBuffer, currentTimerIndex, currentTimerSessionIndex);

			saveData["start"].asArray()[currentTimerSessionDayIndex].asArray()[0] = tmpInts[0]; //start hour
			saveData["start"].asArray()[currentTimerSessionDayIndex].asArray()[1] = tmpInts[1]; //start am/pm
			saveData["end"].asArray()[currentTimerSessionDayIndex].asArray()[0] = tmpInts[2]; //end hour
			saveData["end"].asArray()[currentTimerSessionDayIndex].asArray()[1] = tmpInts[3]; //end am/pm
			setTimerSessionData(saveData);
		}

		if (cursorX == 14){ //week
			//Next we are going to increament/decrement temp data
			currentTimerSessionIndex += dir;
			if (currentTimerSessionIndex < 1){
				currentTimerSessionIndex = 1;
			}
			if (!SD.exists("dromatic/" + cropName + "/timers/recep0" + currentTimerIndex + "/week" + currentTimerSessionIndex + ".dro")){
				makeTimerSessionFile("dromatic/" + cropName + "/timers/recep0" + currentTimerIndex, currentTimerSessionIndex);
			}

			//Finally we are going to open the next week's data and set it to our temp data
			StaticJsonBuffer<timerSessionBufferSize> openBuffer;
			JsonObject& openData = getTimerSessionData(openBuffer, currentTimerIndex, currentTimerSessionIndex);
			tmpInts[0] = openData["start"].asArray()[currentTimerSessionDayIndex][0]; //start hour
			tmpInts[1] = openData["start"].asArray()[currentTimerSessionDayIndex][1]; //start am/pm
			tmpInts[2] = openData["end"].asArray()[currentTimerSessionDayIndex][0]; //end hour
			tmpInts[3] = openData["end"].asArray()[currentTimerSessionDayIndex][1]; //end am/pm
			currentTimerSessionDayIndex = 0;
		}

		if (cursorX == 11){//day
			//Next we are going to increament/decrement temp data
			currentTimerSessionDayIndex += dir;
			if (currentTimerSessionDayIndex <= 0){
				currentTimerSessionDayIndex = 0;
			}
			if (currentTimerSessionDayIndex >= 6){
				currentTimerSessionDayIndex = 6;
			}

			//Finally we are going to open the next week's data and set it to our temp data
			StaticJsonBuffer<timerSessionBufferSize> openBuffer;
			JsonObject& openData = getTimerSessionData(openBuffer, currentTimerIndex, currentTimerSessionIndex);
			tmpInts[0] = openData["start"].asArray()[currentTimerSessionDayIndex][0]; //start hour
			tmpInts[1] = openData["start"].asArray()[currentTimerSessionDayIndex][1]; //start am/pm
			tmpInts[2] = openData["end"].asArray()[currentTimerSessionDayIndex][0]; //end hour
			tmpInts[3] = openData["end"].asArray()[currentTimerSessionDayIndex][1]; //end am/pm
			
		}
		DOWDisplay = DOW[currentTimerSessionDayIndex];
		
		lcd.clear();
		lcd.home();

		//START
		if (cursorX == 1){
			tmpInts[0] += dir;
			if (tmpInts[0] > 12){
				tmpInts[0] = 1;
			}
			if (tmpInts[0] < 1) {
				tmpInts[0] = 12;
			}
		}
		if (tmpInts[0] >= 10){
			startDisplay = F("");
		}else{
			startDisplay = F("0");
		}
		if (cursorX == 3){
			if (dir > 0){
				tmpInts[1] = 1;
			}else{
				tmpInts[1] = 0;
			}
		}
		AMPM1 = (tmpInts[1] > 0) ? F("PM") : F("AM");

		lcd.print(startDisplay);
		lcd.print(tmpInts[0]);
		lcd.print(AMPM1);
		lcd.print(String("-"));

		//END
		if (cursorX == 6){
			tmpInts[2] += dir;

			if (tmpInts[2] > 12){
				tmpInts[2] = 1;
			}
			if (tmpInts[2] < 1) {
				tmpInts[2] = 12;
			}
		}

		if (tmpInts[2] >= 10){
			endDisplay = F("");
		}else{
			endDisplay = F("0");
		}

		if (cursorX == 8){
			if (dir > 0){
				tmpInts[3] = 1;
			}else{
				tmpInts[3] = 0;
			}
		}

		AMPM2 = (tmpInts[3] > 0) ? F("PM") : F("AM");

		lcd.print(endDisplay);
		lcd.print(tmpInts[2]);
		lcd.print(AMPM2);
		lcd.print(F(" "));
		lcd.print(DOWDisplay);
		lcd.print(F("/"));
		if (currentTimerSessionIndex < 10){
			lcd.print(F("0"));
		}
		lcd.print(currentTimerSessionIndex);
	}

	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void makeTimerSessionFile(String path, byte index){
	byte i;
	StaticJsonBuffer<768> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	JsonArray& start = root.createNestedArray("start");
	JsonArray& end = root.createNestedArray("end");

	for (i = 1; i <= 7; i++){
		JsonArray& StartDOW = start.createNestedArray();
		JsonArray& EndDOW = end.createNestedArray();

		StartDOW.add(12);//hour
		StartDOW.add(0); //am/pm

		EndDOW.add(12);//hour
		EndDOW.add(1); //am/pm
	}

	char buffer[128];
	root.printTo(buffer, sizeof(buffer));

	tmpFile = SD.open(path + "/Week"+index+".dro", FILE_WRITE);
	tmpFile.print(buffer);
	tmpFile.close();

	Serial.flush();
}