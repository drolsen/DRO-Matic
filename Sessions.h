/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Sessions.h

#ifndef _SESSIONS_h
#define _SESSIONS_h
#include "Globals.h"
#include "Channels.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)

extern int currentSessionIndex;

extern JsonObject& getSessionData(JsonBuffer& b, int channelIndex = currentChannelIndex, int sessionIndex = currentSessionIndex);
extern void setSessionData(JsonObject& d, int channelIndex = currentChannelIndex, int sessionIndex = currentSessionIndex, bool returnHome = true);

extern void setSessionNumber(int dir);
extern void setSessionAmount(int dir);
extern void setSessionDelay(int dir);
extern void setSessionRepeat(int dir);

extern void makeSession(String path, JsonObject& data);

extern void addSessions(int currentSize, int addAmount);
extern void trimSessions(int currentSize, int trimAmount);

#endif

