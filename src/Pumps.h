/*
*  DROMatic.ino
*  DROMatic OS Pumps
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _PUMPS_h
#define _PUMPS_h
#include "Globals.h"
#include <ArduinoJson.h> //Arduno Json (aka epic)
extern byte currentPumpIndex;

//Read & Write from SD
extern JsonObject& getPumpsData(JsonBuffer& b);
extern void setPumpsData(JsonObject& d);
extern JsonObject& getPumpData(JsonBuffer& b, byte pumpIndex = currentPumpIndex);
extern void setPumpData(JsonObject& d, byte pumpIndex = currentPumpIndex, bool returnHome = true);

//Prints
extern void printPumpCalibration(int dir = 0);
extern void printPumpDelay(int dir = 0);

//Saves
extern void savePumpCalibration();
extern void savePumpDelay();

//Helpers
extern void pumpCreate(String path, byte pumpIndex, int totalRegimens, JsonObject& sessionData);
extern void primePump(int dir);
extern void pumpSpin(float setAmount, int pumpNumber);






#endif

