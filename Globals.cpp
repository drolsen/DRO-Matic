#include "Globals.h"
#include "Channels.h"
#include "Sessions.h"
#include "Menus.h"
#include "DateTime.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DS3231  rtc(SDA, SCL);
Stepper myStepper(stepsPerRevolution, 15, 14);

File tmpFile;
String cropName, screenName;
String nameArry[15];
String tmpDisplay[5]; //suffix, hour, min, day
vector<String> menus;
vector<String> menusHistory;
vector<vector<vector<byte>>> matrix;


const int stepsPerRevolution = 100;
const int stepperSpeed = 800;

unsigned long previousMillis, currentMillis;  //stores last time

int Key, menuIndex, currentMinute;
int minPPM = 1200;
int maxPPM = 1600;
int tmpInts[6];
float tmpFloats[2];
int days[12] = { 31, ((tmpInts[5] % 4 == 0 && tmpInts[5] % 100 != 0) || (tmpInts[5] % 400 == 0)) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int cursorX, cursorY;
int currentChannelIndex, currentSessionIndex;
int currentAlphaIndex = 0;

byte upArrow[8] = {
	B00000,
	B00100,
	B01110,
	B11111,
	B00100,
	B00100,
	B00100,
	B00000
};

byte downArrow[8] = {
	B00000,
	B00100,
	B00100,
	B00100,
	B11111,
	B01110,
	B00100,
	B00000
};

const char blank[2] PROGMEM = " ";
const char a[2] PROGMEM = "A";
const char b[2] PROGMEM = "B";
const char c[2] PROGMEM = "C";
const char d[2] PROGMEM = "D";
const char e[2] PROGMEM = "E";
const char f[2] PROGMEM = "F";
const char g[2] PROGMEM = "G";
const char h[2] PROGMEM = "H";
const char i[2] PROGMEM = "I";
const char j[2] PROGMEM = "J";
const char k[2] PROGMEM = "K";
const char l[2] PROGMEM = "L";
const char m[2] PROGMEM = "M";
const char n[2] PROGMEM = "N";
const char o[2] PROGMEM = "O";
const char p[2] PROGMEM = "P";
const char q[2] PROGMEM = "Q";
const char r[2] PROGMEM = "R";
const char s[2] PROGMEM = "S";
const char t[2] PROGMEM = "T";
const char u[2] PROGMEM = "U";
const char v[2] PROGMEM = "V";
const char w[2] PROGMEM = "W";
const char x[2] PROGMEM = "X";
const char y[2] PROGMEM = "Y";
const char z[2] PROGMEM = "Z";
const char zero[2] PROGMEM = "0";
const char one[2] PROGMEM = "1";
const char two[2] PROGMEM = "2";
const char three[2] PROGMEM = "3";
const char four[2] PROGMEM = "4";
const char five[2] PROGMEM = "5";
const char six[2] PROGMEM = "6";
const char seven[2] PROGMEM = "7";
const char eight[2] PROGMEM = "8";
const char nine[2] PROGMEM = "9";

const char* const alphabet[37] PROGMEM = {
	blank, a, b, c, d, e, f, g, h, i, j, k, l, m,
	n, o, p, q, r, s, t, u, v, w, x, y, z, zero,
	one, two, three, four, five, six, seven, eight, nine
};

const char sysconf[8] PROGMEM = "SYSCONF";
const char System[7] PROGMEM = "SYSTEM";
const char settings[9] PROGMEM = "SETTINGS";
const char sessions[9] PROGMEM = "SESSIONS";
const char session[8] PROGMEM = "SESSION";
const char channel[8] PROGMEM = "CHANNEL";
const char channels[9] PROGMEM = "CHANNELS";
const char numberOf[10] PROGMEM = "NUMBER OF";
const char dateTime[12] PROGMEM = "DATE & TIME";
const char configuration[14] PROGMEM = "CONFIGURATION";
const char calibration[12] PROGMEM = "CALIBRATION";
const char ppmRange[13] PROGMEM = "EC/PPM RANGE";
const char phRange[9] PROGMEM = "PH RANGE";
const char cropLoad[10] PROGMEM = "LOAD CROP";
const char newCrop[9] PROGMEM = "NEW CROP";
const char deleteCrop[12] PROGMEM = "DELETE CROP";
const char sizeMl[12] PROGMEM = "SIZE(ml) OF";
const char amountMl[11] PROGMEM = "AMOUNT(ml)";
const char sessionStart[14] PROGMEM = "SESSION START";
const char sessionDelay[14] PROGMEM = "SESSION DELAY";
const char sessionRepeat[15] PROGMEM = "SESSION REPEAT";
const char datetime[9] PROGMEM = "DATETIME";
const char choconf[8] PROGMEM = "CHCONF";
const char chnum[6] PROGMEM = "CHNUM";
const char ppm[4] PROGMEM = "PPM";
const char ph[3] PROGMEM = "PH";
const char Open[5] PROGMEM = "OPEN";
const char New[8] PROGMEM = "NEWCROP";
const char Delete[7] PROGMEM = "DELETE";
const char chdoses[8] PROGMEM = "CHDOSES";
const char chcalib[8] PROGMEM = "CHCALIB";
const char chsize[7] PROGMEM = "CHSIZE";
const char amt[4] PROGMEM = "AMT";
const char str[12] PROGMEM = "STR";
const char dly[12] PROGMEM = "DLY";
const char rpt[12] PROGMEM = "RPT";

const char* const displayNames[18][3] PROGMEM = {
	{ sysconf, System, settings },
	{ channels, System, channels },
	{ datetime, dateTime, configuration },
	{ choconf, channel, settings },
	{ chnum, numberOf, channels },
	{ ppm, ppmRange, configuration },
	{ ph, phRange, configuration },
	{ Open, cropLoad, blank },
	{ New, newCrop, blank },
	{ Delete, deleteCrop, blank },
	{ chdoses, numberOf, sessions },
	{ chsize, sizeMl, channel },
	{ chcalib, channel, calibration },
	{ sessions, channel, sessions },
	{ amt, session, amountMl },
	{ str, sessionStart, dateTime },
	{ dly, sessionDelay, configuration },
	{ rpt, sessionRepeat, configuration }
};

const char jan[4] PROGMEM = "Jan";
const char feb[4] PROGMEM = "Feb";
const char mar[4] PROGMEM = "Mar";
const char apl[4] PROGMEM = "Apl";
const char may[4] PROGMEM = "May";
const char jun[4] PROGMEM = "Jun";
const char jul[4] PROGMEM = "Jul";
const char aug[4] PROGMEM = "Aug";
const char sep[4] PROGMEM = "Sep";
const char oct[4] PROGMEM = "Oct";
const char nov[4] PROGMEM = "Nov";
const char dec[4] PROGMEM = "Dec";

const char* const months[12] PROGMEM = { jan, feb, mar, apl, may, jun, jul, aug, sep, oct, nov, dec };

const char none[5] PROGMEM = "None";
const char hourly[7] PROGMEM = "Hourly";
const char daily[6] PROGMEM = "Daily";
const char weekly[7] PROGMEM = "Weekly";
const char monthly[8] PROGMEM = "Monthly";
const char yearly[7] PROGMEM = "Yearly";

const char* const displayRepeats[6] PROGMEM = { none, hourly, daily, weekly, monthly, yearly };


void makeNewFile(String path, JsonObject& data){
	char buffer[1024];
	tmpFile = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}