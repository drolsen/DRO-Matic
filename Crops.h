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

extern JsonObject& getECData(JsonBuffer& b, byte ecRangeIndex);

extern void setECData(JsonObject& d, byte ecRangeIndex);

extern void changeCrop();

extern void startNewCrop();

extern void renameCrop(int dir);

extern int getCropCount();

extern void buildCrop();

extern void setPHRange(double dir);
extern void setECRange(int dir);

extern void setPPMHundredth(int dir);

extern void makeECRangeFile(String path, JsonObject& data, int index);

#endif

