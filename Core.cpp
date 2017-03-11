/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Core.h"
#include "Crops.h"
#include "Channels.h"
#include "Sessions.h"
#include "Menus.h"
#include "Screens.h"
#include "DatesTime.h"

JsonObject& getCoreData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/core.dro");
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setCoreData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/core.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

void coreInit(){
	if (SD.exists("dromatic")){ //has OS already been setup?
		StaticJsonBuffer<coreBufferSize> coreBuffer;
		JsonObject& coreData = getCoreData(coreBuffer);

		cropName = coreData["crop"].asString();
		if (cropName != "" && SD.exists("dromatic/" + cropName)){ //Loading up exisiting core file's crop directory
			screenName = "";
			tmpFile = SD.open("dromatic/" + cropName);
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			lcd.print(F("LOADING CROP..."));
			lcd.setCursor(0, 1);
			lcd.print(F("  PLEASE WAIT  "));
			lcd.home();
			StaticJsonBuffer<cropBufferSize> cropBuffer;
			JsonObject& cropData = getCropData(cropBuffer);
			JsonArray& ECData = cropData["ec"].asArray();
			JsonArray& PHData = cropData["ph"].asArray();
			PPMHundredth = cropData["ppm"];
			maxRegimens = cropData["regimens"];
			minPPM = ECData[0];
			maxPPM = ECData[1];
			minPH = PHData[0];
			maxPH = PHData[1];
			printHomeScreen();
		}
		else{ //we have core file with crop, but no crop directory. //VERY CORNER CASE!
			startNewCrop();
		}
	}
	else { //if OS has not been setup, lets build out core OS file / directory
		SD.mkdir("dromatic");
		lcd.print(F("New Drive Found!"));
		lcd.setCursor(0, 1);
		lcd.print(F(" Please Hold... "));
		tmpFile = SD.open("dromatic/core.dro", FILE_WRITE);
		char buffer[64];
		StaticJsonBuffer<coreBufferSize> coreBuffer;
		JsonObject& settings = coreBuffer.createObject();
		settings["crop"] = "";
		settings.printTo(buffer, sizeof(buffer));
		tmpFile.print(buffer);
		tmpFile.close();

		lcd.clear();
		lcd.home();
		lcd.print(F("New Crop Setup"));
		lcd.setCursor(0, 1);
		lcd.print(F(" Please Hold... "));
		delay(1000);
		lcd.clear();
		startNewCrop();
	}
}

void makeNewFile(String path, JsonObject& data){
	char buffer[1024];
	tmpFile = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}

//Pump Functions
void turing(){
	captureDateTime();
	byte i, j,
		setMonth, currentMonth,
		setDay, currentDay,
		setDOW, currentDOW,
		setHour, currentHour,
		setMin, currentMin,
		setAmount, setCalibration, setSize,
		totalSessions;

	int setDOY, currentDOY,
		setYear, currentYear;

	//We start by looping over channesl
	for (i = 1; i <= 8; i++){
		if (analogRead(0) >= 0 && analogRead(0) <= 650){
			break;
		}
		StaticJsonBuffer<channelBufferSize> channelBuffer;
		JsonObject& channel = getChannelData(channelBuffer, i);
		
		totalSessions = channel["sessionsTotal"];
		setCalibration = channel["calibration"];
		setSize = channel["size"];

		for (j = 1; j <= totalSessions; j++){
			if (analogRead(0) >= 0 && analogRead(0) <= 650){
				break;
			}
			printHomeScreen();

			StaticJsonBuffer<375> sessionBuffer;
			JsonObject& session = getSessionData(sessionBuffer, i, j);
			
			if (session["expired"] == true) continue; //lets skip this session if it has already expired.
			JsonArray& sessionDate = session["date"].asArray();
			JsonArray& sessionTime = session["time"].asArray();

			//Capture session's set data
			setYear = sessionDate[0];
			setMonth = sessionDate[1];
			setDay = sessionDate[2];
			setDOW = sessionDate[3];
			setDOY = calculateDayOfYear(setDay, setMonth, setYear);
			setHour = sessionTime[0];
			setMin = sessionTime[1];
			setAmount = session["amount"];

			//Capture current date/time data
			currentYear = tmpInts[0];
			currentMonth = tmpInts[1];
			currentDay = tmpInts[2];
			currentDOW = tmpInts[3];
			currentDOY = calculateDayOfYear(currentDay, currentMonth, currentYear);
			currentHour = tmpInts[4];
			currentMin = tmpInts[5];
			
			//Validation of session date time
			//if not valid we break loop and move on to next channel to speed up turing process.

				if (setYear != currentYear){	//year
					break;
				}

				if (setMonth != currentMonth){	//month
					break;
				}

				if (setDay != currentDay){		//day
					break;
				}

				if (setHour != currentHour){	//hour
					break;
				}

				if (setMin != currentMin){		//min
					break;
				}

			//LET THE DOSING BEGIN!!
			pumpSpin(setAmount, setCalibration, setSize, i, j, session); //do pump spin
			setSessionData(session, i, j, false); //finally we save our sessions if data has changed
		}
		if (i == 8){
			printHomeScreen();
		}
	}
}

void RelayToggle(int channel, bool gate) {
	if (gate == true){
		switch (channel){
		case 1:
			digitalWrite(RELAY1, LOW);
			break;
		case 2:
			digitalWrite(RELAY2, LOW);
			break;
		case 3:
			digitalWrite(RELAY3, LOW);
			break;
		case 4:
			digitalWrite(RELAY4, LOW);
			break;
		case 5:
			digitalWrite(RELAY5, LOW);
			break;
		case 6:
			digitalWrite(RELAY6, LOW);
			break;
		case 7:
			digitalWrite(RELAY7, LOW);
			break;
		case 8:
			digitalWrite(RELAY8, LOW);
			break;
		case 9:
			digitalWrite(RELAY9, LOW);
			break;
		case 10:
			digitalWrite(RELAY10, LOW);
			break;
		}
	}
	if (gate == false){
		switch (channel){
		case 1:
			digitalWrite(RELAY1, HIGH);
			break;
		case 2:
			digitalWrite(RELAY2, HIGH);
			break;
		case 3:
			digitalWrite(RELAY3, HIGH);
			break;
		case 4:
			digitalWrite(RELAY4, HIGH);
			break;
		case 5:
			digitalWrite(RELAY5, HIGH);
			break;
		case 6:
			digitalWrite(RELAY6, HIGH);
			break;
		case 7:
			digitalWrite(RELAY7, HIGH);
			break;
		case 8:
			digitalWrite(RELAY8, HIGH);
			break;
		case 9:
			digitalWrite(RELAY9, HIGH);
			break;
		case 10:
			digitalWrite(RELAY10, HIGH);
			break;
		}
	}
}

void pumpSpin(int setAmount, int setCalibration, int channelSize, int channelNumber, int sessionNumber, JsonObject& sessionData){
	RelayToggle(channelNumber, true); //turn channel gate power on
	//int setAmount, int setCalibration, int channelSize, int channelNumber
	int mlPerSec = (setCalibration * 100) / 60; //100m / 60sec = 1.6ml per seconds
	int pumpLength = setAmount / mlPerSec; //25ml target / 1.6ml per seconds = 15.625 seconds
	for (int i = 0; i < pumpLength; i++){
		delay(1000);
		printHomeScreen();
		Serial.flush();
	}
	RelayToggle(channelNumber, false); //turn channel gate power on
}