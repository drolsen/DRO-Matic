/*
*  DROMatic.ino
*  DROMatic OS DateTime
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "DatesTime.h"
#include "Core.h"
#include "Regimens.h"
#include "Screens.h"

byte currentMinute;
byte previousMinute;
byte days[12] = { 31, ((tmpInts[5] % 4 == 0 && tmpInts[5] % 100 != 0) || (tmpInts[5] % 400 == 0)) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
unsigned long menuMillis, homeMillis, flowMillis, ecMillis, phRsvrMillis, phPlantMillis;  //stored timestamps

//Read & Write from SD
void captureDateTime(){
	Time current = rtc.getTime();
	previousMinute = current.min;
	tmpInts[0] = current.year;
	tmpInts[1] = ((current.mon-1) > 11)? 0 : current.mon-1;
	tmpInts[2] = current.date;
	tmpInts[3] = current.dow;
	tmpInts[4] = (current.hour == 0) ? 24 : current.hour;
	tmpInts[5] = current.min;
	captureDateTimeDisplays();
}
void captureSessionDateTime(){
	DynamicJsonBuffer sessionBuffer;
	JsonObject& data = getRegimenData(sessionBuffer);
	JsonArray& date = data["date"].asArray();
	JsonArray& time = data["time"].asArray();
	tmpInts[0] = date[0]; //year
	tmpInts[1] = date[1]; //month
	tmpInts[2] = date[2]; //day
	tmpInts[3] = date[3]; //day of week
	tmpInts[4] = time[0]; //hour
	tmpInts[5] = time[1]; //min
	captureDateTimeDisplays();
}

//Prints
void printDateTime(int dir = 0){
	if (dir != 0){
		int maxDaysInMonth = days[tmpInts[1]];
		if (cursorY == 0){
			if (cursorX == 10){
				//Month
				(dir == 1) ? (tmpInts[1] = (tmpInts[1] + 1 > 11) ? 0 : tmpInts[1] + 1) : (tmpInts[1] = (tmpInts[1] - 1 < 0) ? 11 : tmpInts[1] - 1);
				tmpInts[2] = 1;
			}
			if (cursorX == 13){
				//Day
				(dir == 1) ? (tmpInts[2] = (tmpInts[2] + 1 > maxDaysInMonth) ? 1 : tmpInts[2] + 1) : (tmpInts[2] = (tmpInts[2] - 1 < 1) ? maxDaysInMonth : tmpInts[2] - 1);
			}
			if (cursorX == 1){
				//Hour
				(dir == 1) ? (tmpInts[4] = (tmpInts[4] + 1 > 24) ? 1 : tmpInts[4] + 1) : (tmpInts[4] = (tmpInts[4] - 1 < 1) ? 24 : tmpInts[4] - 1);
			}
			if (cursorX == 4){
				//Min
				(dir == 1) ? (tmpInts[5] = (tmpInts[5] + 1 > 59) ? 0 : tmpInts[5] + 1) : (tmpInts[5] = (tmpInts[5] - 1 < 0) ? 59 : tmpInts[5] - 1);
			}
		}
		else{
			if (cursorX == 3){
				//Year
				tmpInts[0] = (dir == 1) ? tmpInts[0] + 1 : tmpInts[0] - 1;
			}
		}
	}

	lcd.clear();
	captureDateTimeDisplays();
	char monthsBuffer[8];

	lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[1]]))) + F(" ") + tmpDisplay[1]);
	lcd.setCursor(0, 1);
	lcd.print(String(tmpInts[0]) + F(" <back> <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

//Saves
void saveDateTime(){
	if (cursorX == 13 && cursorY == 1){
		//hour, min, seconds
		rtc.setTime(tmpInts[4], tmpInts[5], 0);
		//day, month (RTC counts first month as 1, not 0), year
		rtc.setDate(tmpInts[2], tmpInts[1] + 1, tmpInts[0]);
		rtc.setDOW(calcDayOfWeek(tmpInts[0], tmpInts[1], tmpInts[2]));
	}
	if (cursorX == 6 || cursorX == 13 && cursorY == 1){
		tmpDisplay[0] = ""; //suffix
		tmpDisplay[1] = ""; //hour
		tmpDisplay[2] = ""; //min
		tmpDisplay[3] = ""; //day
		exitScreen();
	}
}

//Helpers
void captureDateTimeDisplays(int month = tmpInts[1], int day = tmpInts[2], int hour = tmpInts[4], int min = tmpInts[5]){
	byte i, maxDaysInMonth, hourConversion;

	maxDaysInMonth = days[month];
	hourConversion = (hour == 0) ? 12 : hour;

	//Thanks Romans...
	int hoursKey[12][2] = {
		{ 13, 1 }, { 14, 2 }, { 15, 3 }, { 16, 4 }, { 17, 5 }, { 18, 6 }, { 19, 7 }, { 20, 8 }, { 21, 9 }, { 22, 10 }, { 23, 11 }, { 24, 12 }
	};
	if (hourConversion > 12){
		for (i = 0; i < 12; i++){
			if (hourConversion == hoursKey[i][0]){
				hourConversion = hoursKey[i][1];
			}
		}
	}

	//day suffix
	tmpDisplay[0] = (day == 1 || day == 21 || day == 31) ? "st" : (day == 2 || day == 22) ? "nd" : (day == 3 || day == 23) ? "rd" : "th";
	//day
	tmpDisplay[1] = (day > maxDaysInMonth) ? "01" + String(tmpDisplay[0]) : (day < 10) ? "0" + String(day) + String(tmpDisplay[0]) : String(day) + String(tmpDisplay[0]);
	//hour
	tmpDisplay[2] = (hourConversion < 10) ? "0" + String(hourConversion) : String(hourConversion);
	//minute
	tmpDisplay[3] = (min < 10) ? "0" + String(min) : String(min);
	//AM/PM
	tmpDisplay[4] = (hour >= 12 && hour < 24) ? F("PM") : F("AM");
}
int calculateDayOfYear(int day, int month, int year) {
	// Given a day, month, and year (4 digit), returns 
	// the day of year. Errors return 999.
	byte i;
	byte daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// Verify we got a 4-digit year
	if (year < 1000) {
		return 999;
	}

	// Check if it is a leap year, this is confusing business
	if (year % 4 == 0) {
		if (year % 100 != 0) {
			daysInMonth[1] = 29;
		}
		else {
			if (year % 400 == 0) {
				daysInMonth[1] = 29;
			}
		}
	}

	// Make sure we are on a valid day of the month
	if (day < 1)
	{
		return 999;
	}
	else if (day > daysInMonth[month - 1]) {
		return 999;
	}

	byte doy = 0;
	for (i = 0; i < month - 1; i++) {
		doy += daysInMonth[i];
	}

	doy += day;
	return doy;
}
byte calcDayOfWeek(unsigned int y, byte m, byte d)
{
	// cast out multiples of 400 from y
	y -= (2000 * (y >> 11));
	while (y >= 400) y -= 400;

	boolean leap = ((y & 3) == 0);
	if ((y == 100) || (y == 200) || (y == 300)) leap = false;

	if (d > 31 || d == 0) return 0;

	byte w = 6;
	while (y >= 100) { y -= 100; w -= 2; }

	w += (y + (y >> 2));

	// correction for Jan. and Feb. of leap year
	if (leap && (m <= 2)) w--;

	// using subtraction iso addition makes the while at end possible 1 iteration faster.
	switch (m)
	{
	case 1:
		w++;
		break;
	case 2:
		if (d > (leap ? 29 : 28)) return 0;
		w += 4;
		break;
	case 3:
		w += 4;
		break;
	case 5:
		w += 2;
		break;
	case 7:
		break;
	case 8:
		w += 3;
		break;
	case 10:
		w++;
		break;
	case 12:
		w += 6;
		break;
	default:
		if (d > 30) return 0;
		switch (m)
		{
		case 4:
			break;
		case 6:
			w += 5;
			break;
		case 9:
			w += 6;
			break;
		case 11:
			w += 4;
			break;
		default:
			return 0;
		}
	}

	w += d;

	// there are only 7 days in a week, so we "cast out" sevens
	while (w > 7) w = (w >> 3) + (w & 7);
	return w;
}