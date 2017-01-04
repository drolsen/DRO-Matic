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
using namespace std; //Prefence of developer, I don't like typing std:: everywhere.

//Pin Configuration For Power Relay
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

extern File tmpFile; //Single location to store file data when interfacting with SD card files
extern String nameArry[15], tmpDisplay[5]; //tmpDisplay = suffix, hour, min, day
extern int tmpInts[6];
extern float tmpFloats[2];

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

extern const char sysconf[8] PROGMEM;
extern const char System[7] PROGMEM;
extern const char settings[9] PROGMEM;
extern const char sessions[9] PROGMEM;
extern const char session[8] PROGMEM;
extern const char channel[8] PROGMEM;
extern const char channels[9] PROGMEM;
extern const char numberOf[10] PROGMEM;
extern const char dateTime[12] PROGMEM;
extern const char configuration[14] PROGMEM;
extern const char calibration[12] PROGMEM;
extern const char ppmRange[13] PROGMEM;
extern const char phRange[9] PROGMEM;
extern const char cropLoad[10] PROGMEM;
extern const char newCrop[9] PROGMEM;
extern const char deleteCrop[12] PROGMEM;
extern const char sizeMl[12] PROGMEM;
extern const char amountMl[11] PROGMEM;
extern const char sessionStart[14] PROGMEM;
extern const char sessionDelay[14] PROGMEM;
extern const char sessionRepeat[15] PROGMEM;
extern const char datetime[9] PROGMEM;
extern const char choconf[8] PROGMEM;
extern const char chnum[6] PROGMEM;
extern const char ppm[4] PROGMEM;
extern const char ph[3] PROGMEM;
extern const char Open[5] PROGMEM;
extern const char New[8] PROGMEM;
extern const char Delete[7] PROGMEM;
extern const char chdoses[8] PROGMEM;
extern const char chcalib[8] PROGMEM;
extern const char chsize[7] PROGMEM;
extern const char amt[4] PROGMEM;
extern const char str[12] PROGMEM;
extern const char dly[12] PROGMEM;
extern const char rpt[12] PROGMEM;

extern const char* const displayNames[18][3] PROGMEM;

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

