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
#include <vector> //Vectors
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <DS3231.h> //Real time clock lib
#include <Adafruit_NeoPixel.h> //LED lib

//Tentical Sheild
#define TOTAL_CIRCUITS 4

//Leds
#define LEDPIN 0
#define NUMOFLEDS 10

//Pin Configuration For Power Relay
#define FlowPinOut 2
#define FlowPinIn  3

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

#define coreBufferSize 64
#define cropBufferSize 300
#define pumpBufferSize 196
#define regimenBufferSize 512
#define regimenSessionBufferSize 64
#define timerBufferSize 164
#define timerSessionBufferSize 512
#define irrigateBufferSize 260
#define ecBufferSize 64

#define PLANTEC 0
#define PLANTPH 1
#define RSVREC 2
#define RSVRPH 3


#define menusBufferSize 31

using namespace std; //Prefence of developer, I don't like typing std:: everywhere.

extern LiquidCrystal lcd;
extern DS3231  rtc;
extern Adafruit_NeoPixel pixels;

extern boolean irrigationInFlag, irrigationOutFlag;

extern byte
currentTimerSessions[4],
maxRegimens,
currentRegimen, //Where we store the current regimen we are on
drainTime, //Where we store how long of a drain time we will take when flushing plants water
topOffConcentrate,  //The fraction amount we dose our topoff water
topOffDelay,
cropStatus, //Does user have crop paused or started
feedingType,
pumpDelay,
timerStartHours[4],
timerEndHours[4];

extern volatile unsigned long pulseInFlowCount, pulseOutFlowCount;
extern float tmpFloats[2], flowMeterConfig[2], rsvrVol, currentRsvrVol, topOffAmount, flowInRate, flowOutRate, minPH, maxPH, phAmount;
extern int Key, minPPM, maxPPM, phDelay, pumpCalibration;

//Tentical Sheild
extern char sensordata[30];                  // A 30 byte character array to hold incoming data from the sensors
extern byte sensor_bytes_received;       // We need to know how many characters bytes have been received

extern byte code;                        // used to hold the I2C response code.
extern byte in_char;                     // used as a 1 byte buffer to store in bound bytes from the I2C Circuit.
extern int channel_ids[];				 // tentical shield channel ids array

extern File tmpFile; //Single location to store file data when interfacting with SD card files
extern String nameArry[15], tmpDisplay[5]; //tmpDisplay = suffix, hour, min, day
extern int tmpInts[6];

extern const char* const alphabet[37] PROGMEM;
extern const char* const screenNames[menusBufferSize][3] PROGMEM;
extern const char* const months[12] PROGMEM;


#endif