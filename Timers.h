/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Timers.h

#ifndef _TIMERS_h
#define _TIMERS_h
#include "Globals.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)

extern byte currentTimerIndex, currentTimerSessionIndex, currentTimerSessionDayIndex;

//Get and set timer data
extern JsonObject& getTimerData(JsonBuffer& b, int timerIndex = currentTimerIndex);
extern void setTimerData(JsonObject& d, int timerIndex = currentTimerIndex);

//Get and set timer sessions
extern JsonObject& getTimerSessionData(JsonBuffer& b, int timerIndex = currentTimerIndex, int sessionIndex = currentTimerSessionIndex);
extern void setTimerSessionData(JsonObject& d, int timerIndex = currentTimerIndex, int sessionIndex = currentTimerSessionIndex);

//Helpers
extern void setTimerWeeks(int dir);
extern void setTimerStartEnd(int dir);
extern void makeTimerSessionFile(String path, byte index);

#endif

