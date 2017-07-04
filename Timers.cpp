/*
*  DROMatic.ino
*  DROMatic OS Timers
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Timers.h"
#include "Core.h"
#include "Crops.h"
#include "Screens.h"
#include "DatesTime.h"

byte currentTimerIndex, currentTimerSessionIndex, currentTimerSessionDayIndex;

//Read & Write from SD
JsonObject& getTimerData(JsonBuffer& b){
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

//Prints
void printTimerWeeks(int dir) {
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
void printTimerStartEnd(int dir){

	String DOW[7] = { "Su", "Mo", "Tu", "We", "Th", "Fi", "Sa" };
	String DOWDisplay, startDisplay, endDisplay = "";
	String AMPM1 = F("AM");
	String AMPM2 = F("PM");
	if (cursorY == 0){
		if (cursorX == 14 || cursorX == 11){
			StaticJsonBuffer<timerSessionBufferSize> saveBuffer;
			JsonObject& saveData = getTimerSessionData(saveBuffer, currentTimerIndex, currentTimerSessionIndex);

			saveData["times"].asArray()[currentTimerSessionDayIndex].asArray()[0] = tmpInts[0]; //start hour
			saveData["times"].asArray()[currentTimerSessionDayIndex].asArray()[1] = tmpInts[1]; //end hour
			setTimerSessionData(saveData);
		}

		if (cursorX == 14){ //week
			//Next we are going to increament/decrement temp data
			currentTimerSessionIndex += dir;
			if (currentTimerSessionIndex < 1){
				currentTimerSessionIndex = 1;
			}
			if (!SD.exists("dromatic/" + cropName + "/timers/recep0" + currentTimerIndex + "/week" + currentTimerSessionIndex + ".dro")){
				makeNewFile("dromatic/" + cropName + "/timers/recep0" + currentTimerIndex, newTimerSessionData());
			}

			//Finally we are going to open the next week's data and set it to our temp data
			StaticJsonBuffer<timerSessionBufferSize> openBuffer;
			JsonObject& openData = getTimerSessionData(openBuffer, currentTimerIndex, currentTimerSessionIndex);
			tmpInts[0] = openData["times"].asArray()[currentTimerSessionDayIndex][0]; //start hour
			tmpInts[1] = openData["times"].asArray()[currentTimerSessionDayIndex][1]; //end hour
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
			tmpInts[0] = openData["times"].asArray()[currentTimerSessionDayIndex][0]; //start hour
			tmpInts[1] = openData["times"].asArray()[currentTimerSessionDayIndex][1]; //end hour
			
		}

		DOWDisplay = DOW[currentTimerSessionDayIndex];
		
		lcd.clear();
		lcd.home();

		//START
		if (cursorX == 1){
			tmpInts[0] += dir;
			if (tmpInts[0] > 23){
				tmpInts[0] = 23;
			}
			if (tmpInts[0] < 0) {
				tmpInts[0] = 0;
			}
			if (tmpInts[0] >= tmpInts[1] && tmpInts[1] < 24 && dir > 0){
				tmpInts[1]++;
			}
		}

		if (tmpInts[0] > 12){
			startDisplay = ((tmpInts[0] - 12) < 10) ? "0" + String(tmpInts[0] - 12) : String(tmpInts[0] - 12);
		}else{
			if (tmpInts[0] == 0){
				startDisplay = String(12);
			}else{
				startDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
			}
		}

		//END
		if (cursorX == 6){
			tmpInts[1] += dir;

			if (tmpInts[1] > 24){
				tmpInts[1] = 24;
			}
			if (tmpInts[1] < 1) {
				tmpInts[1] = 1;
			}
			if (tmpInts[1] <= tmpInts[0] && tmpInts[0] > 0 && dir != 0){
				tmpInts[0]--;
			}
		}

		if (tmpInts[1] > 12){
			endDisplay = ((tmpInts[1] - 12) < 10) ? "0" + String(tmpInts[1] - 12) : String(tmpInts[1] - 12);
		}
		else{
			if (tmpInts[1] == 0){
				endDisplay = String(12);
			}
			else{
				endDisplay = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);
			}
		}

		AMPM1 = (tmpInts[0] > 11 && tmpInts[0] < 24) ? F("PM") : F("AM");

		lcd.print(startDisplay);
		lcd.print(AMPM1);
		lcd.print(String("-"));

		AMPM2 = (tmpInts[1] > 11 && tmpInts[1] < 24) ? F("PM") : F("AM");

		lcd.print(endDisplay);
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

//Saves
void saveStartEnd(){
	if (cursorX == 11 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<timerBufferSize> jsonBuffer;
		JsonObject& data = getTimerSessionData(jsonBuffer);
		JsonArray& start = data["start"].asArray();
		JsonArray& end = data["end"].asArray();
		start[0] = tmpInts[0];
		start[0] = tmpInts[1];
		start[0] = tmpInts[2];

		end[0] = tmpInts[3];
		end[0] = tmpInts[4];
		end[0] = tmpInts[5];

		setTimerSessionData(data);
	}
	if (cursorX == 1 && cursorY == 1){
		exitScreen();
	}
}

//Helpers
JsonObject& newTimerSessionData(){
	byte i;
	StaticJsonBuffer<timerSessionBufferSize> jsonBuffer;
	JsonObject& sessionData = jsonBuffer.createObject();
	JsonArray& times = sessionData.createNestedArray("times");

	for (i = 1; i <= 7; i++){
		JsonArray& StartEnd = times.createNestedArray();
		StartEnd.add(0);//hour
		StartEnd.add(12); //am/pm
	}
	return sessionData;
}