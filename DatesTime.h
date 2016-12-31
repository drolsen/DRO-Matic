// DateTime.h

#ifndef _DATESTIME_h
#define _DATESTIME_h
#include "Globals.h"

extern void captureDateTime();
extern void captureSessionDateTime();
extern void captureDateTimeDisplays();
extern int calculateDayOfYear(int day, int month, int year);
extern void setDateTime(int dir);

#endif

