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
#include "DatesTime.h"

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
				tmpFloats[0] += 0.1;
			}
			else{
				tmpFloats[0] -= 0.1;
			}
		}
		if (cursorX == 13 && cursorY == 0){
			if (dir > 0){
				tmpFloats[1] += 0.1;
			}
			else{
				tmpFloats[1] -= 0.1;
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
	lcd.print(F("<in><out> <done>"));
	cursorX = 0;
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
		data["rsvrvol"].asArray()[0] = rsvrVol;
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
	//detach & re-attach flow meters to counter methods per OS loop
	detachInterrupt(digitalPinToInterrupt(FlowPinIn));
	detachInterrupt(digitalPinToInterrupt(FlowPinOut));

	//Get our rate of flow for either irrigation directions
	flowInRate = ((1000.0 / (millis() - homeMillis)) * pulseInFlowCount) / flowMeterConfig[0]; 
	flowOutRate = ((1000.0 / (millis() - homeMillis)) * pulseOutFlowCount) / flowMeterConfig[1];

	//automated irrigation or a menu config editing irrigation?
	//we don't want to store or capture currentRsvrVol while configuring OS
	if (screenName == ""){
		//do we have a flow rate for in?
		if (flowInRate > 0){
			currentRsvrVol += (flowInRate / 60) * 1000;
			irrigationFlag = true; //flag OS while irrigation is taking place
		}

		//do we have a flow rate for out?
		if (flowOutRate > 0){
			currentRsvrVol -= (flowOutRate / 60) * 1000;
			irrigationFlag = true; //flag OS while irrigation is taking place
		}

		//did we previously have a flow rate, but now it it seems to have stopped?
		if (irrigationFlag == true && flowInRate <= 0 && flowOutRate <= 0){
			//store the final size once know we are done filling
			StaticJsonBuffer<irrigateBufferSize> irrigationBuffer;
			JsonObject& irrigationData = getIrrigationData(irrigationBuffer);
			irrigationData["currentVol"] = currentRsvrVol;
			setIrrigationData(irrigationData);
			irrigationFlag = false; //reset irrigation flag for OS
		}
	}
	pulseInFlowCount = pulseOutFlowCount = 0; //reset pulse counts for next time around

	//re-attach interrupts
	attachInterrupt(digitalPinToInterrupt(FlowPinIn), countRsvrFill, FALLING);
	attachInterrupt(digitalPinToInterrupt(FlowPinOut), countRsvrDrain, FALLING);
}
//flow rate counter for in
void countRsvrFill(){
	pulseInFlowCount++;
}
//flow rate counter for out
void countRsvrDrain(){
	pulseOutFlowCount++;
}
//Flush only plant water (drainTime config based)
void flushPlantWater(){
	RelayToggle(12, true);
	int i = drainTime * 60; //mins x 60secs = loop total
	while (i--){//loop to count i (aka seconds), then delay each loop by 1 second
		delay(1000); //1 second delay ensures we don't exceed 30k delay max
		if (i == 0){ //we done waiting?
			RelayToggle(12, false); //if out of wait loops, we turn drain valve off.
		}
	}
}
//Flush only reservoir water (flowInRate event based)
void flushRsvrWater(){
	//while reservoir isn't filling up, flush remaining reservoir water to plants.
	while (flowInRate == 0){
		RelayToggle(11, true);
		if (flowInRate > 0){
			RelayToggle(11, false);
			break; //break the loop.
		}
	}
}
//Flush both plant and reservoir water, 
//but with premature stopping of plant water drain based on OS drain time configuration.
void fullFlushing(){
	RelayToggle(11, true); //in
	RelayToggle(12, true); //out
	int i = drainTime * 60; //mins x 60secs = loop total
	while (i--){
		delay(1000);
		if (i == 0){ //if out of drainTime loops, we close off both irrigation values.
			RelayToggle(11, false); //in
			RelayToggle(12, false); //out
		}
		//if reservoir begins filling, we prematurely close off in valve.
		if (flowInRate > 0){ 
			RelayToggle(11, false); //in
		}
	}
}
