/*
*  DROMatic.ino
*  DROMatic OS Crops
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _CROPS_h
#define _CROPS_h
#include "Globals.h"
#include <ArduinoJson.h> //Arduno Json (aka epic)

extern String cropName;

extern JsonObject& getCropData(JsonBuffer& b);

extern void setCropData(JsonObject& d, bool returnHome = true);

extern JsonObject& getECData(JsonBuffer& b, byte ecRangeIndex);

extern void setECData(JsonObject& d, byte ecRangeIndex);

extern void cropChange();

extern void cropCreate();

extern void cropRename(int dir);

extern void cropReset();

extern void cropBuild();

extern void cropLoad();

//Saves
extern void saveECRange();
extern void savePHRange();
extern void saveStatus();
extern void savePHDelay();
extern void savePHAmount();
extern void setPHWaterProbeCalibration(byte channel, int value, char type);
extern void setECWaterProbeCalibration(byte channel, int value, char type);

//Prints
extern void printStatus(int dir = 0);
extern void printPHAmount(int dir = 0);
extern void printPHDelay(int dir = 0);
extern void printPHRange(double dir);
extern void printECRange(int dir);
extern void printECCalibrations(String type, int dir = 0);
extern void printPHCalibrations(String type, byte value);
extern void printOpen(int dir = 0);
extern void printReset();

#endif

