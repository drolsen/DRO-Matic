/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Core.h

#ifndef _CORE_h
#define _CORE_h

#include "Globals.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)


extern JsonObject& getCoreData(JsonBuffer& b);
extern void setCoreData(JsonObject& d);
extern void coreInit();
extern void makeNewFile(String path, JsonObject& data);

//Pumping Functions
extern void turing();
extern void RelayToggle(int channel, bool gate);
extern void pumpSpin(int setAmount, int setCalibration, int channelSize, int channelNumber, int sessionNumber, JsonObject& session);



#endif

