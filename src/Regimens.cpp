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
JsonObject& getRegimenData(JsonBuffer& b, byte pumpIndex = currentPumpIndex, byte regimenIndex = currentRegimenIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/pumps/syspmp" + pumpIndex + "/pmpse" + regimenIndex + ".dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setRegimenData(JsonObject& d, byte pumpIndex = currentPumpIndex, byte regimenIndex = currentRegimenIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/pumps/syspmp" + pumpIndex + "/pmpse" + regimenIndex + ".dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		printHomeScreen();
	}
}

//Prints
void printRegimenNumber(int dir){
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	lcd.clear();
	if (tmpInts[0] < 10){ lcd.print(0); }

	lcd.print(tmpInts[0]);
	lcd.print(F(" REGIMEN DOSES"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}
void printRegimenAmount(int dir = 0){
	String amountDisplay = F("0000.0");
	StaticJsonBuffer<regimenBufferSize> sessionBuffer;
	JsonObject& data = getRegimenData(sessionBuffer, currentPumpIndex, 1);
	tmpFloats[0] = data["ml"];
	tmpInts[0] = 1;

	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& cropData = getCropData(cropBuffer);
	tmpInts[1] = cropData["maxReg"];

	if (dir != 0 && cursorX == 12){ 
		//only when changing value
		lcd.clear();

		if (dir > 0){
			tmpFloats[0] += .1;
		}

		if (dir < 0){
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
			cropData["maxReg"] = currentRegimenIndex;
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
	StaticJsonBuffer<regimenSessionBufferSize> buffer;
	JsonObject& data = buffer.createObject();
	data["ml"] = 80;

	for (byte i = 1; i <= 8; i++){
		for (byte j = 0; j < addAmount; j++){
			if (j > currentSize){
				makeNewFile("dromatic/" + cropName + "/pumps/syspmp" + String(i) + j + ".dro", data);
			}
			Serial.flush();
		}
	}
}
void trimRegimens(int currentSize, int trimAmount){
	for (byte i = 1; i <= 8; i++){
		for (byte j = 0; j <= currentSize; j++){
			if (j > trimAmount-1){
				SD.remove("dromatic/" + cropName + "/pumps/syspmp" + String(i) + "/pmpse" + String(j) + ".dro");
			}
			Serial.flush();
		}
	}
}

void moveToNextRegimen(){
	lcd.clear();
	lcd.print(F("MOVING ONTO"));
	lcd.setCursor(0, 1);
	currentRegimen = ((currentRegimen+1) >= maxRegimens) ? currentRegimen : (currentRegimen+1);
	lcd.print(currentRegimen);
	lcd.print(F(" REGIMEN"));
	
	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& cropData = getCropData(cropBuffer);
	cropData["currentReg"] = currentRegimen;
	cropData["feedType"] = feedingType = 0;
	setCropData(cropData);

	//load new EC ranges
	StaticJsonBuffer<ecBufferSize> ecBuffer;
	JsonObject& ECData = getECData(ecBuffer, currentRegimen);
	minPPM = ECData["ec"].asArray()[0];
	maxPPM = ECData["ec"].asArray()[1];
}

//Regimen dosing functionality
void checkRegimenDosing(){
	if (flowInRate > 0.05 || feedingType == 2) { return; } //if we have a flowInRate, we can't proceed.
	if (((millis() - phRsvrMillis) < (phDelay * 60))){ return; } //if we have not waited longer enough since last pH adjustment, we can't proceed. 
	float pH = getPHProbeValue(RSVRPH);
	if (pH > maxPH || pH < minPH) { return; } //if we still have a pH lower or higher than configured range, we can't proceed.

	StaticJsonBuffer<pumpBufferSize> pumpConfigBuffer;
	JsonObject& pumpConfig = getPumpsData(pumpConfigBuffer);

	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& cropData = getCropData(cropBuffer);

	for (byte i = 1; i <= 7; i++){
		lcd.clear();
		if (feedingType == 0){
			lcd.print(F("DOSING REGIMEN"));
		}
		else if (feedingType == 1){
			lcd.print(F("DOSING TOPOFF"));
		}
		lcd.setCursor(0, 1);
		lcd.print(F("RSVR EC:"));
		lcd.print(getECProbeValue(RSVREC));
		lcd.home();

		//Get current regimen's ml dosing amount for this pump
		StaticJsonBuffer<regimenBufferSize> regimenBuffer;
		JsonObject& regimenData = getRegimenData(regimenBuffer, i, currentRegimen); //remember, this is a single pump instance (aka getPumpData)
		float amount = regimenData["ml"];
		float concentrate = (amount / 6) * topOffConcentrate;
		float ml = (feedingType == 0) ? (amount * rsvrVol) : (concentrate * rsvrVol);
		pumpSpin(ml, i); //perform dosing 

		//Have reached the end of our 7 available pumps?
		//We need to update our crop settings to let OS know if current reservoir water is full feeding water, or topoff feeding water
		if (i == 7){
			//if this is a full feedingType dosing, we flush the whole
			if (feedingType == 0){	//only while under full feeding type do we flush entire batch of water to plants
				drainPlants(drainTime);	//flush all exsisting poopy plant water
				feedPlants();	//next, we feed our entire batch of freshly dosed water to plants.
				cropData["feedType"] = feedingType = 1;	//lastly we progress crop to next feeding type to being top off dosing
			} else if (feedingType == 1) { 
				cropData["feedType"] = feedingType = 2; 
			}
			setCropData(cropData); //save
		}else{
			//Delay logic between each pump's dosing
			byte pumpDelay = pumpConfig["delay"];
			int i = pumpDelay * 60; //mins x secs = loop total
			while (i--){ //count down total seconds
				lcd.clear();
				if (i < 10){
					lcd.print(F("00"));
				}else if (i < 100){
					lcd.print(F("0"));
				}
				lcd.print(i);
				lcd.print(F(" SECOND DELAY"));
				lcd.setCursor(0, 1);
				lcd.print(F("PLEASE HOLD!!"));
				lcd.home();

				delay(1000); //delay for 1 second each loop
				Serial.flush();
			}
		}
	}
}