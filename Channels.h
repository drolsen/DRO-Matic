/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Channels.h

#ifndef _CHANNELS_h
#define _CHANNELS_h
#include "Globals.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)

extern byte currentChannelIndex;

extern JsonObject& getChannelData(JsonBuffer& b, byte channelIndex = currentChannelIndex);

extern void setChannelData(JsonObject& d, byte channelIndex = currentChannelIndex, bool returnHome = true);

extern void makeChannel(String path, int numberOfSessions, JsonObject& sessionData);

extern void setCalibrationSize(int dir);

extern void primeChannelPump(int dir);

#endif

