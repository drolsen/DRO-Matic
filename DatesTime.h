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

extern byte currentMinute, previousMinute, days[12];
extern unsigned long menuMillis, homeMillis, currentMillis;  //stores last time

extern void captureDateTime();
extern void captureSessionDateTime();
extern void captureDateTimeDisplays(int month = tmpInts[1], int day = tmpInts[2], int hour = tmpInts[4], int min = tmpInts[5]);
extern int calculateDayOfYear(int day, int month, int year);
extern void setDateTime(int dir);

#endif

