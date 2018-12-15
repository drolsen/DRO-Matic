/*
*  DROMatic.ino
*  DROMatic OS Regimens
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _SESSIONS_h
#define _SESSIONS_h
#include "Globals.h"
#include "Pumps.h"
#include <ArduinoJson.h> //Arduno Json (aka epic)

extern byte currentRegimenIndex;

//Read & Write from SD
extern JsonObject& getRegimenData(JsonBuffer& b, byte pumpIndex = currentPumpIndex, byte sessionIndex = currentRegimenIndex);
extern void setRegimenData(JsonObject& d, byte pumpIndex = currentPumpIndex, byte sessionIndex = currentRegimenIndex, bool returnHome = true);

//Prints
extern void printRegimenNumber(int dir);
extern void printRegimenAmount(int dir = 0);

//Saves
extern void saveRegimenAmount();

//Helpers
extern void addRegimens(int currentSize, int addAmount);
extern void trimRegimens(int currentSize, int trimAmount);
extern void moveToNextRegimen();

//Dosing functionality
extern void checkRegimenDosing();

#endif

