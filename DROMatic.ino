/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/
#include <Wire.h>    //Include the software serial library for white sheild
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <SPI.h> //Suppoting lib for SD card
#include <SD.h> //SD card API
#include <StandardCplusplus.h> //STD
#include <vector> //Vectors
#include <ctime> //Time helper
#include <ArduinoJson.h> //Arduno Json (aka epic)
#include <DS3231.h> //Real time clock lib
 
#include "src/Globals.h" //All temp and PROGMEM global variables
#include "src/Core.h" //All core functions and variables
#include "src/Crops.h" //All crop functions and variables
#include "src/Pumps.h" //All pump functions and variables
#include "src/Regimens.h" //All session functions and variables
#include "src/Screens.h" //All screen functions and variables
#include "src/Menus.h" //All menu functions and variables
#include "src/DatesTime.h" //All date & time functions and variables
#include "src/Irrigation.h" //All irrigation related functions and variables
#include "src/Timers.h" //All timer related functions and variables

//OS main setup
void setup()
{
	lcd.createChar(0, upArrow);
	lcd.createChar(1, downArrow);
	lcd.begin(16, 2);
	pixels.begin(); // This initializes the NeoPixel library.
	pixels.setBrightness(128);
	pixels.show();

	//If SD Card is not found, we can't proceed
	if (!SD.begin(53)){
		lcd.print(F("SD Card Required"));
		lcd.setCursor(0, 1);
		lcd.print(F("Insert And Reset"));
		screenName = F("REQUIREDSD");
	} else {//if SD card is found, proceed with crop load or setup
		rtc.begin(); //Realtime Clock setup AFTER having built and loaded crop
		captureDateTime(); //Capture current time from real time clock
		phRsvrMillis = phPlantMillis = ecMillis = millis(); //set first time use of timestamps

		//Setup Flow Sensor Pins
		pinMode(FlowPinIn, INPUT);	//irrigation "in" flow meter
		digitalWrite(FlowPinIn, HIGH);

		pinMode(FlowPinOut, INPUT);	//irrigation "out" flow meter
		digitalWrite(FlowPinOut, HIGH);

		//irrigation flow meters being hooked into flow counter methods
		attachInterrupt(digitalPinToInterrupt(FlowPinIn), countRsvrFill, RISING);
		attachInterrupt(digitalPinToInterrupt(FlowPinOut), countRsvrDrain, RISING);

		//Setup Relay Pins
		pinMode(RELAY1, OUTPUT);	//perstaltic pump 1
		digitalWrite(RELAY1, HIGH);
		pinMode(RELAY2, OUTPUT);	//perstaltic pump 2
		digitalWrite(RELAY2, HIGH);
		pinMode(RELAY3, OUTPUT);	//perstaltic pump 3
		digitalWrite(RELAY3, HIGH);
		pinMode(RELAY4, OUTPUT);	//perstaltic pump 4
		digitalWrite(RELAY4, HIGH);
		pinMode(RELAY5, OUTPUT);	//perstaltic pump 5
		digitalWrite(RELAY5, HIGH);
		pinMode(RELAY6, OUTPUT);	//perstaltic pump 6
		digitalWrite(RELAY6, HIGH);
		pinMode(RELAY7, OUTPUT);	//perstaltic pump 7
		digitalWrite(RELAY7, HIGH);
		pinMode(RELAY8, OUTPUT);	//perstaltic pump 8
		digitalWrite(RELAY8, HIGH);
		pinMode(RELAY9, OUTPUT);	//perstaltic pump 9
		digitalWrite(RELAY9, HIGH);
		pinMode(RELAY10, OUTPUT);	//perstaltic pump 10
		digitalWrite(RELAY10, HIGH);

		pinMode(RELAY11, OUTPUT);	//irrigation "in" valve
		digitalWrite(RELAY11, HIGH);
		pinMode(RELAY12, OUTPUT);	//irrigation "out" valve
		digitalWrite(RELAY12, HIGH);

		pinMode(RELAY13, OUTPUT);	//High voltage power receptical 1
		digitalWrite(RELAY13, HIGH);
		pinMode(RELAY14, OUTPUT);	//High voltage power receptical 2
		digitalWrite(RELAY14, HIGH);
		pinMode(RELAY15, OUTPUT);	//High voltage power receptical 3
		digitalWrite(RELAY15, HIGH);
		pinMode(RELAY16, OUTPUT);	//High voltage power receptical 4
		digitalWrite(RELAY16, HIGH);

		//Serial.begin(9600);
		Wire.begin();   // enable i2c ports.
		coreInit();		//Loads or Creates Crops
	}
}

//Our runtime loop function
void loop()
{
	Key = analogRead(0);
	
	//Reset home screen and menu timers to current miliseconds after any interaction with LCD keys
	if (Key >= 0 && Key <= 650){
		homeMillis = menuMillis = millis();
	}
	//If no menu interactions have happened after 10 seconds
	if ((millis() - menuMillis) >= 10000){

		//OS monitors flowRates of both irrigation directions
		if ((millis() - flowMillis) >= 1000){
			checkFlowRates();
			if (screenName == "RSVRVOL"){
				printReservoirVolume();
			}
		}

		//Home print, PH corrections and EC corrections
		if ((millis() - homeMillis) >= 1000){
			if (screenName == "") {
				if (cropStatus == 1){
					correctPlantPH(); //check if plant PH correction is needed
					correctPlantEC(); //check if plant EC correction is needed
					correctRsvrPH(); //check if reservoir PH correction is needed
					checkRegimenDosing(); //check if we are ready for a regimen dosing (full or topoff)
				}

				if ((millis() - homeMillis) >= 2000) {
					printHomeScreen();	   //home print
					homeMillis = millis(); //home millis reset 
				}
			}
		}
	}



	//60 seconds has passed - Check logic for action
	if (previousMinute != rtc.getTime().min) {
		checkTimers();
		previousMinute = rtc.getTime().min;
	}

	//UI Menus
	if (Key == 0 || Key == 408){
		//Left & Right
		if (screenName == "DATETIME"){
			matrix = {
				{ { 1, 1 }, { 4, 4 }, { 10, 10 }, { 13, 13 } },
				{ { 3, 3 }, { 6, 6 }, { 13, 13 } }
			};
		}
		if (screenName == "NEW"){
			matrix = {
				{ { 0, 7 } },
				{ { 11, 11 } }
			};
			if (Key == 0){
				cropRename(NULL);
				lcd.blink();
			}
		}
		if (screenName == "OPEN"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 9, 9 } }
			};
		}
		if (screenName == "RESET"){
			matrix = {
				{ {0, 0} },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "DELETE") {}
		if (screenName == "STATUS") {
			matrix = {
				{ { 8, 8 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "ECRANGE") {
			matrix = {
				{ { 3, 3 }, { 8, 8 }, { 15, 15 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "PHRANGE"){
			matrix = {
				{ { 3, 3 }, {9, 9} },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PHDLY"){
			matrix = {
				{ { 15, 15 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PHAMNT") {
			matrix = {
				{ { 15, 15 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "RSVRVOL"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "REGIMENS"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PUMPCAL"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PUMPDLY"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "AMOUNT"){
			matrix = {
				{ { 12, 12 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "TPFCCNT"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "TPFAMNT"){
			matrix = {
				{ { 15, 15 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "TPFDLY"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "DRNTIME"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PRIME"){
			matrix = {
				{ { 0, 0 } },
				{ { 11, 11 } }
			};
		}
		if (screenName == "STARTEND"){
			matrix = {
				{ { 0, 0 }, { 3, 3 }, { 5, 5 }, { 8, 8 }, { 13, 13 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "RECEP01" || screenName == "RECEP02" || screenName == "RECEP03" || screenName == "RECEP04"){
			matrix = {
				{ { 1, 1 }, { 6, 6 }, { 11, 11 }, { 14, 14 }, },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "FLOWCAL"){
			matrix = {
				{ { 5, 5 }, { 13, 13 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PHCAL" || screenName == "PHCALLOW" || screenName == "PHCALHI"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "ECCAL" || screenName == "ECCALLOW" || screenName == "ECCALHI"){
			matrix = {
				{ { 10, 10 }, { 11, 11 }, { 12, 12 }, { 13, 13 }, { 14, 14 }, { 15, 15 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "MANFLUSH"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 5, 5 }, {11, 11} }
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
			printScreenNames(menus.front());
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
		if (screenName == "DATETIME"){
			if (cursorY == 0){
				printDateTime(dir);
			} else if (cursorX == 1){
				printDateTime(dir);
			}
		}
		if (screenName == "NEW"){
			cropRename(dir);
		}
		if (screenName == "OPEN"){
			if (cursorX == 0 && cursorY == 0){
				printOpen(dir);
			}
		}
		if (screenName == "DELETE") {}
		if (screenName == "STATUS") {
			if (cursorX == 8 && cursorY == 0){
				printStatus(dir);
			}
		}
		if (screenName == "REGIMENS"){
			if (cursorX == 1 && cursorY == 0){
				printRegimenNumber(dir);
			}
		}
		if (screenName == "PUMPCAL"){
			if (cursorX == 2 && cursorY == 0){
				printPumpCalibration(dir);
			}
		}
		if (screenName == "PUMPDLY"){
			if (cursorX == 2 && cursorY == 0){
				printPumpDelay(dir);
			}
		}
		if (screenName == "AMOUNT"){
			if (cursorX == 12 && cursorY == 0){
				printRegimenAmount(dir);
			}
		}
		if (screenName == "TPFCCNT") {
			if (cursorX == 0 && cursorY == 0){
				printTopOffConcentrate(dir);
			}
		}
		if (screenName == "TPFAMNT"){
			if (cursorX == 15 && cursorY == 0){
				printTopOffAmount(dir);
			}
		}
		if (screenName == "TPFDLY"){
			if (cursorX == 2 && cursorY == 0){
				printTopOffDelay(dir);
			}
		}
		if (screenName == "DRNTIME") {
			if (cursorX == 1 && cursorY == 0){
				printDrainTime(dir);
			}
		}
		if (screenName == "PRIME") {
			if (cursorX == 0 && cursorY == 0){
				primePump(dir);
			}
		}
		if (screenName == "STARTEND") {
			if (cursorX == 0 || cursorX == 3 || cursorX == 5 || cursorX == 8 || cursorX == 13 && cursorY == 0){
				printTimerStartEnd(dir);
			}
		}
		if (screenName == "RECEP01" || screenName == "RECEP02" || screenName == "RECEP03" || screenName == "RECEP04") {
			if (cursorX == 1 || cursorX == 6 || cursorX == 11 || cursorX == 14 && cursorY == 0){
				printTimerStartEnd(dir);
			}

		}
		if (screenName == "FLOWCAL") {
			if (cursorX == 5 || cursorX == 13 && cursorY == 0){
				printFlowCalibration(dir);
			}
		}
		if (screenName == "ECRANGE"){
			if (cursorX == 3 || cursorX == 8 || cursorX == 15 && cursorY == 0){
				printECRange(dir);
			}
		}
		if (screenName == "PHRANGE"){
			if (cursorX == 3 || cursorX == 9 && cursorY == 0){
				printPHRange(dir);
			}
		}
		if (screenName == "PHDLY") {
			if (cursorX == 15 && cursorY == 0){
				printPHDelay(dir);
			}
		}
		if (screenName == "PHAMNT") {
			if (cursorX == 15 && cursorY == 0){
				printPHAmount(dir);
			}
		}
		if (screenName == "ECCAL" || screenName == "ECCALLOW" || screenName == "ECCALHI"){
			if ((cursorX >=10 || cursorX <=15) && cursorY == 0){
				if (screenName == "ECCAL"){
					printECCalibrations("DRY", dir);
				}
				if (screenName == "ECCALLOW"){
					printECCalibrations("LOW", dir);
				}
				if (screenName == "ECCALHI"){
					printECCalibrations("HIGH", dir);
				}
			}
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
			tmpFile = SD.open("dromatic/" + cropName + "/" + getMenuHistory() + "/" + menus[menuIndex]);
			menusHistory.push_back(menus[menuIndex]);
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			if (menus.size() > 0){
				menuIndex = 0;
				tmpFile.close();
				printScreenNames(menus.front());
				printScrollArrows();
			}
			else {
				screenName = menusHistory.back();
				lcd.blink();
				lcd.home();
				if (screenName == "DATETIME"){
					captureDateTime();
					printDateTime();
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "NEW"){
					menus.clear();
					menusHistory.clear();
					currentAlphaIndex = 0;
					currentPumpIndex = 1;
					currentRegimenIndex = 1;
					menuIndex = 0;
					cursorX = cursorY = 0;
					cropCreate();
				}
				if (screenName == "OPEN"){
					printOpen();
				}
				if (screenName == "RESET"){
					printReset();
				}

				if (screenName == "STATUS") {
					printStatus();
				}
				if (screenName == "RSVRVOL"){
					menuMillis = 9000; //bypasses the 10second menuMillis wait period
					tmpFloats[0] = 0;
				}
				if (screenName == "REGIMENS"){
					cursorX = 1;
					cursorY = 0;
					StaticJsonBuffer<cropBufferSize> buffer;
					JsonObject& data = getCropData(buffer);
					tmpInts[0] = data["maxReg"];
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" REGIMEN DOSES"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "PUMPCAL"){
					tmpInts[0] = pumpCalibration;
					printPumpCalibration();
				}
				if (screenName == "PUMPDLY"){
					tmpInts[0] = pumpDelay;
					printPumpDelay();
				}
				if (screenName == "AMOUNT"){
					printRegimenAmount();
				}
				if (screenName == "TPFCCNT"){
					tmpInts[0] = topOffConcentrate;
					printTopOffConcentrate();
				}
				if (screenName == "TPFAMNT"){
					tmpInts[0] = topOffAmount;
					printTopOffAmount();
				}
				if (screenName == "TPFDLY"){
					tmpInts[0] = topOffDelay;
					printTopOffDelay();
				}
				if (screenName == "DRNTIME"){
					tmpInts[0] = drainTime;
					printDrainTime();
				}
				if (screenName == "FLOWCAL"){
					tmpFloats[0] = flowMeterConfig[0];
					tmpFloats[1] = flowMeterConfig[1];
					printFlowCalibration();
				}
				if (screenName == "PRIME"){
					lcd.write(byte(0));
					lcd.print(F(" TO PRIME CH0"));
					lcd.print(String(currentPumpIndex));
					lcd.setCursor(0, 1);
					lcd.print(F("          <done>"));
					cursorX = 0;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "STARTEND"){
					StaticJsonBuffer<timerBufferSize> buffer;
					JsonObject& data = getTimerSessionData(buffer);
					JsonArray& start = data["start"].asArray();
					JsonArray& end = data["end"].asArray();

					tmpInts[0] = start[0];
					tmpInts[1] = start[1];
					tmpInts[2] = start[2];
					tmpInts[3] = end[0];
					tmpInts[4] = end[1];
					tmpInts[5] = end[2];

					String startAMPM = (tmpInts[2] == 0) ? F("AM") : F("PM");
					String endAMPM = (tmpInts[2] == 0) ? F("AM") : F("PM");

					lcd.print(String(tmpInts[0]) + F(":") + String(tmpInts[1]) + startAMPM + F("-") + String(tmpInts[3]) + F(":") + String(tmpInts[4]) + endAMPM);
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 3;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "RECEP01" || screenName == "RECEP02" || screenName == "RECEP03" || screenName == "RECEP04"){
					String startDisplay, endDisplay;
					String AMPM1, AMPM2;
					StaticJsonBuffer<timerSessionBufferSize> buffer;
					if (currentTimerSessionIndex < 1){ currentTimerSessionIndex = 1; }
					JsonObject& data = getTimerSessionData(buffer, currentTimerIndex, currentTimerSessionIndex);

					JsonArray& times = data["times"];

					tmpInts[0] = times[0][0]; //start hour
					tmpInts[1] = times[0][1]; //end hour

					//start
					if (cursorX == 1){
						if (tmpInts[0] > 23){
							tmpInts[0] = 0;
						}
						if (tmpInts[0] < 0) {
							tmpInts[0] = 23;
						}
					}

					if (tmpInts[0] > 12){
						startDisplay = ((tmpInts[0] - 12) < 10) ? "0" + String(tmpInts[0] - 12) : String(tmpInts[0] - 12);
					}
					else{
						if (tmpInts[0] == 0){
							startDisplay = String(12);
						}
						else{
							startDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
						}
					}

					//END
					if (cursorX == 6){
						if (tmpInts[1] > 23){
							tmpInts[1] = 0;
						}
						if (tmpInts[1] < 0) {
							tmpInts[1] = 23;
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

					AMPM1 = (tmpInts[0] > 11) ? "PM" : "AM";
					AMPM2 = (tmpInts[1] > 11) ? "PM" : "AM";

					lcd.print(startDisplay);
					lcd.print(AMPM1);
					lcd.print(F("-"));
					lcd.print(endDisplay);
					lcd.print(AMPM2);
					lcd.print(F(" Su/01"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 6;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "ECRANGE"){
					printECRange(0); //0 = first print version
				}
				if (screenName == "PHRANGE"){
					printPHRange(0); //0 = first print version
				}
				if (screenName == "PHAMNT") {
					tmpFloats[0] = phAmount;
					printPHAmount();
				}
				if (screenName == "PHDLY") {
					tmpInts[0] = phDelay;
					printPHDelay();
				}
				if (screenName == "ECCAL"){
					printECCalibrations("DRY");
				}
				if (screenName == "PHCAL"){
					printPHCalibrations("LOW", 4);
				}
				if (screenName == "MANFLUSH"){
					printFullFlushing();
				}
			}
			delay(350);
		}
		
		//Saves
		if (screenName == "DATETIME"){
			saveDateTime();
		}
		if (screenName == "NEW"){
			if (cursorX == 11 && cursorY == 1){
				String nameConfirm;
				for (int i = 0; i < 15; i++){
					nameConfirm = nameConfirm + nameArry[i];
				}
				if (nameConfirm == ""){
					lcd.clear();
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
					cropBuild();
				}
			}
		}
		if (screenName == "OPEN"){
			cropChange();
		}
		if (screenName == "RESET"){
			cropReset();
		}
		//if (screenName == "DELETE") {}

		if (screenName == "STATUS") {
			saveStatus();
		}
		if (screenName == "RSVRVOL") {
			saveReservoirVolume();
		}
		if (screenName == "REGIMENS"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				StaticJsonBuffer<cropBufferSize> buffer;
				JsonObject& data = getCropData(buffer);

				if (data["maxReg"] > tmpInts[0]){ //we are trimming sessions
					lcd.print(F("TRIM REGIMENS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					trimRegimens(data["maxReg"], tmpInts[0]);
				}
				else if (data["maxReg"] < tmpInts[0]){ //we are adding sessions
					lcd.print(F("ADDING REGIMENS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					addRegimens(data["maxReg"], tmpInts[0]);
				}
				data["maxReg"] = tmpInts[0]; //update pump's session total
				maxRegimens = tmpInts[0];
				setCropData(data, false);
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "PUMPCAL"){
			savePumpCalibration();
		}
		if (screenName == "PUMPDLY"){
			savePumpDelay();
		}
		if (screenName == "AMOUNT"){
			saveRegimenAmount();
		}
		if (screenName == "TPFCCNT"){
			saveTopOffConcentrate();
		}
		if (screenName == "TPFAMNT"){
			saveTopOffAmount();
		}
		if (screenName == "TPFDLY") {
			saveTopOffDelay();
		}
		if (screenName == "FLOWCAL"){
			saveFlowCalibration();
		}
		if (screenName == "DRNTIME") {
			saveDrainTime();
		}
		if (screenName == "STARTEND"){
			saveStartEnd();
		}
		if (screenName == "RECEP01" || screenName == "RECEP02" || screenName == "RECEP03" || screenName == "RECEP04") {
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				StaticJsonBuffer<timerSessionBufferSize> saveBuffer;
				JsonObject& saveData = getTimerSessionData(saveBuffer, currentTimerIndex, currentTimerSessionIndex);

				saveData["times"].asArray()[currentTimerSessionDayIndex].asArray()[0] = tmpInts[0]; //start hour
				saveData["times"].asArray()[currentTimerSessionDayIndex].asArray()[1] = tmpInts[1]; //end hour
				setTimerSessionData(saveData);
				checkTimers();
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = tmpInts[1] = tmpInts[2] = tmpInts[3] = 0;
				exitScreen();
			}
		}
		if (screenName == "PRIME"){
			if (cursorX == 11 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "ECRANGE"){
			saveECRange();
		}
		if (screenName == "PHRANGE"){
			savePHRange();
		}
		if (screenName == "PHCAL"){
			if (cursorX == 1 && cursorY == 1){ //back
				exitScreen();
			}
			if (cursorX == 11 && cursorY == 1){ //forward
				setPHWaterProbeCalibration(112, 4.0, 'low'); //ph probe 1
				setPHWaterProbeCalibration(114, 4.0, 'low'); //ph probe 2
				printPHCalibrations("MID", 7);
				screenName = F("PHCALMID");
			}
		}
		if (screenName == "PHCALMID"){
			if (cursorX == 1 && cursorY == 1){ //back
				printPHCalibrations("LOW", 4);
				screenName = F("PHCALLOW");
			}
			if (cursorX == 11 && cursorY == 1){ //forward
				setPHWaterProbeCalibration(112, 7.0, 'mid'); //ph probe 1
				setPHWaterProbeCalibration(114, 7.0, 'mid'); //ph probe 2
				printPHCalibrations("HI", 10);
				screenName = F("PHCALHI");
			}
		}
		if (screenName == "PHCALHI"){
			if (cursorX == 1 && cursorY == 1){ //back
				printPHCalibrations("MID", 7);
				screenName = F("PHCALMID");
			}
			if (cursorX == 11 && cursorY == 1){ //forward
				setPHWaterProbeCalibration(112, 10.0, 'high'); //ph probe 1
				setPHWaterProbeCalibration(114, 10.0, 'high'); //ph probe 2
				lcd.clear();
				lcd.print(F("PH CALIBRATION"));
				lcd.setCursor(0, 1);
				lcd.print(F("NOW FINISHED!"));
				delay(5000);
				exitScreen();
			}
		}
		if (screenName == "PHAMNT"){
			savePHAmount();
		}
		if (screenName == "PHDLY"){
			savePHDelay();
		}
		if (screenName == "ECCAL"){
			if (cursorX == 11 && cursorY == 1){ //moving forward
				setECWaterProbeCalibration(111, tmpIntsToInt(5), 'dry'); //ec probe 1
				setECWaterProbeCalibration(113, tmpIntsToInt(5), 'dry'); //ec probe 2
				printECCalibrations("LOW");
				screenName = F("ECCALLOW");
			}
			if ((cursorX == 1 && cursorY == 1)){ //cancel calibration
				exitScreen();
			}
		}
		if (screenName == "ECCALLOW"){
			if (cursorX == 11 && cursorY == 1){ //going forward
				setECWaterProbeCalibration(111, tmpIntsToInt(5), 'low'); //ec probe 1
				setECWaterProbeCalibration(113, tmpIntsToInt(5), 'low'); //ec probe 2
				printECCalibrations("HIGH");
				screenName = F("ECCALHI");
			}
			if ((cursorX == 1 && cursorY == 1)){ //going back
				printECCalibrations("DRY");
				screenName = F("ECCAL");
			}
		}
		if (screenName == "ECCALHI"){
			if (cursorX == 11 && cursorY == 1){ //going forward
				tmpInts[0] = tmpInts[1] = tmpInts[2] = tmpInts[3] = tmpInts[4] = tmpInts[5] = 0; //reset tmpInts.
				setECWaterProbeCalibration(111, tmpIntsToInt(5), 'high'); //ec probe 1
				setECWaterProbeCalibration(113, tmpIntsToInt(5), 'high'); //ec probe 2
				lcd.clear();
				lcd.print(F("EC CALIBRATION"));
				lcd.setCursor(0, 1);
				lcd.print(F("NOW FINISHED!"));
				delay(5000);
				exitScreen();
			}
			if ((cursorX == 1 && cursorY == 1)){ //going back
				printECCalibrations("LOW");
				screenName = F("ECCALLOW");
			}
		}
		if (screenName == "MANFLUSH"){
			if (cursorX == 1 && cursorY == 1){
				RelayToggle(11, true);
				RelayToggle(12, false);
			}
			if (cursorX == 5 && cursorY == 1){
				RelayToggle(11, false);
				RelayToggle(12, true);
			}
			if (cursorX == 11 && cursorY == 1){
				RelayToggle(11, false);
				RelayToggle(12, false);
				exitScreen();
			}
		}
	}
}