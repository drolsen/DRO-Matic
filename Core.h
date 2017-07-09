/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _CORE_h
#define _CORE_h

#include "Globals.h"
#include <Wire.h>
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)


extern JsonObject& getCoreData(JsonBuffer& b);
extern void setCoreData(JsonObject& d);
extern void coreInit();
extern void makeNewFile(String path, JsonObject& data);
extern int tmpIntsToInt(byte decimalPlaces);
extern void correctPlantPH();
extern void correctRsvrPH();
extern void correctPlantEC();
extern void correctRsvrEC();
extern void openWaterProbeChannel(int channel);
extern int getWaterProbeValue(byte channel);
extern void setPHWaterProbeCalibration(byte channel, int value, char type);
extern void setECWaterProbeCalibration(byte channel, int value, char type);
extern void checkRecepticals();

//Pumping Functions
extern void RelayToggle(int channel, bool gate);
extern void pumpSpin(int setAmount, int pumpNumber, int pumpFlowRate = 100);



#endif

