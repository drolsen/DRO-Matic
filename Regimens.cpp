/*
*  DROMatic.ino
*  DROMatic OS Regimens
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Regimens.h"
#include "Core.h"
#include "Screens.h"
#include "Crops.h"
#include "Pumps.h"
#include "DatesTime.h"
#include "Irrigation.h"

byte currentRegimenIndex;
//Read & Write from SD
JsonObject& getRegimenData(JsonBuffer& b, byte pumpIndex = currentPumpIndex, byte sessionIndex = currentRegimenIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/pumps/syspmp" + pumpIndex + "/pmpse" + sessionIndex + ".dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setRegimenData(JsonObject& d, byte pumpIndex = currentPumpIndex, byte sessionIndex = currentRegimenIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/pumps/syspmp" + pumpIndex + "/pmpse" + sessionIndex + ".dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		printHomeScreen();
	}
}

//Prints
void printRegimenNumber(int dir){
	char instructions[15] = " REGIMEN DOSES";
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
void printRegimenAmount(int dir = 0){
	String amountDisplay = F("0000.0");
	StaticJsonBuffer<regimenBufferSize> sessionBuffer;
	JsonObject& data = getRegimenData(sessionBuffer, currentPumpIndex, 0);
	tmpFloats[0] = data["ml"];
	tmpInts[0] = 1;

	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& cropData = getCropData(cropBuffer);
	tmpInts[1] = cropData["maxRegimens"];

	if (dir != 0 && cursorX == 12){ 
		//only when changing value
		lcd.clear();
		if (dir > 0){
			tmpFloats[0] += .1;
		}else{
			tmpFloats[0] -= .1;
		}

		if (tmpFloats[0] < 0){
			tmpFloats[0] = 0;
		}
	}

	if (tmpFloats[0] > 1000){
		amountDisplay = F("");
	}
	else if (tmpFloats[0] > 100){
		amountDisplay = F("0");
	}
	else if (tmpFloats[0] > 10){
		amountDisplay = F("00");
	}
	else if (tmpFloats[0] > 0.1){
		amountDisplay = F("000");
	}
	else{
		amountDisplay = F("0000.00");
	}
	lcd.clear();
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

//Saves
void saveRegimenAmount(){
	if (cursorX == 11 && cursorY == 1){
		lcd.clear();
		lcd.home();
		lcd.print(F("SAVING REGIMEN"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEASE HOLD..."));
		//Save current session data
		StaticJsonBuffer<regimenBufferSize> regimenBuffer;
		JsonObject& sessionData = getRegimenData(regimenBuffer);

		sessionData["ml"] = tmpFloats[0];
		if (currentRegimenIndex > maxRegimens){
			StaticJsonBuffer<cropBufferSize> cropBuffer;
			JsonObject& cropData = getCropData(cropBuffer);

			maxRegimens = currentRegimenIndex;
			cropData["maxRegimens"] = currentRegimenIndex;
			setCropData(cropData);
		}
		setRegimenData(sessionData, currentPumpIndex, currentRegimenIndex, false);

		//Move on to next session
		if (tmpInts[0] < tmpInts[1]){
			tmpInts[0] += 1;
			currentRegimenIndex = tmpInts[0];
			cursorX = 12;
			cursorY = 0;
			printRegimenAmount();
		}
		else if (tmpInts[0] == tmpInts[1]){
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
			currentRegimenIndex = tmpInts[0];
			cursorX = 12;
			cursorY = 0;
			printRegimenAmount();
		}
	}
}

//Helpers
void addRegimens(int currentSize, int addAmount){
	byte i, j;
	String path;
	StaticJsonBuffer<regimenSessionBufferSize> buffer;
	JsonObject& data = buffer.createObject();
	data["ml"] = 80;
	data["expired"] = false;

	for (i = 1; i <= 8; i++){
		path = "dromatic/" + cropName + "/pumps/syspmp" + String(i);
		for (j = 0; j < addAmount; j++){
			if (j > currentSize){
				makeNewFile(path + j + ".dro", data);
			}
			Serial.flush();
		}
	}
}
void trimRegimens(int currentSize, int trimAmount){
	byte i, j;
	String path;
	i = 8; //number of pumps
	for (i = 1; i <= 8; i++){
		path = "dromatic/" + cropName + "/pumps/syspmp" + String(i);
		for (j = 0; j <= currentSize; j++){
			if (j > trimAmount-1){
				SD.remove(path + "/pmpse" + String(j) + ".dro");
			}
			Serial.flush();
		}
	}
}

//Regimen dosing functionality
void checkRegimenDosing(){
	//First, lets check to make sure we are even allowed to be within a dosing state
	//We can't be within a dosing state if any of the following are true:
	//1) currently filling reservoir with water
	//2) plant water is less than maxPPM for current regimen
	//3) we have remaining topOff water within reservoir
	if (flowInRate == true || feedingType == 2) { return; }

	StaticJsonBuffer<pumpBufferSize> pumpConfigBuffer;
	JsonObject& pumpConfig = getPumpsData(pumpConfigBuffer);

	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& cropData = getCropData(cropBuffer);

	lcd.clear();
	lcd.home();
	lcd.print(F("DOSING REGIMEN"));
	lcd.setCursor(0, 1);
	lcd.print(F("PLEASE HOLD!!"));
	lcd.home();
	for (byte i = 1; i <= 7; i++){
		//Second, lets open our SD data up and get this current regimen's ml dosing amount for this pump
		StaticJsonBuffer<regimenBufferSize> regimenBuffer;
		JsonObject& regimenData = getRegimenData(regimenBuffer, i, currentRegimen); //remember, this is a single pump instance (aka getPumpData)
		int amount = regimenData["ml"];
		int concentrate = (amount / 6) * topOffConcentrate;
		float ml = (feedingType == 0) ? (amount * rsvrVol) : (concentrate * rsvrVol);
		pumpSpin(ml, i, pumpCalibration); //perform dosing 

		//Third, if we have reached the end of our 7 available pumps
		//we need to update our crop settings to let OS know if current reservoir water is full feeding water, or topoff feeding water
		if (i == 7){
			//if this is a full feedingType dosing, we flush the whole
			if (feedingType == 0){ //we now must flush this water to our plants.
				Time current = rtc.getTime();
				flushPlantWater(); //first we must make sure all our poopy plant water is drained.
				flushRsvrWater(); //then after poopy water is out, we flush our freshly nute dosed water to plants.
				feedingType = 1;
				lastFeedingWeek = current.dow;
				lastFeedingDay = calcDayOfWeek(current.year, current.mon, current.date);
			}
			if (feedingType == 1){ feedingType = 2; } //this prevents topOff water from being dosed twice
			//now we have to update our currentRegimen records
			currentRegimen++; 
			currentRegimen = (currentRegimen >= maxRegimens) ? maxRegimens : currentRegimen;
			cropData["currentRegimen"] = currentRegimen;

			//lastly we flipping our global feedingType variable around so we are alternating between full feedings and topoff feedings

			cropData["feedingType"] = feedingType;
			setCropData(cropData);
		}else{
			byte pumpDelay = pumpConfig["delay"];
			int i = pumpDelay * 60; //mins x 60secs = loop total
			while (i--){ //count down total seconds
				delay(1000); //delay for 1 second each loop
			}
		}
	}
}