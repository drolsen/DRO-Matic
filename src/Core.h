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
#include <ArduinoJson.h> //Arduno Json (aka epic)


extern JsonObject& getCoreData(JsonBuffer& b);
extern void setCoreData(JsonObject& d);
extern void coreInit();
extern void correctPlantPH();
extern void correctRsvrPH();
extern void correctPlantEC();
extern void correctRsvrEC();
extern float getPHProbeValue(byte channel);
extern int getECProbeValue(byte channel);


//Helper Functions
extern void RelayToggle(int channel, bool gate);
extern void makeNewFile(String path, JsonObject& data);
extern int tmpIntsToInt(byte decimalPlaces);
extern void resetTimestamps();
#endif

