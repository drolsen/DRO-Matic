// Channels.h

#ifndef _CHANNELS_h
#define _CHANNELS_h
#include "Globals.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)

extern JsonObject& getChannelData(JsonBuffer& b, int channelIndex = currentChannelIndex);

extern void setChannelData(JsonObject& d, int channelIndex = currentChannelIndex, bool returnHome = true);

extern void setPHChannels(int dir);

extern void setChannelNumber(int dir);

extern void makeChannel(String path, int numberOfSessions, JsonObject& channelData, JsonObject& sessionData);

extern void removeChannel(String path);

extern void trimChannels(int currentSize, int trimAmount);

extern void addChannels(int currentSize, int addAmount);

extern void setChannelSize(int dir);

extern void setCalibrationSize(int dir);

#endif

