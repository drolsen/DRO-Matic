/*
*  DROMatic.ino
*  DROMatic OS Irrigation
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _IRRIGATION_h
#define _IRRIGATION_h
#include <ArduinoJson.h> //Arduno Json (aka epic)

extern JsonObject& getIrrigationData(JsonBuffer& b);
extern void setIrrigationData(JsonObject& d);

//Prints
extern void printReservoirVolume();
extern void printTopOffConcentrate(int dir = 0);
extern void printTopOffAmount(int dir = 0);
extern void printTopOffDelay(int dir = 0);
extern void printDrainTime(int dir = 0);
extern void printFlowCalibration(int dir = 0);
extern void printFullFlushing();

//Saves
extern void saveReservoirVolume();
extern void saveTopOffConcentrate();
extern void saveTopOffAmount();
extern void saveTopOffDelay();
extern void saveFlowCalibration();
extern void saveDrainTime();

//Helpers
extern void feedPlants(int min = 0, int sec = 0);
extern void drainPlants(int min = 0, int sec = 0);
extern void checkFlowRates();
extern void countRsvrFill();
extern void countRsvrDrain();

#endif

