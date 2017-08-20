/*
*  DROMatic.ino
*  DROMatic OS DateTime
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _DATESTIME_h
#define _DATESTIME_h
#include "Globals.h"

extern byte currentMinute, previousMinute, days[12];
extern unsigned long menuMillis, homeMillis, flowMillis, ecMillis, phRsvrMillis, phPlantMillis;  //stores last time

//Read & Write from SD
extern void captureDateTime();
extern void captureSessionDateTime();

//Prints
extern void printDateTime(int dir = 0);

//Saves
extern void saveDateTime();

//Helpers
extern void captureDateTimeDisplays(int month = tmpInts[1], int day = tmpInts[2], int hour = tmpInts[4], int min = tmpInts[5]);
extern int calculateDayOfYear(int day, int month, int year);
extern byte calcDayOfWeek(unsigned int y, byte m, byte d);

#endif

