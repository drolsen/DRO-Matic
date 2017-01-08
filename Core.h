/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Core.h

#ifndef _CORE_h
#define _CORE_h

#include "Globals.h"
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)
#include <Stepper.h> //Stepper motoer lib
#include <DS3231.h> //Real time clock lib

extern Stepper myStepper;
extern LiquidCrystal lcd;
extern DS3231  rtc;

extern int Key, minPPM, maxPPM;
extern const int stepsPerRevolution, stepperSpeed;

extern JsonObject& getCoreData(JsonBuffer& b);
extern void setCoreData(JsonObject& d);
extern void coreInit();
extern void setPHRange(double dir);
extern void setPPMRangeValues(int dir);
extern void makeNewFile(String path, JsonObject& data);

//Pumping Functions
extern void turing();
extern void RelayToggle(int channel, bool gate);
extern void pumpSpin(int setAmount, int setCalibration, int channelSize, int channelNumber, int sessionNumber, JsonObject& session);



#endif

