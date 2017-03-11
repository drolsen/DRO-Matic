/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <SPI.h> //Suppoting lib for SD card
#include <SD.h> //SD card API
#include <StandardCplusplus.h> //STD
#include <StandardCplusplus\vector> //Vectors
#include <StandardCplusplus\ctime> //Time helper
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)
#include <DS3231.h> //Real time clock lib
#include <ResponsiveAnalogRead.h> //Noise reduction on EC/PH sensors

#include "Globals.h" //All temp and PROGMEM global variables
#include "Core.h" //All core functions and variables
#include "Crops.h" //All crop functions and variables
#include "Channels.h" //All channel functions and variables
#include "Sessions.h" //All session functions and variables
#include "Screens.h" //All screen functions and variables
#include "Menus.h" //All menu functions and variables
#include "DatesTime.h" //All date & time functions and variables
#include "Irrigation.h" //All irrigation related functions and variables
#include "Timers.h" //All timer related functions and variables



//Our main setup function
void setup()
{
	lcd.createChar(0, upArrow);
	lcd.createChar(1, downArrow);
	lcd.createChar(3, infinityLeft);
	lcd.createChar(4, infinityRight);
	lcd.begin(16, 2);
	rtc.begin();
	captureDateTime();
	String relayName;
	pixels.begin(); // This initializes the NeoPixel library.
	

	//Setup Flow Metors
	pinMode(FlowPinIn, INPUT);
	pinMode(FlowPinOut, INPUT);
	attachInterrupt(1, Flow, RISING);

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
	pinMode(RELAY11, OUTPUT);
	pinMode(RELAY12, OUTPUT);
	pinMode(RELAY13, OUTPUT);
	pinMode(RELAY14, OUTPUT);
	pinMode(RELAY15, OUTPUT);
	pinMode(RELAY16, OUTPUT);

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
	digitalWrite(RELAY11, HIGH);
	digitalWrite(RELAY12, HIGH);
	digitalWrite(RELAY13, HIGH);
	digitalWrite(RELAY14, HIGH);
	digitalWrite(RELAY15, HIGH);
	digitalWrite(RELAY16, HIGH);

	//Serial.begin(9600);
	if (!SD.begin(53)){
		lcd.print(F("SD Card Required"));
		lcd.setCursor(0, 1);
		lcd.print(F("Insert And Reset"));
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

	//If user has given input, we reset timers to current miliseconds
	if (Key >= 0 && Key <= 650){
		homeMillis = currentMillis;
		menuMillis = currentMillis;
	}

	//60 seconds has passed - Check logic for action
	if (previousMinute != rtc.getTime().min) {
		if (screenName == "") {
			previousMinute = rtc.getTime().min;
			//turing();
		}
	}
	
	//10 seconds has passed - Return to home screen
	if ((currentMillis - homeMillis) >= 2000 && (currentMillis - menuMillis >= 10000)) {
		if (screenName == ""){
			homeMillis = currentMillis;
			printHomeScreen();
		}
	}

	//250 miliseconds has passed - Realtime UI Menus update
	if ((currentMillis - homeMillis) >= 250){
		if (screenName == "PHCAL"){
			float PH1Reading = (PH1Analog.getValue() * 14.0 / 1024);
			float PH2Reading = (PH2Analog.getValue() * 14.0 / 1024);
			String PH1Display, PH2Display;

			PH1Display = (PH1Reading > 10) ? String(PH1Reading) : "0" + String(PH1Reading);
			PH2Display = (PH2Reading > 10) ? String(PH2Reading) : "0" + String(PH2Reading);

			lcd.clear();
			lcd.home();
			lcd.print(PH1Display + String("pH ") + PH2Display + String("pH"));
			lcd.setCursor(0, 1);
			lcd.print(F("pH CALIB <done>"));
			lcd.setCursor(cursorX, cursorY);
			lcd.blink();
			homeMillis = currentMillis;
		}
		if (screenName == "ECCAL"){
			int EC1Reading = (EC1Analog.getValue() * PPMHundredth);
			int EC2Reading = (EC2Analog.getValue() * PPMHundredth);
			String EC1Display, EC2Display;

			if (EC1Reading > 10000){
				EC1Display = String(EC1Reading) + F("EC");
			}
			else if (EC1Reading > 1000){
				EC1Display = "0" + String(EC1Reading) + F("EC");
			}
			else if (EC1Reading > 100){
				EC1Display = "00" + String(EC1Reading) + F("EC");
			}
			else if (EC1Reading > 10){
				EC1Display = "000" + String(EC1Reading) + F("EC");
			}
			else if (EC1Reading > 1){
				EC1Display = "0000" + String(EC1Reading) + F("EC");
			}
			else {
				EC1Display = F("00000EC");
			}

			if (EC2Reading > 10000){
				EC2Display = String(EC2Reading) + F("EC");
			}
			else if (EC2Reading > 1000){
				EC2Display = "0" + String(EC2Reading) + F("EC");
			}
			else if (EC2Reading > 100){
				EC2Display = "00" + String(EC2Reading) + F("EC");
			}
			else if (EC2Reading > 10){
				EC2Display = "000" + String(EC2Reading) + F("EC");
			}
			else if (EC2Reading > 1){
				EC2Display = "0000" + String(EC2Reading) + F("EC");
			}
			else {
				EC2Display = F("00000EC");
			}

			lcd.clear();
			lcd.home();
			lcd.print(EC1Display + String(" ") + EC2Display);
			lcd.setCursor(0, 1);
			lcd.print(String(((int)PPMHundredth * 100)) + F("PPM    <done>"));
			lcd.setCursor(cursorX, cursorY);
			lcd.blink();
			homeMillis = currentMillis;
		}
		if (screenName == "RSVRVOL"){
			interrupts();   //Enables interrupts on the Arduino
			noInterrupts(); //Disable the interrupts on the Arduino

			InFlowRate = (tmpFlowCount * 2.25);
			float liters = tmpFlowCount / 1000;
			float USgallons = tmpFlowCount / 4546.091879;
			float UKgallons = USgallons * 0.83267384;

			lcd.clear();
			lcd.home();
			lcd.print(USgallons, 1);
			lcd.print("lqd/");
			lcd.print(UKgallons, 1);
			lcd.print("gal");
			lcd.setCursor(0, 1);
			lcd.print(F("<back>      <ok>"));
			lcd.setCursor(cursorX, cursorY);
			lcd.blink();
			homeMillis = currentMillis;
		}
	}

	//Update noise reduction analog readings on EC/PH sensors
	PH1Analog.update();
	PH2Analog.update();
	EC1Analog.update();
	EC2Analog.update();

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
		if (screenName == "RESET"){
			matrix = {
				{ {2, 2} },
				{ { 1, 1 }, { 9, 9 } }
			};
		}
		if (screenName == "DELETE") {}
		if (screenName == "EC") {
			matrix = {
				{ { 3, 3 }, { 8, 8 }, { 15, 15 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "PH"){
			matrix = {
				{ { 3, 3 }, {9, 9} },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "RSVRVOL"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "DOSES"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CALIB"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 9, 9 }, { 12, 12 } }
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
		if (screenName == "TPFVOL"){
			matrix = {
				{ { 0, 0 } },
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
				{ { 1, 1 }, { 3, 3 }, { 6, 6 }, {8, 8}, { 11, 11 }, { 14, 14 }, },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "FLOWCAL"){
			matrix = {
				{ { 5, 5 }, { 13, 13 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PHCAL"){
			matrix = {
				{ { 0, 0 } },
				{ { 11, 11 } }
			};
		}
		if (screenName == "ECCAL"){
			matrix = {
				{ { 4, 4 }, { 12, 12} },
				{ { 2, 2 }, { 11, 11 } }
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
		if (screenName == "DATETIME"){
			setDateTime(dir);
		}
		if (screenName == "NEW"){
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
		if (screenName == "RESET") {}
		if (screenName == "DELETE") {}
		if (screenName == "RSVRVOL") {
			setReservoirVolume(dir);
		}
		if (screenName == "DOSES"){
			setSessionNumber(dir);
		}
		if (screenName == "CALIB"){
			setCalibrationSize(dir);
		}
		if (screenName == "AMOUNT"){
			setSessionAmount(dir);
		}
		if (screenName == "TPFCCNT") {
			setTopOffConcentrate(dir);
		}
		if (screenName == "TPFVOL") {
			setTopOffVolume(dir);
		}
		if (screenName == "DRNTIME") {
			setDrainTime(dir);
		}
		if (screenName == "PRIME") {
			primeChannelPump(dir);
		}
		if (screenName == "STARTEND") {
			setTimerStartEnd(dir);
		}
		if (screenName == "RECEP01" || screenName == "RECEP02" || screenName == "RECEP03" || screenName == "RECEP04") {
			setTimerStartEnd(dir);
		}
		if (screenName == "FLOWCAL") {
			setFlowMeterCalibration(dir);
		}
		if (screenName == "EC"){
			setECRange(dir);
		}
		if (screenName == "PH"){
			setPHRange(dir);
		}
		if (screenName == "ECCAL"){
			if (cursorX == 2 && cursorY == 1){
				setPPMHundredth(dir);
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
				if (screenName == "NEW"){
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
				if (screenName == "RESET"){
					lcd.clear();
					lcd.home();
					lcd.print(F(" CONFIRM RESET "));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <reset>"));
					printScrollArrows();
				}
				if (screenName == "DELETE"){

				}
				if (screenName == "RSVRVOL"){
					tmpFlowCount = 0;
				}
				if (screenName == "DOSES"){
					cursorX = 1;
					cursorY = 0;
					StaticJsonBuffer<cropBufferSize> buffer;
					JsonObject& data = getCropData(buffer);
					tmpInts[0] = data["regimens"];
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" REGIMEN DOSES"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CALIB"){
					StaticJsonBuffer<channelBufferSize> buffer;
					JsonObject& data = getChannelData(buffer);
					tmpInts[0] = data["calibration"];
					tmpInts[1] = data["size"];
					lcd.print(String(tmpInts[0]) + F("00 (ml) per min"));
					cursorX = 2;

					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <ok|all>"));

					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "AMOUNT"){
					String amountDisplay;
					StaticJsonBuffer<regimenBufferSize> sessionBuffer;
					JsonObject& data = getSessionData(sessionBuffer);
					tmpFloats[0] = data["ml"];
					tmpInts[0] = 1;

					StaticJsonBuffer<cropBufferSize> cropBuffer;
					JsonObject& cropData = getCropData(cropBuffer);
					tmpInts[1] = cropData["regimens"];

					if (tmpFloats[0] > 1000){
						amountDisplay = F("");
					}else if (tmpFloats[0] > 100){
						amountDisplay = F("0");
					}else if (tmpFloats[0] > 10){
						amountDisplay = F("00");
					} else if (tmpFloats[0] > 0.1){
						amountDisplay = F("000");
					} else {
						amountDisplay = F("0000.00");
					}

					lcd.print(F("REGI "));
					lcd.print(tmpInts[0]);
					lcd.print(F(" "));
					lcd.print(amountDisplay);
					lcd.print(F("ml"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>    <next>"));
					cursorX = 12;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "TPFCCNT"){
					StaticJsonBuffer<irrigateBufferSize> buffer;
					JsonObject& data = getIrrigationData(buffer);
					tmpInts[0] = data["tpfccnt"];
					lcd.print(String(tmpInts[0]) + F("/6") + F(" CONCENTRATE"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 0;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "TPFVOL"){
					StaticJsonBuffer<irrigateBufferSize> buffer;
					JsonObject& data = getIrrigationData(buffer);
					tmpInts[0] = data["tpfvol"];
					lcd.print(String(tmpInts[0]) + F("lqd/gal"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 3;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "DRNTIME"){
					StaticJsonBuffer<irrigateBufferSize> buffer;
					JsonObject& data = getIrrigationData(buffer);
					tmpInts[0] = data["draintime"];
					lcd.print(F("0"));
					lcd.print(String(tmpInts[0]) + F(" Min(s) Drain"));
					lcd.setCursor(0, 2);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "FLOWCAL"){
					StaticJsonBuffer<irrigateBufferSize> buffer;
					JsonObject& data = getIrrigationData(buffer);
					tmpFloats[0] = data["flMeters"].asArray()[0];
					tmpFloats[1] = data["flMeters"].asArray()[1];
					lcd.print(F("IN"));
					lcd.print(String(tmpFloats[0]));
					lcd.print(F(" OUT"));
					lcd.print(String(tmpFloats[1]));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 5;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "PRIME"){
					lcd.write(byte(0));
					lcd.print(F(" TO PRIME CH0"));
					lcd.print(String(currentChannelIndex));
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

					JsonArray& startArray = data["start"];
					JsonArray& endArray = data["end"];

					tmpInts[0] = startArray[0][0]; //start hour
					tmpInts[1] = startArray[0][1]; //start am/pm
					tmpInts[2] = endArray[0][0]; //end hour
					tmpInts[3] = endArray[0][1]; //end am/pm

					if (tmpInts[0] >= 10){
						startDisplay = F("");
					}else{
						startDisplay = F("0");
					}

					if (tmpInts[2] >= 10){
						endDisplay = F("");
					}else{
						endDisplay = F("0");
					}

							if (tmpInts[2] >= 10){
			endDisplay = F("");
		}else{
			endDisplay = F("0");
		}

					AMPM1 = (tmpInts[1] > 0) ? "PM" : "AM";
					AMPM2 = (tmpInts[3] > 0) ? "PM" : "AM";

					lcd.print(startDisplay);
					lcd.print(tmpInts[0]);
					lcd.print(AMPM1);
					lcd.print(F("-"));
					lcd.print(endDisplay);
					lcd.print(tmpInts[2]);
					lcd.print(AMPM2);
					lcd.print(" Mo/01");
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 6;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "EC"){
					currentSessionIndex = 1;
					String EC1Display, EC2Display;
					StaticJsonBuffer<ecBufferSize> ecBuffer;
					JsonObject& ecData = getECData(ecBuffer, currentSessionIndex);
					tmpInts[0] = ecData["ec"].asArray()[0];
					tmpInts[1] = ecData["ec"].asArray()[1];
					tmpInts[2] = 1;

					if (tmpInts[0] >= 1000){
						EC1Display = "";
					}
					else if (tmpInts[0] >= 100){
						EC1Display = "0";
					}
					else if (tmpInts[0] >= 10){
						EC1Display = "00";
					}
					else if (tmpInts[0] >= 1){
						EC1Display = "000";
					}

					if (tmpInts[1] >= 1000){
						EC2Display = "";
					}
					else if (tmpInts[1] >= 100){
						EC2Display = "0";
					}
					else if (tmpInts[1] >= 10){
						EC2Display = "00";
					}
					else if (tmpInts[1] >= 1){
						EC2Display = "000";
					}

					lcd.print(EC1Display + String(tmpInts[0]));
					lcd.write(byte(1));
					lcd.print(EC2Display + String(tmpInts[1]));
					lcd.write(byte(0));

					lcd.print(F(" EC/01"));
					
					lcd.setCursor(0, 1);
					lcd.print(F("<back>    <done>"));
					cursorX = 3;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "PH"){
					StaticJsonBuffer<cropBufferSize> buffer;
					JsonObject& data = getCropData(buffer);
					tmpFloats[0] = data["ph"].asArray()[0];
					tmpFloats[1] = data["ph"].asArray()[1];
					lcd.print(String(tmpFloats[0]));
					lcd.write(byte(1));
					lcd.print(String(F(" ")) + String(tmpFloats[1]));
					lcd.write(byte(0));
					lcd.print(F(" PH"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "ECCAL"){
					cursorX = 4;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "PHCAL"){
					cursorX = 0;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
			}
			delay(350);
		}
		//Saves
		if (screenName == "DATETIME"){
			if (cursorX == 13 && cursorY == 1){
				//hour, min, seconds
				rtc.setTime(tmpInts[4], tmpInts[5], 0);
				//day, month (RTC counts first month as 1, not 0), year
				rtc.setDate(tmpInts[2], tmpInts[1] + 1, tmpInts[0]);
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				tmpDisplay[0] = ""; //suffix
				tmpDisplay[1] = ""; //hour
				tmpDisplay[2] = ""; //min
				tmpDisplay[3] = ""; //day
				exitScreen();
			}
		}
		if (screenName == "NEW"){
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
		if (screenName == "RESET"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.print(F("RESETTING"));
				lcd.setCursor(1, 0);
				lcd.print(F("  PLEASE HOLD  "));

				for (byte i = 1; i < 8; i++){
					StaticJsonBuffer<channelBufferSize> channelBuffer;
					JsonObject& channelData = getChannelData(channelBuffer, i);
					byte totalSessions = channelData["totalSessions"];

					for (byte j = 1; j < totalSessions; j++){
						StaticJsonBuffer<regimenBufferSize> regimenBuffer;
						JsonObject& regimenData = getSessionData(regimenBuffer, i, j);
						JsonArray& regimenDate = regimenData["date"].asArray();
						regimenData["repeated"] = 0;
						regimenData["expired"] = false;
						regimenDate[0] = rtc.getTime().year;
					}
				}
			}
			if (cursorX == 1 || cursorX == 9 && cursorY == 1){
				tmpInts[1] = tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "DELETE") {}
		if (screenName == "RSVRVOL") {
			if (cursorX == 11 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<irrigateBufferSize> jsonBuffer;
				JsonObject& data = getIrrigationData(jsonBuffer);
				data["rsvrvol"].asArray()[0] = tmpFlowCount;
				setIrrigationData(data);
			}
			if (cursorX == 1 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "DOSES"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<cropBufferSize> buffer;
				JsonObject& data = getCropData(buffer);

				if (data["regimens"] > tmpInts[0]){ //we are trimming sessions
					lcd.print(F("TRIM REGIMENS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					trimSessions(data["regimens"], tmpInts[0]);
				}
				else if (data["regimens"] < tmpInts[0]){ //we are adding sessions
					lcd.print(F("ADDING REGIMENS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					addSessions(data["regimens"], tmpInts[0]);
				}
				data["regimens"] = tmpInts[0]; //update channel's session total
				setCropData(data, false);
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CALIB"){
			if (cursorX == 9 && cursorY == 1){ //single channel save
				StaticJsonBuffer<channelBufferSize> channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);
				channelData["calibration"] = tmpInts[0];
				setChannelData(channelData, currentChannelIndex, false);
				tmpInts[0] = tmpInts[1] = 0;
				exitScreen();
			}
			if (cursorX == 12 && cursorY == 1){ //all channel save
				lcd.clear();
				lcd.home();
				lcd.print(F("  SAVING ALL  "));
				lcd.setCursor(0, 1);
				lcd.print(F("  PLEASE WAIT "));

				StaticJsonBuffer<64> cropBuffer;
				JsonObject& cropData = getCropData(cropBuffer);
				int channelSize = cropData["totalChannels"];
				while (channelSize--){
					StaticJsonBuffer<64> channelBuffer;
					JsonObject& channelData = getChannelData(channelBuffer, channelSize);
					channelData["calibration"] = tmpInts[0];
					setChannelData(channelData, channelSize, false);
				}
			}
			if (cursorX == 1 || cursorX == 9 || cursorX == 12 && cursorY == 1){
				tmpInts[0] = tmpInts[1] = 0;
				exitScreen();
			}
		}
		if (screenName == "AMOUNT"){
			if (cursorX == 11 && cursorY == 1){
				lcd.clear();
				lcd.home();
				lcd.print("SAVING...");
				//Save current session data
				StaticJsonBuffer<regimenBufferSize> saveBuffer;
				JsonObject& sessionData = getSessionData(saveBuffer);
				sessionData["ml"] = tmpFloats[0];
				setSessionData(sessionData, currentChannelIndex, currentSessionIndex, false);

				//Move on to next session
				if (tmpInts[0] < tmpInts[1]){

					tmpInts[0] += 1;
					currentSessionIndex = tmpInts[0];

					String amountDisplay;
					StaticJsonBuffer<regimenBufferSize> openBuffer;
					JsonObject& data = getSessionData(openBuffer);
					tmpFloats[0] = data["ml"];

					if (tmpFloats[0] > 1000){
						amountDisplay = F("");
					}else if (tmpFloats[0] > 100){
						amountDisplay = F("0");
					}else if (tmpFloats[0] > 10){
						amountDisplay = F("00");
					}else if (tmpFloats[0] > 0.1){
						amountDisplay = F("000");
					}else{
						amountDisplay = F("0000.00");
					}

					lcd.clear();
					lcd.home();
					lcd.print(F("WEEK "));
					lcd.print(tmpInts[0]);
					lcd.print(F(" "));
					lcd.print(amountDisplay);
					lcd.print(F("ml"));
					lcd.setCursor(0, 1);

					if (tmpInts[0] == 1){
						lcd.print(F("<back>    <next>"));
					}else if (tmpInts[0] < tmpInts[1]){
						lcd.print(F("<prev>    <next>"));
					}else{
						lcd.print(F("<prev>    <done>"));
					}
					cursorX = 12;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				else if(tmpInts[0] == tmpInts[1]){
					exitScreen();
				}
			}
			if (cursorX == 1 && cursorY == 1){
				if (tmpInts[0] == 1){
					tmpInts[0] = 0;
					exitScreen();
				}
				else{
					//Move on to previous session
					tmpInts[0] -= 1;
					currentSessionIndex = tmpInts[0];
					String amountDisplay;
					StaticJsonBuffer<regimenBufferSize> openBuffer;
					JsonObject& data = getSessionData(openBuffer);
					tmpFloats[0] = data["ml"];

					if (tmpFloats[0] > 1000){
						amountDisplay = F("");
					}else if (tmpFloats[0] > 100){
						amountDisplay = F("0");
					}else if (tmpFloats[0] > 10){
						amountDisplay = F("00");
					}else if (tmpFloats[0] > 0.1){
						amountDisplay = F("000");
					}else{
						amountDisplay = F("0000.00");
					}

					lcd.clear();
					lcd.home();
					lcd.print(F("WEEK "));
					lcd.print(tmpInts[0]);
					lcd.print(F(" "));
					lcd.print(amountDisplay);
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
					cursorX = 12;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
			}
		}
		if (screenName == "TPFCCNT"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<irrigateBufferSize> buffer;
				JsonObject& data = getIrrigationData(buffer);
				data["tpfccnt"] = tmpInts[0];
				setIrrigationData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "TPFVOL"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<irrigateBufferSize> buffer;
				JsonObject& data = getIrrigationData(buffer);
				data["tpfvol"] = tmpInts[0];
				setIrrigationData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "FLOWCAL"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<irrigateBufferSize> jsonBuffer;
				JsonObject& data = getIrrigationData(jsonBuffer);
				data["flMeters"].asArray()[0] = tmpFloats[0];
				data["flMeters"].asArray()[1] = tmpFloats[1];
				setIrrigationData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpFloats[0] = 0;
				tmpFloats[1] = 0;
				exitScreen();
			}
		}
		if (screenName == "DRNTIME") {
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<irrigateBufferSize> buffer;
				JsonObject& data = getIrrigationData(buffer);
				data["drntime"] = tmpInts[0];
				setIrrigationData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "STARTEND"){
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
		if (screenName == "RECEP01" || screenName == "RECEP02" || screenName == "RECEP03" || screenName == "RECEP04") {
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<timerSessionBufferSize> saveBuffer;
				JsonObject& saveData = getTimerSessionData(saveBuffer, currentTimerIndex, currentTimerSessionIndex);

				saveData["start"].asArray()[currentTimerSessionDayIndex].asArray()[0] = tmpInts[0]; //start hour
				saveData["start"].asArray()[currentTimerSessionDayIndex].asArray()[1] = tmpInts[1]; //start am/pm
				saveData["end"].asArray()[currentTimerSessionDayIndex].asArray()[0] = tmpInts[2]; //end hour
				saveData["end"].asArray()[currentTimerSessionDayIndex].asArray()[1] = tmpInts[3]; //end am/pm
				setTimerSessionData(saveData);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				tmpInts[1] = 0;
				tmpInts[2] = 0;
				tmpInts[3] = 0;
				exitScreen();
			}
		}
		if (screenName == "PRIME"){
			if (cursorX == 11 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "EC"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<ecBufferSize> ecBuffer;
				JsonObject& ecData = getECData(ecBuffer, tmpInts[2]);
				ecData["ec"].asArray()[0] = tmpInts[0];
				ecData["ec"].asArray()[1] = tmpInts[1];
				setECData(ecData, tmpInts[2]);

			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "PH"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<cropBufferSize> jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
				data["ph"].asArray()[0] = tmpFloats[0];
				data["ph"].asArray()[1] = tmpFloats[1];
				setCropData(data, false);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "PHCAL"){
			if (cursorX == 11 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "ECCAL"){
			if (cursorX == 11 && cursorY == 1){
				lcd.clear();
				lcd.home();
				StaticJsonBuffer<cropBufferSize> jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
				data["ppm"] = PPMHundredth;
				setCropData(data);
				exitScreen();
			}
		}
	}
}