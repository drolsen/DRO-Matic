// Irrigation.h

#ifndef _IRRIGATION_h
#define _IRRIGATION_h
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)

extern JsonObject& getIrrigationData(JsonBuffer& b);
extern void setIrrigationData(JsonObject& d);
extern void setReservoirVolume(int dir);
extern void setTopOffConcentrate(int dir);
extern void setTopOffVolume(int dir);
extern void setDrainTime(int dir);
extern void setFlowMeterCalibration(int dir);
extern void Flow();
#endif

