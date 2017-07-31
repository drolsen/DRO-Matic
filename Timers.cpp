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
void setTimerData(JsonObject& d){
	char b[128];
	tmpFile = SD.open("dromatic/" + cropName + "/Timer.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}
JsonObject& getTimerSessionData(JsonBuffer& b, int timerIndex = currentTimerIndex, int weekIndex = currentTimerSessionIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/Timers/Recep0" + timerIndex + "/Week" + weekIndex + ".dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setTimerSessionData(JsonObject& d, int timerIndex = currentTimerIndex, int weekIndex = currentTimerSessionIndex){
	char b[128];
	tmpFile = SD.open("dromatic/" + cropName + "/Timers/Recep0" + timerIndex + "/Week" + weekIndex + ".dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

void checkTimers(){
	byte currentHour, currentDOW, startHour, endHour, currentReceptical, currentRecepticalWeek;
	Time time = rtc.getTime();
	currentHour = time.hour;
	currentDOW = time.dow;
	for (byte i = 0; i < 4; i++){
		currentReceptical = 13 + i;
		if (currentHour >= timerStartHours[i] && currentHour < timerEndHours[i]){
			RelayToggle(currentReceptical, true);
		}
		else{
			//Only if if a timer was previously on before turing it off, do we gather next start/end times.
			boolean updateReceptical = false;
			switch (i){
			case 1:
				if (digitalRead(RELAY13) == 'LOW'){
					updateReceptical = true;
				}
				break;
			case 2:
				if (digitalRead(RELAY14) == 'LOW'){
					updateReceptical = true;
				}
				break;
			case 3:
				if (digitalRead(RELAY15) == 'LOW'){
					updateReceptical = true;
				}
				break;
			case 4:
				if (digitalRead(RELAY16) == 'LOW'){
					updateReceptical = true;
				}
				break;
			}
			if (updateReceptical == true){
				StaticJsonBuffer<timerBufferSize> timersBuffer;
				JsonObject& timersData = getTimerData(timersBuffer);
				int currentWeek = timersData["currents"].asArray()[i]; //i = current receptical
				int maxWeeks = timersData["weeks"].asArray()[i]; //i = current receptical

				//Update RAM & SD currents
				timersData["currents"].asArray()[i] = currentTimerSessions[i] = ((currentTimerSessions[i] + 1) > maxWeeks) ? currentTimerSessions[i] : (currentTimerSessions[i] + 1);

				StaticJsonBuffer<timerSessionBufferSize> timerSessionBuffer;
				JsonObject& sessionData = getTimerSessionData(timerSessionBuffer, (i + 1), currentWeek);
				timerStartHours[i] = sessionData["times"].asArray()[currentDOW].asArray()[0];
				timerEndHours[i] = sessionData["times"].asArray()[currentDOW].asArray()[1];
				setTimerData(timersData);
			}
			//Finally no matter what, we always turn off recepticals (hench all the checking above).
			RelayToggle(currentReceptical, false);
		}
	}
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

	String DOW[7] = { F("Su"), F("Mo"), F("Tu"), F("We"), F("Th"), F("Fi"), F("Sa") };
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

		tmpDisplay[2] = DOW[currentTimerSessionDayIndex];
		
		lcd.clear();

		//START
		if (cursorX == 1){
			tmpInts[0] += dir;
			if (tmpInts[0] > 24){
				tmpInts[0] = 0;
			}
			if (tmpInts[0] < 0) {
				tmpInts[0] = 24;
			}
		}

		if (tmpInts[0] > 12){
			tmpDisplay[0] = ((tmpInts[0] - 12) < 10) ? "0" + String(tmpInts[0] - 12) : String(tmpInts[0] - 12);
		}else{
			tmpDisplay[0] = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
			if (tmpInts[0] == 0){
				tmpDisplay[0] = String(12);
			}
		}

		//END
		if (cursorX == 6){
			tmpInts[1] += dir;

			if (tmpInts[1] > 24){
				tmpInts[1] = 0;
			}
			if (tmpInts[1] < 0) {
				tmpInts[1] = 24;
			}
		}

		if (tmpInts[1] > 12){
			tmpDisplay[1] = ((tmpInts[1] - 12) < 10) ? "0" + String(tmpInts[1] - 12) : String(tmpInts[1] - 12);
		}else{
			tmpDisplay[1] = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);
			if (tmpInts[1] == 0){
				tmpDisplay[1] = String(12);
			}
		}

		AMPM1 = (tmpInts[0] > 11 && tmpInts[0] < 24) ? F("PM") : F("AM");

		lcd.print(tmpDisplay[0]); //start time
		lcd.print(AMPM1);		  //am or pm
		lcd.print(String("-"));

		AMPM2 = (tmpInts[1] > 11 && tmpInts[1] < 24) ? F("PM") : F("AM");

		lcd.print(tmpDisplay[1]); //end time
		lcd.print(AMPM2);		  //am or pm
		lcd.print(F(" "));
		lcd.print(tmpDisplay[2]); //day of week
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
	if (cursorX == 1 || cursorX == 11 && cursorY == 1){
		tmpInts[0] = tmpInts[1] = tmpInts[2] = tmpInts[3] = tmpInts[4] = tmpInts[5] = 0;
		tmpDisplay[0] = tmpDisplay[1] = tmpDisplay[2] = "";
		currentTimerSessionDayIndex = currentTimerSessionIndex = 0;
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