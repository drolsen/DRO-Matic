/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <SPI.h> //Suppoting lib for SD card
#include <SD.h> //SD card API
#include <StandardCplusplus.h> //STD
#include <StandardCplusplus\vector> //Vectors
#include <StandardCplusplus\ctime> //Time helper
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)
#include <DS3231.h> //Real time clock lib
#include <Stepper.h> //Stepper motoer lib

#include "Globals.h" //All temp and PROGMEM global variables
#include "Core.h" //All core functions and variables
#include "Crops.h" //All crop functions and variables
#include "Channels.h" //All channel functions and variables
#include "Sessions.h" //All session functions and variables
#include "Screens.h" //All screen functions and variables
#include "Menus.h" //All menu functions and variables
#include "DatesTime.h" //All date & time functions and variables

//Our main setup function
void setup()
{
	lcd.createChar(0, upArrow);
	lcd.createChar(1, downArrow);
	lcd.begin(16, 2);
	Serial.begin(9600);
	rtc.begin();
	captureDateTime();
	String relayName;
	myStepper.setSpeed(stepperSpeed);
	previousMillis = 0;

	pinMode(MS1MS2, OUTPUT);
	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);
	pinMode(RELAY4, OUTPUT);
	pinMode(RELAY5, OUTPUT);
	pinMode(RELAY6, OUTPUT);
	pinMode(RELAY7, OUTPUT);
	pinMode(RELAY8, OUTPUT);
	pinMode(RELAY9, OUTPUT);
	pinMode(RELAY10, OUTPUT);

	digitalWrite(MS1MS2, LOW);
	digitalWrite(RELAY1, HIGH);
	digitalWrite(RELAY2, HIGH);
	digitalWrite(RELAY3, HIGH);
	digitalWrite(RELAY4, HIGH);
	digitalWrite(RELAY5, HIGH);
	digitalWrite(RELAY6, HIGH);
	digitalWrite(RELAY7, HIGH);
	digitalWrite(RELAY8, HIGH);
	digitalWrite(RELAY9, HIGH);
	digitalWrite(RELAY10, HIGH);

	if (!SD.begin(53)){
		lcd.print(F("SD Card Required"));
		lcd.setCursor(0, 1);
		lcd.print(F("Insert And Rest"));
		screenName = "REQUIREDSD";
	} 
	else {
		coreInit();
	}
}

//Our runtime loop function
void loop()
{
	Key = analogRead(0);
	currentMillis = millis();
	if (Key >= 0 && Key <= 650){
		previousMillis = currentMillis;
	}

	//30 seconds
	if ((currentMillis - previousMillis) >= 30000) {
		if (screenName == "") {
			previousMillis = currentMillis;
			openHomeScreen();
		}
	}

	if (Key == 0 || Key == 408){
		//Right & Left
		if (screenName == "NewCrop"){
			matrix = {
				{ { 0, 15 } },
				{ { 11, 11 } }
			};
			if (Key == 0){
				renameCrop(NULL);
				lcd.blink();
			}
		}
		if (screenName == "OPEN"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 9, 9 } }
			};
		}
		if (screenName == "DATETIME" || screenName == "STR"){
			matrix = {
				{ { 1, 1 }, { 4, 4 }, { 10, 10 }, { 13, 13 } },
				{ { 3, 3 }, { 6, 6 }, { 13, 13 } }
			};
		}
		if (screenName == "PPM"){
			matrix = {
				{ { 3, 3 }, { 8, 8 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PH"){
			matrix = {
				{ { 3, 3 }, { 9, 9 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "PHCHL"){
			matrix = {
				{ { 6, 6 }, { 14, 14 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHNUM" || screenName == "CHDOSES"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "AMT"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHSIZE"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 9, 9 }, { 12, 12 } }
			};
		}
		if (screenName == "CHCALIB"){
			matrix = {
				{ { 10, 10 } },
				{ { 1, 1 }, { 9, 9 }, { 12, 12 } }
			};
		}
		if (screenName == "RPT"){
			matrix = {
				{ { 13, 13 } },
				{ { 2, 2 }, { 6, 6 }, { 13, 13 } }
			};
		}

		cursorX = (Key == 0) ? cursorX + 1 : cursorX - 1;
		if (Key == 408 && screenName == ""){
			menusHistory.pop_back();
			menuIndex = 0;
			tmpFile = SD.open("dromatic/" + cropName + "/" + getMenuHistory());
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			lcd.clear();
			printDisplayNames(menus.front());
			printScrollArrows();
		}
		if (Key == 0 || Key == 408 && screenName != ""){
			screenMatrix();
		}
		delay(250);
	}
	if (Key == 99 || Key == 255) {
		//Up & Down
		int dir = (Key == 99) ? 1 : -1;
		int lgdir = (Key == 99) ? 10 : -10;
		if (screenName == "NewCrop"){
			renameCrop(dir);
		}
		if (screenName == "OPEN"){
			if (cursorX == 0 && cursorY == 0){
				lcd.clear();
				scrollMenus(dir);
				lcd.setCursor(0, 1);
				lcd.print(F("<back>  <open>"));
				lcd.home();
			}
		}
		if (screenName == "PPM"){
			setPPMRangeValues(dir);
		}
		if (screenName == "PH"){
			setPHRange(dir);
		}
		if (screenName == "PHCHL"){
			setPHChannels(dir);
		}
		if (screenName == "DATETIME"){
			setDateTime(dir);
		}
		if (screenName == "CHNUM"){
			setChannelNumber(dir);
		}
		if (screenName == "CHDOSES"){
			setSessionNumber(dir);
		}
		if (screenName == "CHSIZE"){
			setChannelSize(lgdir);
		}
		if (screenName == "CHCALIB"){
			setCalibrationSize(dir);
		}
		if (screenName == "AMT"){
			setSessionAmount(lgdir);
		}
		if (screenName == "STR"){
			setDateTime(dir);
		}
		if (screenName == "RPT"){
			setSessionRepeat(dir);
		}
		if (screenName == ""){
			scrollMenus(dir);
		}
		delay(250);
	}
	if (Key == 639) {
		//Select
		if (screenName == ""){
			lcd.clear();
			String history = getMenuHistory();
			tmpFile = SD.open("dromatic/" + cropName + "/" + history + "/" + menus[menuIndex]);
			menusHistory.push_back(menus[menuIndex]);
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			if (menus.size() > 0){
				menuIndex = 0;
				tmpFile.close();
				printDisplayNames(menus.front());
				printScrollArrows();
			}
			else {
				screenName = menusHistory.back();
				lcd.blink();
				lcd.home();
				if (screenName == "DATETIME"){
					captureDateTime();
					char monthsBuffer[8];
					lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[1]]))) + F(" ") + tmpDisplay[1]);
					lcd.setCursor(0, 1);
					lcd.print(String(tmpInts[0]) + F(" <back> <ok>"));
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "NEWCROP"){
					menus.clear();
					menusHistory.clear();
					currentAlphaIndex = 0;
					currentChannelIndex = 0;
					currentSessionIndex = 0;
					menuIndex = 0;
					cursorX = cursorY = 0;
					startNewCrop();
				}
				if (screenName == "OPEN"){
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <open>"));
					lcd.home();
					cursorX = cursorY = 0;
					tmpFile = SD.open("dromatic/");
					getDirectoryMenus(tmpFile);
					menuIndex = 0;
					lcd.print(menus[menuIndex]);
					printScrollArrows();
					lcd.home();
				}
				if (screenName == "PPM"){
					lcd.print(String(minPPM) + F("-") + String(maxPPM) + F(" EC/PPM"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "PH"){
					DynamicJsonBuffer jsonBuffer;
					JsonObject& data = getCropData(jsonBuffer);
					tmpFloats[0] = data["ph"].asArray()[0];
					tmpFloats[1] = data["ph"].asArray()[1];
					lcd.print(String(tmpFloats[0]));
					lcd.write(byte(1));
					lcd.print(String(F(" ")) + String(tmpFloats[1]));
					lcd.write(byte(0));
					lcd.print(F(" PH"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>    <next>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "CHNUM"){
					cursorX = 1;
					cursorY = 0;
					DynamicJsonBuffer jsonBuffer;
					JsonObject& data = getCropData(jsonBuffer);
					int total = data["totalChannels"];
					tmpInts[0] = total;
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" CHANNELS"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CHDOSES"){
					cursorX = 1;
					cursorY = 0;
					DynamicJsonBuffer channelBuffer;
					JsonObject& data = getChannelData(channelBuffer);
					tmpInts[0] = data["sessionsTotal"];
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" # OF SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CHSIZE"){
					DynamicJsonBuffer channelBuffer;
					JsonObject& data = getChannelData(channelBuffer);
					tmpInts[0] = data["size"];
					String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(channelSize + F(" (ml) volume"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <ok|all>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CHCALIB"){
					DynamicJsonBuffer channelBuffer;
					JsonObject& data = getChannelData(channelBuffer);
					tmpInts[0] = data["size"];
					tmpInts[1] = data["calibration"];
					String targetSize = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);
					String rotsSize = (tmpInts[1] >= 10 && tmpInts[1] <= 99) ? "0" + String(tmpInts[1]) : (tmpInts[1] < 10 && tmpInts[1] >= 0) ? "00" + String(tmpInts[1]) : String(tmpInts[1]);
					lcd.print(targetSize + F("(ml) ") + rotsSize + F(" rots"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <ok|all>"));
					cursorX = 10;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "AMT"){
					DynamicJsonBuffer sessionBuffer;
					JsonObject& data = getSessionData(sessionBuffer);
					tmpInts[0] = data["amount"];
					String displayAmount = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);

					lcd.print(displayAmount + F("(ml) volume"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "STR"){
					captureSessionDateTime();
					char monthsBuffer[8];
					lcd.clear();
					lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[1]]))) + F(" ") + tmpDisplay[1]);
					lcd.setCursor(0, 1);
					lcd.print(String(tmpInts[0]) + F(" <back> <ok>"));
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "RPT"){
					char repeatsBuffer[8];
					DynamicJsonBuffer sessionBuffer;
					JsonObject& data = getSessionData(sessionBuffer);
					tmpInts[0] = data["repeat"];
					tmpInts[1] = data["repeatBy"];

					String displayRepeat = (tmpInts[0] == -1)? "Inf." : (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) + "x" : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) + "x" : String(tmpInts[0]) + "x";
					String displayRepeatBy = strcpy_P(repeatsBuffer, (char*)pgm_read_word(&(displayRepeats[tmpInts[1]])));

					lcd.print(F("Repeats: "));
					lcd.print(displayRepeatBy);
					lcd.setCursor(0, 1);
					lcd.print(displayRepeat + F(" <back> <ok>"));
					cursorX = 13;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
			}
			delay(350);
		}
		//Saves
		if (screenName == "NewCrop"){
			if (cursorX == 11 && cursorY == 1){
				String nameConfirm;
				for (int i = 0; i < 15; i++){
					nameConfirm = nameConfirm + nameArry[i];
				}
				if (nameConfirm == ""){
					lcd.clear();
					lcd.home();
					lcd.print(F("Sorry, No Blank"));
					lcd.setCursor(0, 1);
					lcd.print(F("Crop Names"));
					delay(3000);
					lcd.clear();
					lcd.setCursor(0, 1);
					lcd.print(F("Crop Name <done>"));
					cursorX = cursorY = 0;
					lcd.home();
				}
				else if (SD.exists("dromatic/" + nameConfirm)){
					lcd.clear();
					lcd.home();
					lcd.print(F("Sorry, Crop"));
					lcd.setCursor(0, 1);
					lcd.print(F("Already Exists"));
					delay(3000);
					lcd.clear();
					lcd.print(nameConfirm);
					lcd.setCursor(0, 1);
					lcd.print(F("Crop Name <done>"));
					cursorX = cursorY = 0;
					lcd.home();
				}
				else {
					buildCrop();
				}
			}
		}
		if (screenName == "OPEN"){
			if (cursorX == 9 && cursorY == 1){
				if (menus[menuIndex] != cropName){
					changeCrop();
				}
				else{
					exitScreen();
				}
			}
			if (cursorX == 1 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "PPM"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
				data["ppm"].asArray()[0] = minPPM;
				data["ppm"].asArray()[1] = maxPPM;
				setCropData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "PH"){
			if (cursorX == 11 && cursorY == 1){
				lcd.clear();
				lcd.home();
				DynamicJsonBuffer jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
				data["ph"].asArray()[0].set(tmpFloats[0]);
				data["ph"].asArray()[1].set(tmpFloats[1]);

				setCropData(data, false);
				tmpFloats[0] = tmpFloats[1] = 0.0;
				screenName = "PHCHL";
				delay(250);

				tmpInts[0] = data["phChannels"].asArray()[0];
				tmpInts[1] = data["phChannels"].asArray()[1];

				String UpDisplay = (tmpInts[0] < 10) ? String(F("0")) + String(tmpInts[0]) : String(tmpInts[0]);
				String DownDisplay = (tmpInts[1] < 10) ? String(F("0")) + String(tmpInts[1]) : String(tmpInts[1]);
				lcd.blink();
				lcd.print(F("PH"));
				lcd.write(byte(1));
				lcd.print(F("CH"));
				lcd.print(UpDisplay);
				lcd.print(F(" PH"));
				lcd.write(byte(0));
				lcd.print(String(F("CH")) + DownDisplay);
				lcd.setCursor(0, 1);
				lcd.print(F("<back>      <ok>"));
				cursorX = 6;
				cursorY = 0;
				lcd.setCursor(cursorX, cursorY);
			}
			if (cursorX == 1 && cursorY == 1){
				tmpFloats[0] = tmpFloats[1] = 0.0;
				screenName = "";
				exitScreen();
			}
		}
		if (screenName == "PHCHL"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
				data["phChannels"].asArray()[0] = tmpInts[0];
				data["phChannels"].asArray()[1] = tmpInts[1];
				setCropData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = tmpInts[1] = 0;
				exitScreen();
			}
		}
		if (screenName == "DATETIME"){
			if (cursorX == 13 && cursorY == 1){
				//hour, min, seconds
				rtc.setTime(tmpInts[4], tmpInts[5], 0);
				//day, month (RTC counts first month as 1, not 0), year
				rtc.setDate(tmpInts[2], tmpInts[1]+1, tmpInts[0]);
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				tmpDisplay[0] = ""; //suffix
				tmpDisplay[1] = ""; //hour
				tmpDisplay[2] = ""; //min
				tmpDisplay[3] = ""; //day
				exitScreen();
			}
		}
		if (screenName == "CHNUM"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				int i, j, currentTotal, newTotal, dir;
				Time currentTime = rtc.getTime();
				DynamicJsonBuffer jsonBuffer;
				JsonObject& cropData = getCropData(jsonBuffer);

				currentTotal = cropData["totalChannels"]; //capture orignal total
				newTotal = tmpInts[0];
				dir = (currentTotal < newTotal) ? 1 : -1;

				cropData["totalChannels"] = newTotal; //then set new total
				setCropData(cropData);
				if (dir < 0){
					lcd.print(F(" TRIM CHANNELS "));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					trimChannels(currentTotal + 1, newTotal);
				}

				if (dir > 0){
					lcd.print(F("ADDING CHANNELS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					addChannels(currentTotal, newTotal + 1);
				}
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHDOSES"){
			if (cursorX == 13 && cursorY == 1){
				//Get channel data
				DynamicJsonBuffer channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);
				lcd.clear();
				lcd.home();

				if (channelData["sessionsTotal"] > tmpInts[0]){ //we are trimming sessions
					lcd.print(F("TRIM SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					trimSessions(channelData["sessionsTotal"], tmpInts[0]);
				}
				else if (channelData["sessionsTotal"] < tmpInts[0]){ //we are adding sessions
					lcd.print(F("ADDING SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					addSessions(channelData["sessionsTotal"], tmpInts[0]);
				}
				channelData["sessionsTotal"] = tmpInts[0]; //update channel's session total
				setChannelData(channelData);
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHSIZE"){
			if (cursorX == 9 && cursorY == 1){ //single channel save
				DynamicJsonBuffer channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);

				channelData["size"] = tmpInts[0];
				setChannelData(channelData);
			}
			if (cursorX == 12 && cursorY == 1){ //all channel save
				lcd.clear();
				lcd.home();
				lcd.print(F("  SAVING ALL  "));
				lcd.setCursor(0, 1);
				lcd.print(F("  PLEASE WAIT "));

				DynamicJsonBuffer cropBuffer;
				JsonObject& cropData = getCropData(cropBuffer);
				int channelSize = cropData["totalChannels"];
				while (channelSize--){
					DynamicJsonBuffer channelBuffer;
					JsonObject& channelData = getChannelData(channelBuffer, channelSize);
					channelData["size"] = tmpInts[0];
					setChannelData(channelData, channelSize, false);
				}
			}
			if (cursorX == 1 || cursorX == 9 || cursorX == 12 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHCALIB"){
			if (cursorX == 9 && cursorY == 1){ //single channel save
				DynamicJsonBuffer channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);

				channelData["calibration"] = tmpInts[1];
				setChannelData(channelData);
				tmpInts[1] = 0;
				tmpInts[0] = 0;
				exitScreen();
			}
			if (cursorX == 12 && cursorY == 1){ //all channel save
				lcd.clear();
				lcd.home();
				lcd.print(F("  SAVING ALL  "));
				lcd.setCursor(0, 1);
				lcd.print(F("  PLEASE WAIT "));
				
				DynamicJsonBuffer cropBuffer;
				JsonObject& cropData = getCropData(cropBuffer);
				int channelSize = cropData["totalChannels"];
				while (channelSize--){
					DynamicJsonBuffer channelBuffer;
					JsonObject& channelData = getChannelData(channelBuffer, channelSize);
					channelData["calibration"] = tmpInts[1];
					setChannelData(channelData, channelSize, false);
				}
			}
			if (cursorX == 1 || cursorX == 9 || cursorX == 12 && cursorY == 1){
				tmpInts[1] = tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "AMT"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);
				sessionData["amount"] = tmpInts[0];
				setSessionData(sessionData);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "STR"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.noBlink();
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);
				JsonArray& date = sessionData["date"];
				JsonArray& time = sessionData["time"];

				date[0] = tmpInts[0]; //year
				date[1] = tmpInts[1]; //month
				date[2] = tmpInts[2]; //day
				date[3] = tmpInts[3]; //day of week
				time[0] = tmpInts[4]; //hour
				time[1] = tmpInts[5]; //min

				//Leap year help
				tm time_in = { 
					0,			// second
					tmpInts[5], // minute
					tmpInts[4], // hour
					tmpInts[2], // 1-based day
					tmpInts[1], // 0-based month
					tmpInts[0] - tmpInts[0] //year since 1900
				};
				time_t time_temp = mktime(&time_in);
				tm const *time_out = localtime(&time_temp);
				sessionData["date"].asArray()[3] = time_out->tm_wday;
				setSessionData(sessionData); //save session data
				tmpDisplay[0] = tmpDisplay[1] = tmpDisplay[2] = tmpDisplay[3] = ""; //Clear out tmpDisplays
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "RPT"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);

				sessionData["repeatBy"] = tmpInts[1];
				sessionData["repeat"] = sessionData["repeated"] = tmpInts[0];
				//we use this as a changeable variable so we can use repeat value for crop resets

				setSessionData(sessionData);
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				tmpInts[1] = tmpInts[0] = 0;
				exitScreen();
			}
		}
	}
}