/*
*  DROMatic.ino
*  DROMatic OS Timers
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _TIMERS_h
#define _TIMERS_h
#include "Globals.h"
#include <ArduinoJson.h> //Arduno Json (aka epic)

extern byte currentTimerIndex, currentTimerSessionIndex, currentTimerSessionDayIndex;

//Read & Write from SD
extern JsonObject& getTimerData(JsonBuffer& b);
extern void setTimerData(JsonObject& d);
extern JsonObject& getTimerSessionData(JsonBuffer& b, int timerIndex = currentTimerIndex, int weekIndex = currentTimerSessionIndex);
extern void setTimerSessionData(JsonObject& d, int timerIndex = currentTimerIndex, int weekIndex = currentTimerSessionIndex);
extern void checkTimers();
//Print
extern void printTimerWeeks(int dir);
extern void printTimerStartEnd(int dir);

//Saves
extern void saveStartEnd();

//Helpers
extern JsonObject& newTimerSessionData();

#endif

