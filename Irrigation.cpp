/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Irrigation.h"
#include "Core.h"
#include "Crops.h"
#include "Screens.h"

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

void setReservoirVolume(int dir){
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

void setTopOffConcentrate(int dir){
	String fraction;
	tmpInts[0] += dir;
	//min - max prevention
	if (tmpInts[0] > 5) {
		tmpInts[0] = 5;
	}else if (tmpInts[0] < 1){
		tmpInts[0] = 1;
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

void setTopOffVolume(int dir){
	tmpInts[0] += dir;
	String tpfVol;
	if (tmpInts[0] > 9999){ //max
		tpfVol = F("9999");
		tmpInts[0] = 9999;
	}
	else if (tmpInts[0] < 1000) {
		tpfVol = F("0");
	}
	else if (tmpInts[0] < 100){
		tpfVol = F("00");
	}
	else if (tmpInts[0] < 10){
		tpfVol = F("000");
	}
	else if (tmpInts[0] < 0){ //min
		tpfVol = F("0000");
		tmpInts[0] = 0;
	}

	lcd.clear();
	lcd.print(tpfVol);
	lcd.print(tmpInts[0]);
	lcd.print(F(" lqd/gal"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}

void setDrainTime(int dir){
	tmpInts[0] += dir;
	String drainTime;
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

void setFlowMeterCalibration(int dir){

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
	lcd.clear();
	lcd.print(F("IN"));
	lcd.print(tmpFloats[0]);
	lcd.print(F(" OUT"));
	lcd.print(tmpFloats[1]);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}

void Flow()
{
	tmpFlowCount++; //Every time this function is called, increment "count" by 1
}