/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Crops.h

#ifndef _CROPS_h
#define _CROPS_h
#include "Globals.h"
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)

extern String cropName;

extern JsonObject& getCropData(JsonBuffer& b);

extern void setCropData(JsonObject& d, bool returnHome = true);

extern void changeCrop();

extern void startNewCrop();

extern void renameCrop(int dir);

extern int getCropCount();

extern void buildCrop();

#endif

