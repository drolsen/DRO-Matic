/*
*  DROMatic.ino
*  DROMatic OS Channels
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _CHANNELS_h
#define _CHANNELS_h
#include "Globals.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)
extern byte currentChannelIndex;

//Read & Write from SD
extern JsonObject& getChannelsData(JsonBuffer& b);

extern void setChannelsData(JsonObject& d);

extern JsonObject& getChannelData(JsonBuffer& b, byte channelIndex = currentChannelIndex);

extern void setChannelData(JsonObject& d, byte channelIndex = currentChannelIndex, bool returnHome = true);

//Prints
extern void printPumpCalibration(int dir = 0);
extern void printPumpDelay(int dir = 0);

//Saves
extern void savePumpCalibration();
extern void savePumpDelay();

//Helpers
extern void channelCreate(String path, int numberOfSessions, JsonObject& sessionData);
extern void primeChannelPump(int dir);






#endif

