/*
*  DROMatic.ino
*  DROMatic OS Globals
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _GLOBALS_h
#define _GLOBALS_h

#include <avr/pgmspace.h> //native to arduino no lib
#include <SPI.h> //Suppoting lib for SD card
#include <SD.h> //SD card API
#include <StandardCplusplus.h> //STD
#include <StandardCplusplus\vector> //Vectors
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <DS3231.h> //Real time clock lib
#include <Adafruit_NeoPixel.h> //LED lib

//Tentical Sheild
#define TOTAL_CIRCUITS 4

//Leds
#define LEDPIN 0
#define NUMOFLEDS 12

//Pin Configuration For Power Relay
#define FlowPinOut 2
#define FlowPinIn  3

const byte PHPower[3] = { 38, 40, 42 };
const byte ECPower[3] = { 39, 41, 43 };

#define RELAY1  22
#define RELAY2  23
#define RELAY3  24
#define RELAY4  25
#define RELAY5  26
#define RELAY6  27
#define RELAY7  28
#define RELAY8  29
#define RELAY9  30
#define RELAY10 31
#define RELAY11 32
#define RELAY12 33

#define RELAY13 35
#define RELAY14 34
#define RELAY15 37
#define RELAY16 36

#define coreBufferSize 32
#define cropBufferSize 220
#define pumpBufferSize 196
#define regimenBufferSize 512
#define regimenSessionBufferSize 64
#define timerBufferSize 164
#define timerSessionBufferSize 512
#define irrigateBufferSize 220
#define ecBufferSize 64

using namespace std; //Prefence of developer, I don't like typing std:: everywhere.

extern LiquidCrystal lcd;
extern DS3231  rtc;
extern Adafruit_NeoPixel pixels;

extern float flowInRate, flowOutRate, flowMeterConfig[2];
extern boolean irrigationFlag;

extern byte
currentTimerSessions[4],
maxRegimens,
minPH, maxPH, //Where we store the min and max pH values of our crop configuration
currentRegimen, //Where we store the current regimen we are on
drainTime, //Where we store how long of a drain time we will take when flushing plants water
topOffConcentrate,  //The fraction amount we dose our topoff water
topOffAmount, //The topoff amount we dose plant water with to correct over concentrated PPMs
topOffDelay,
cropStatus, //Does user have crop paused or started
feedingType,
lastFeedingDay,
lastFeedingWeek;


extern int Key, minPPM, maxPPM, rsvrVol, pumpCalibration, pumpDelay, pulseInFlowCount, pulseOutFlowCount;
extern double currentRsvrVol;

//Tentical Sheild
extern char sensordata[30];                  // A 30 byte character array to hold incoming data from the sensors
extern byte sensor_bytes_received;       // We need to know how many characters bytes have been received

extern byte code;                        // used to hold the I2C response code.
extern byte in_char;                     // used as a 1 byte buffer to store in bound bytes from the I2C Circuit.
extern int channel_ids[];				 // tentical shield channel ids array
extern char *channel_names[];			 // channe names (optional)

extern File tmpFile; //Single location to store file data when interfacting with SD card files
extern String nameArry[15], tmpDisplay[5]; //tmpDisplay = suffix, hour, min, day
extern int tmpInts[6];
extern float tmpFloats[2];

extern const char* const alphabet[37] PROGMEM;
extern const char* const screenNames[29][3] PROGMEM;
extern const char* const months[12] PROGMEM;
extern const char* const displayRepeats[6] PROGMEM;

#endif