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
void printReservoirVolume(){
	tmpFloats[0] += (flowInRate / 60) * 1000;
	float liters = tmpFloats[0] / 1000;
	float USgallons = tmpFloats[0] / 4546.091879;
	float UKgallons = USgallons * 0.83267384;
	currentRsvrVol = USgallons;

	lcd.clear();
	lcd.print(USgallons, 1);
	lcd.print(F("lqd/"));
	lcd.print(UKgallons, 1);
	lcd.print(F("gal"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);
	lcd.blink();
}
void printTopOffConcentrate(int dir = 0){
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
		lcd.print(F("1/6"));
	}
	if (tmpInts[0] == 2) {
		lcd.print(F("2/6"));
	}
	if (tmpInts[0] == 3) {
		lcd.print(F("3/6"));
	}
	if (tmpInts[0] == 4) {
		lcd.print(F("4/6"));
	}
	if (tmpInts[0] == 5) {
		lcd.print(F("5/6"));
	}
	lcd.print(F(" CONCENTRATE"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}
void printTopOffAmount(int dir = 0){
	if (dir != 0){
		tmpFloats[0] += (dir > 0) ? 0.05 : -0.05;
		//top off amount can't be lower than 1 gallon.
		if (tmpFloats[0] < 0.05){
			tmpFloats[0] = 0.05;
		}
		//top off amounts can't be higher than reservoir volume / 4
		if (tmpFloats[0] > rsvrVol){
			tmpFloats[0] = rsvrVol;
		}
	}
	lcd.clear();

	if (tmpInts[0] > 100) {
		tmpDisplay[0] = "";
	}
	else if (tmpFloats[0] > 10) {
		tmpDisplay[0] = "0";
	}
	else if (tmpFloats[0] > 1) {
		tmpDisplay[0] = "00";
	}
	
	lcd.print(F("TPF (gal) "));
	lcd.print(tmpDisplay[0]);
	lcd.print(tmpFloats[0]);
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
			} else {
				tmpFloats[0] -= 0.1;
			}
		}
		if (cursorX == 13 && cursorY == 0){
			if (dir > 0){
				tmpFloats[1] += 0.1;
			} else {
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
	lcd.clear();
	lcd.print(F("MANUAL FLUSHING"));
	lcd.setCursor(0, 1);
	lcd.print(F("<in><out> <done>"));
	cursorX = cursorY = 1;
	lcd.setCursor(cursorX, cursorY);
}

//Saves
void saveReservoirVolume(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		StaticJsonBuffer<irrigateBufferSize> jsonBuffer;
		JsonObject& data = getIrrigationData(jsonBuffer);
		data["rsvrvol"] = rsvrVol = currentRsvrVol;
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		exitScreen();
	}
}
void saveTopOffConcentrate(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
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
		StaticJsonBuffer<irrigateBufferSize> buffer;
		JsonObject& data = getIrrigationData(buffer);
		data["tpfamt"] = topOffAmount = tmpInts[0];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpFloats[0] = 0;
		exitScreen();
	}
}
void saveTopOffDelay(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
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
		StaticJsonBuffer<irrigateBufferSize> jsonBuffer;
		JsonObject& data = getIrrigationData(jsonBuffer);
		data["flMeters"].asArray()[0] = tmpFloats[0];
		data["flMeters"].asArray()[1] = tmpFloats[1];
		flowMeterConfig[0] = tmpFloats[0];
		flowMeterConfig[1] = tmpFloats[1];
		setIrrigationData(data);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		tmpFloats[0] = tmpFloats[1] = 0;
		exitScreen();
	}
}
void saveDrainTime(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
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

//Flush plants
void drainPlants(int min = 0, int sec = 0){
	lcd.clear();
	lcd.print(F("FLUSHING PLANTS"));
	lcd.setCursor(0, 1);
	lcd.print(F("PLEASE HOLD!!!"));

	RelayToggle(11, false);
	RelayToggle(12, true);

	int i = (min * 60) + sec; //mins x 60secs = loop total
	while (i--){
		delay(1000); //1 second delay ensures we don't exceed 30k delay max
	}

	RelayToggle(11, false);
	RelayToggle(12, false); //if out of wait loops, we turn drain valve off.
}

//Feed plants (flowInRate event based)
void feedPlants(int min = 0, int sec = 0){
	lcd.clear();
	lcd.print(F("FEEDING PLANTS"));
	lcd.setCursor(0, 1);
	lcd.print(F("PLEASE HOLD!!!"));
	RelayToggle(11, true);
	RelayToggle(12, false);
	flowInRate = flowOutRate = pulseInFlowCount = pulseOutFlowCount = 0;
	checkFlowRates();
	if (min == 0 && sec == 0){ 
		/////////////////
		//FULL FEEDINGS//
		/////////////////

		//WARNING KEEP YOUR RESERVOIR WATER "IN-LINE" OUT OF RESEROIVR WATER!! 
		//YOU MAY CREATE A SYPHING EFFECT THAT PREMATURELY SPINS FLOW SENSOR
		while (flowInRate < 1){
			checkFlowRates();
		}
	}
	else {
		//////////////////
		//MICRO FEEDINGS//
		//////////////////
		int i = (min * 60) + sec; //mins x 60secs = loop total
		while (i--){ //time based feeding
			checkFlowRates();
			//break cause reservoir started filling up
			if (flowInRate > 1 && feedingType == 2){ break; }
			delay(1000);
		}
	}

	RelayToggle(11, false); //close in irrigation value
	RelayToggle(12, false); //close out irrigation value
}

void checkFlowRates(){
	if ((millis() - flowMillis) <= 1000){ return; }

	//Capture our flow rates for both irrigation directions
	flowInRate = ((1000.0 / (millis() - flowMillis)) * pulseInFlowCount) / flowMeterConfig[0];
	flowOutRate = ((1000.0 / (millis() - flowMillis)) * pulseOutFlowCount) / flowMeterConfig[1];

	//automated irrigation or a menu config editing irrigation?
	//we don't want to store or capture currentRsvrVol while configuring OS
	if (screenName == "") {
		//do we have a flow rate for in?
		if (flowInRate > 1){ //true irrigations will be over .025 (for safe measure)
			currentRsvrVol += (flowInRate / 60) * 1000;
			irrigationInFlag = true; //flag OS while irrigation is taking place
			RelayToggle(11, false); //ensures that we are not feeding un-dosed water to plants
			if (feedingType == 2 && cropStatus == 1){
				moveToNextRegimen();
			}
		}

		//do we have a flow rate for out?
		if (flowOutRate > 1){ //true irrigations will be over .025 (for safe measure)
			currentRsvrVol -= (flowOutRate / 60) * 1000;
			currentRsvrVol = (currentRsvrVol > 0) ? currentRsvrVol : 0; //prevents rsvr vol from falling below 0
			irrigationOutFlag = true; //flag OS while irrigation is taking place
			if (flowInRate > 1){ RelayToggle(11, false); } //ensures that we are not feeding un-dosed water to plants
		}

		//when all flowRates have stopped, we store data
		if ((irrigationInFlag == true || irrigationOutFlag == true) && flowInRate < 0.025 && flowOutRate < 0.025){
			if (irrigationInFlag == true){
				irrigationInFlag = false; //reset irrigation flag for OS
			}
			if (irrigationOutFlag == true){
				irrigationOutFlag = false; //reset irrigation flag for OS
			}

			//store the reservoir's remaining volume
			StaticJsonBuffer<irrigateBufferSize> irrigationBuffer;
			JsonObject& irrigationData = getIrrigationData(irrigationBuffer);
			irrigationData["currentVol"] = currentRsvrVol;
			setIrrigationData(irrigationData);
		}
	}

	pulseInFlowCount = pulseOutFlowCount = 0; //reset pulse counts for next time around

	//reset flowMillis
	flowMillis = millis();
}

//In flow rate counter
void countRsvrFill(){
	pulseInFlowCount++;
}

//Out flow rate counter
void countRsvrDrain(){
	pulseOutFlowCount++;
}
