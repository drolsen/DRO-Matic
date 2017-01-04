/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// DateTime.h

#ifndef _DATESTIME_h
#define _DATESTIME_h
#include "Globals.h"

extern int currentMinute, days[12];
extern unsigned long previousMillis, currentMillis;  //stores last time

extern void captureDateTime();
extern void captureSessionDateTime();
extern void captureDateTimeDisplays();
extern int calculateDayOfYear(int day, int month, int year);
extern void setDateTime(int dir);

#endif

