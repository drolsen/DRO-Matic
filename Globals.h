/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Globals.h

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
#include <ResponsiveAnalogRead.h> //Noise reduction on EC/PH sensors

#define LEDPIN         0
#define NUMOFLEDS      12

//Pin Configuration For Power Relay
#define FlowPinIn  2
#define FlowPinOut 3

//Set one
#define PHPin1 A1
#define ECPin1 A4
//Set two
#define PHPin2 A3
#define ECPin2 A2

#define MS1MS2  16
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
#define RELAY13 34
#define RELAY14 35
#define RELAY15 36
#define RELAY16 37

#define coreBufferSize 32
#define cropBufferSize 196
#define channelBufferSize 128
#define regimenBufferSize 512
#define regimenSessionBufferSize 64
#define timerBufferSize 96
#define timerSessionBufferSize 560
#define irrigateBufferSize 164
#define ecBufferSize 64

using namespace std; //Prefence of developer, I don't like typing std:: everywhere.

extern LiquidCrystal lcd;
extern DS3231  rtc;
extern Adafruit_NeoPixel pixels;

extern ResponsiveAnalogRead PH1Analog;
extern ResponsiveAnalogRead PH2Analog;
extern ResponsiveAnalogRead EC1Analog;
extern ResponsiveAnalogRead EC2Analog;

extern int Key, minPPM, maxPPM, minPH, maxPH, maxRegimens;
extern double PPMHundredth;
extern double InFlowRate, OutFlowRate;

extern File tmpFile; //Single location to store file data when interfacting with SD card files
extern String nameArry[15], tmpDisplay[5]; //tmpDisplay = suffix, hour, min, day
extern int tmpInts[6];
extern float tmpFloats[2];
extern volatile int tmpFlowCount;

extern const char blank[2] PROGMEM;
extern const char a[2] PROGMEM;
extern const char b[2] PROGMEM;
extern const char c[2] PROGMEM;
extern const char d[2] PROGMEM;
extern const char e[2] PROGMEM;
extern const char f[2] PROGMEM;
extern const char g[2] PROGMEM;
extern const char h[2] PROGMEM;
extern const char i[2] PROGMEM;
extern const char j[2] PROGMEM;
extern const char k[2] PROGMEM;
extern const char l[2] PROGMEM;
extern const char m[2] PROGMEM;
extern const char n[2] PROGMEM;
extern const char o[2] PROGMEM;
extern const char p[2] PROGMEM;
extern const char q[2] PROGMEM;
extern const char r[2] PROGMEM;
extern const char s[2] PROGMEM;
extern const char t[2] PROGMEM;
extern const char u[2] PROGMEM;
extern const char v[2] PROGMEM;
extern const char w[2] PROGMEM;
extern const char x[2] PROGMEM;
extern const char y[2] PROGMEM;
extern const char z[2] PROGMEM;
extern const char zero[2] PROGMEM;
extern const char one[2] PROGMEM;
extern const char two[2] PROGMEM;
extern const char three[2] PROGMEM;
extern const char four[2] PROGMEM;
extern const char five[2] PROGMEM;
extern const char six[2] PROGMEM;
extern const char seven[2] PROGMEM;
extern const char eight[2] PROGMEM;
extern const char nine[2] PROGMEM;

extern const char* const alphabet[37] PROGMEM;

extern const char _sys[4] PROGMEM;
extern const char _crop[5] PROGMEM;
extern const char _irri[5] PROGMEM;
extern const char _chan[4] PROGMEM;
extern const char _timer[7] PROGMEM;
extern const char _datetime[9] PROGMEM;
extern const char _EC[3] PROGMEM;
extern const char _PH[3] PROGMEM;
extern const char _PPM[4] PROGMEM;
extern const char _open[5] PROGMEM;
extern const char _new[4] PROGMEM;
extern const char _delete[7] PROGMEM;
extern const char _reset[7] PROGMEM;
extern const char _amt[7] PROGMEM;
extern const char _rsvrVol[8] PROGMEM;
extern const char _chCalib[6] PROGMEM;
extern const char _topOffCcnt[8] PROGMEM;
extern const char _topOffVol[7] PROGMEM;
extern const char _drainTime[8] PROGMEM;
extern const char _doses[6] PROGMEM;
extern const char _sysCh[6] PROGMEM;
extern const char _prime[6] PROGMEM;
extern const char _startend[9] PROGMEM;
extern const char _weeks[6] PROGMEM;

//Consolidated Repeating Displays Words
extern const char System[7] PROGMEM;
extern const char Settings[9] PROGMEM;
extern const char Channel[8] PROGMEM;
extern const char Channels[9] PROGMEM;
extern const char NumberOf[10] PROGMEM;
extern const char Configuration[14] PROGMEM;
extern const char Config[7] PROGMEM;
extern const char Concent[12] PROGMEM;
extern const char Calib[10] PROGMEM;
extern const char Volume[7] PROGMEM;
extern const char Range[6] PROGMEM;
extern const char Irrigation[11] PROGMEM;
extern const char Reservoir[10] PROGMEM;
extern const char SizeMl[12] PROGMEM;
extern const char Crop[5] PROGMEM;
extern const char Timer[6] PROGMEM;
extern const char Times[6] PROGMEM;
extern const char TopOff[8] PROGMEM;
extern const char Names[6] PROGMEM;
extern const char RegimensML[14] PROGMEM;
extern const char RegimensDoses[14] PROGMEM;
extern const char Weeks[6] PROGMEM;
extern const char Solution[9] PROGMEM;
extern const char PPM[14] PROGMEM;

//Direct Translations
extern const char DateTime[12] PROGMEM;
extern const char EC[13] PROGMEM;
extern const char PH[9] PROGMEM;
extern const char ReservoirPH[13] PROGMEM;
extern const char Open[5] PROGMEM;
extern const char New[4] PROGMEM;
extern const char Delete[7] PROGMEM;
extern const char Reset[6] PROGMEM;
extern const char DrainLength[11] PROGMEM;
extern const char StartEnd[11] PROGMEM;
extern const char PrimeChannel[14] PROGMEM;
extern const char VolumeConfig[14] PROGMEM;

extern const char* const displayNames[27][3] PROGMEM;

extern const char jan[4] PROGMEM;
extern const char feb[4] PROGMEM;
extern const char mar[4] PROGMEM;
extern const char apl[4] PROGMEM;
extern const char may[4] PROGMEM;
extern const char jun[4] PROGMEM;
extern const char jul[4] PROGMEM;
extern const char aug[4] PROGMEM;
extern const char sep[4] PROGMEM;
extern const char oct[4] PROGMEM;
extern const char nov[4] PROGMEM;
extern const char dec[4] PROGMEM;

extern const char* const months[12] PROGMEM;

extern const char none[5] PROGMEM;
extern const char hourly[7] PROGMEM;
extern const char daily[6] PROGMEM;
extern const char weekly[7] PROGMEM;
extern const char monthly[8] PROGMEM;
extern const char yearly[7] PROGMEM;

extern const char* const displayRepeats[6] PROGMEM;

#endif

