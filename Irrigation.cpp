/*
*  DROMatic.ino
*  DROMatic OS Irrigation
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Irrigation.h"
#include "Core.h"
#include "Crops.h"
#include "Screens.h"
#include "Regimens.h"

//Read & Write from SD
JsonObject& getIrrigationData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/irrigate.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}
void setIrrigationData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/irrigate.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

//Prints
void printReservoirVolume(int dir){
	tmpInts[0] += dir;
	String rsvrVol;
	if (tmpInts[0] > 9999){ //max
		rsvrVol = F("9999");
		tmpInts[0] = 9999;
	} else if(tmpInts[0] < 1000) {
		rsvrVol = F("0");
	} else if (tmpInts[0] < 100){
		rsvrVol = F("00");
	} else if (tmpInts[0] < 10){
		rsvrVol = F("000");
	} else if (tmpInts[0] < 0){ //min
		rsvrVol = F("0000");
		tmpInts[0] = 0;
	}

	lcd.clear();
	lcd.print(rsvrVol);
	lcd.print(tmpInts[0]);
	lcd.print(F(" Gallons"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}
void printTopOffConcentrate(int dir = 0){
	String fraction;
	if (dir != 0){
		tmpInts[0] += dir;
		//min - max prevention
		if (tmpInts[0] > 5) {
			tmpInts[0] = 5;
		}else if (tmpInts[0] < 1){
			tmpInts[0] = 1;
		}
	}

	if (tmpInts[0] == 1) {
		lcd.print(F("1/6 CONCENTRATE"));
	}
	if (tmpInts[0] == 2) {
		lcd.print(F("2/6 CONCENTRATE"));
	}
	if (tmpInts[0] == 3) {
		lcd.print(F("3/6 CONCENTRATE"));
	}
	if (tmpInts[0] == 4) {
		lcd.print(F("4/6 CONCENTRATE"));
	}
	if (tmpInts[0] == 5) {
		lcd.print(F("5/6 CONCENTRATE"));
	}
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}
void printTopOffAmount(int dir = 0){
	if (dir != 0){
		tmpInts[0] += dir;
	}
	lcd.clear();
	lcd.print(tmpInts[0]);
	lcd.print(F(" TOPOFF GAL"));
	lcd.setCursor(0,1);
	lcd.print(F("<back>      <ok>"));
}
void printTopOffDelay(int dir = 0){
	if (dir != 0){
		tmpInts[0] += dir;
		tmpInts[0] = (tmpInts[0] >= 99) ? 99 : (tmpInts[0] <= 1) ? 1 : tmpInts[0];
	}
	lcd.clear();
	lcd.print(tmpInts[0]);
	lcd.print(F(" TOPOFF DELAY"));
	lcd.setCursor(0,1);
	lcd.print(F("<back>      <ok>"));
}
void printDrainTime(int dir = 0){
	String drainTime;
	if (dir != 0){
		tmpInts[0] += dir;
	}
	if (tmpInts[0] > 9999){ //max
		drainTime = F("9999");
		tmpInts[0] = 9999;
	}
	else if (tmpInts[0] < 1000) {
		drainTime = F("0");
	}
	else if (tmpInts[0] < 100){
		drainTime = F("00");
	}
	else if (tmpInts[0] < 10){
		drainTime = F("000");
	}
	else if (tmpInts[0] < 0){ //min
		drainTime = F("0000");
		tmpInts[0] = 0;
	}
	lcd.clear();
	lcd.print(drainTime);
	lcd.print(tmpInts[0]);
	lcd.print(F(" Min(s) Drain"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}
void printFlowCalibration(int dir = 0){
	if (dir != 0){
		if (cursorX == 5 && cursorY == 0){
			if (dir > 0){
				tmpFloats[0] += .05;
			}
			else{
				tmpFloats[0] -= .05;
			}
		}
		if (cursorX == 13 && cursorY == 0){
			if (dir > 0){
				tmpFloats[1] += .05;
			}
			else{
				tmpFloats[1] -= .05;
			}
		}
	}
	lcd.clear();
	lcd.print(F("IN"));
	lcd.print(tmpFloats[0]);
	lcd.print(F(" OUT"));
	lcd.print(tmpFloats[1]);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}
void printFullFlushing(){
	lcd.print(F("MANUAL FLUSHING"));
	lcd.setCursor(0, 1);
	lcd.print(F("<in><out><done>"));
	cursorX = 1;
	cursorY = 0;
	lcd.setCursor(cursorX, cursorY);
}

//Saves
void saveReservoirVolume(){
	if (cursorX == 11 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<irrigateBufferSize> jsonBuffer;
		JsonObject& data = getIrrigationData(jsonBuffer);
		data["rsvrvol"].asArray()[0] = rsvrVol = tmpFlowCount;
		setIrrigationData(data);
	}
	if (cursorX == 1 && cursorY == 1){
		exitScreen();
	}
}
void saveTopOffConcentrate(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<irrigateBufferSize> buffer;
		JsonObject& data = getIrrigationData(buffer);
		data["tpfccnt"] = topOffConcentrate = tmpInts[0];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}
void saveTopOffAmount(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<irrigateBufferSize> buffer;
		JsonObject& data = getIrrigationData(buffer);
		data["tpfamt"] = topOffAmount = tmpInts[0];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}
void saveTopOffDelay(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<irrigateBufferSize> buffer;
		JsonObject& data = getIrrigationData(buffer);
		data["tpfdly"] = topOffDelay = tmpInts[0];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}
void saveFlowCalibration(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<irrigateBufferSize> jsonBuffer;
		JsonObject& data = getIrrigationData(jsonBuffer);
		data["flMeters"].asArray()[0] = tmpFloats[0];
		data["flMeters"].asArray()[1] = tmpFloats[1];
		flowMeterConfig[0] = tmpFloats[0];
		flowMeterConfig[1] = tmpFloats[1];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpFloats[0] = 0;
		tmpFloats[1] = 0;
		exitScreen();
	}
}
void saveDrainTime(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<irrigateBufferSize> buffer;
		JsonObject& data = getIrrigationData(buffer);
		data["drntime"] = drainTime = tmpInts[0];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpInts[0] = 0;
		exitScreen();
	}
}

//Helpers
void checkFlowRates(){
	if (currentRsvrVol == (tmpFlowCount * 2.25)){ //water must be done flowing from either direction
		if (flowInRate == true || flowOutRate == true){
			//lets store the final size once know we are done filling
			//StaticJsonBuffer<irrigateBufferSize> irrigationBuffer;
			//JsonObject& irrigationData = getIrrigationData(irrigationBuffer);
			//irrigationData["currentVol"] = tmpFlowCount;
			//setIrrigationData(irrigationData);
		}

		if (flowInRate == true){ //lets turn off our InFlow flag
			flowInRate = false;
		}

		if (flowOutRate == true){ //lets turn off our OutFlow flag
			flowOutRate = false;
		}
	}
}
void countRsvrFill(){
	tmpFlowCount++; //Every time this function is called, increment our global tmpFlowCount by 1
	currentRsvrVol = (tmpFlowCount * 2.25); //next we update our global currentRsvrVol to gallons
	flowInRate = true;
}
void countRsvrDrain(){
	tmpFlowCount--; //Every time this function is called, decrement our global tmpFlowCount by 1
	if (tmpFlowCount <= 0){ tmpFlowCount = 0; } //we can't have less than 0 in our reservoir
	currentRsvrVol = (tmpFlowCount * 2.25); //next we update our global currentRsvrVol to gallons
	flowOutRate = true;
}

void flushPlantWater(){
	RelayToggle(12, true);
	int i = drainTime * 60; //mins x 60secs = loop total
	while (i--){//we use loop to count i (aka seconds), then delay each loop by 1 second
		delay(1000); //1 second delay ensures we don't exceed 30k delay max
		if (i == 0){ //we done waiting?
			RelayToggle(12, false); //if out of wait loops, we turn drain valve off.
		}
	}
}
void flushRsvrWater(){
	//because of reservoir's automated dual float valve 
	//we know we are done flushing when flowInRate beings for us
	while (flowInRate == false){
		RelayToggle(11, true);
		if (flowInRate == true){
			RelayToggle(11, false);
			break;
		}
	}
}
void fullFlushing(){
	RelayToggle(11, true);
	RelayToggle(12, true);
	int i = drainTime * 60;
	while (i--){
		delay(1000);
		if (i == 0){ //we done waiting?
			RelayToggle(11, false);
			RelayToggle(12, false);
		}
	}
}
