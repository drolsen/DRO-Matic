/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - September 11, 2016
*  devin@devinrolsen.com
*/

#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <StandardCplusplus.h>
#include <StandardCplusplus\vector>
#include <StandardCplusplus\ctime>
#include <ArduinoJson\ArduinoJson.h>
#include <DS3231.h>
#include <avr/pgmspace.h>
#include <MemoryFree.h>
DS3231  rtc(SDA, SCL);
using namespace std;

//Global vars
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int tmpInts[6];
float tmpFloats[2];
String tmpDisplay[4]; //suffix, hour, min, day

int Key;
unsigned long previousMillis = 0;  //stores last time

String cropName;
String nameArry[15];
String screenName;
int cursorX;
int cursorY;
int menuIndex;
int minPPM;
int maxPPM;

int days[12] = { 31, ((tmpInts[5] % 4 == 0 && tmpInts[5] % 100 != 0) || (tmpInts[5] % 400 == 0)) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
char* meridiem;

int currentChannelIndex;
int currentSessionIndex;
int currentAlphaIndex = 0;

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

const char jan[] PROGMEM = "Jan";
const char feb[] PROGMEM = "Feb";
const char mar[] PROGMEM = "Mar";
const char apl[] PROGMEM = "Apl";
const char may[] PROGMEM = "May";
const char jun[] PROGMEM = "Jun";
const char jul[] PROGMEM = "Jul";
const char aug[] PROGMEM = "Aug";
const char sep[] PROGMEM = "Sep";
const char oct[] PROGMEM = "Oct";
const char nov[] PROGMEM = "Nov";
const char dec[] PROGMEM = "Dec";

const char* const months[12] PROGMEM = { jan, feb, mar, apl, may, jun, jul, aug, sep, oct, nov, dec };


const char hly[5] PROGMEM = "Hrly";
const char daly[5] PROGMEM = "Daly";
const char wly[5] PROGMEM = "Wkly";
const char mly[5] PROGMEM = "Mtly";
const char yrl[5] PROGMEM = "Yrly";

const char* const displayRepeats[5] PROGMEM = { hly, daly, wly, mly, yrl };

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

vector<String> menus;
vector<String> menusHistory;
vector<vector<vector<int>>> matrix;
int sessionValues[10][12];
File tmpFile;


//Data passing functions for core and crop
JsonObject& getCoreData(){
	StaticJsonBuffer<512> b;
	tmpFile = SD.open("core.dro");
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setCoreData(JsonObject& d){
	char b[128];
	tmpFile = SD.open("core.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

JsonObject& getCropData(){
	DynamicJsonBuffer b;
	tmpFile = SD.open(cropName + "/crop.dro");
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setCropData(JsonObject& d){
	char b[256];
	tmpFile = SD.open(cropName + "/crop.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

JsonObject& getChannelData(){
	DynamicJsonBuffer b;
	tmpFile = SD.open(cropName + "/channels/sysch" + currentChannelIndex + "/channel.dro", FILE_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setChannelData(JsonObject& d){
	char b[256];
	tmpFile = SD.open(cropName + "/channels/sysch" + currentChannelIndex + "/channel.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

JsonObject& getSessionData(){
	DynamicJsonBuffer b;
	tmpFile = SD.open(cropName + "/channels/sysch" + currentChannelIndex + "/sessions/chses" + currentSessionIndex + "/session.dro", FILE_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setSessionData(JsonObject& d){
	char b[256];
	tmpFile = SD.open(cropName + "/channels/sysch" + currentChannelIndex + "/sessions/chses" + currentSessionIndex + "/session.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

void setup()
{
	lcd.createChar(0, upArrow);
	lcd.createChar(1, downArrow);
	lcd.begin(16, 2);
	Serial.begin(9600);
	rtc.begin();
	captureDateTime();

	if (!SD.begin(53)){
		lcd.print(F("SD Card Required"));
		lcd.setCursor(0, 1);
		lcd.print(F("Insert And Rest"));
		screenName = "REQUIREDSD";
	}else{
		coreInit();
	}
}

void loop()
{
	Key = analogRead(0);
	unsigned long currentMillis = millis();
	if (Key >= 0 && Key <= 650){
		previousMillis = currentMillis;
	}
	if (currentMillis - previousMillis >= 10000 && screenName == "") {
		previousMillis = currentMillis;
		openHomeScreen();
		//turing();
	}

	if (Key == 0 || Key == 408){
		//Right & Left
		if (screenName == "NewCrop"){
			matrix = {
				{ { 0, 15 } },
				{ { 11, 11 } }
			};
			if (Key == 0){
				renameCrop(NULL);
				lcd.blink();
			}
		}
		if (screenName == "DATETIME" || screenName == "STR"){
			matrix = {
				{ { 1, 1 }, { 4, 4 }, { 10, 10 }, { 13, 13 } },
				{ { 3, 3 }, { 6, 6 }, { 13, 13 } }
			};
		}
		if (screenName == "PPM"){
			matrix = {
				{ { 3, 3 }, { 8, 8 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "PH"){
			matrix = {
				{ { 3, 3 }, { 9, 9 } },
				{ { 1, 1 }, { 11, 11 } }
			};
		}
		if (screenName == "PHCHL"){
			matrix = {
				{ { 6, 6 }, { 14, 14 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHNUM" || screenName == "CHDOSES"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHSIZE" || screenName == "AMT" || screenName == "DLY"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHCALIB"){
			matrix = {
				{ { 10, 10 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "RPT"){
			matrix = {
				{ { 13, 13 } },
				{ { 2, 2 }, { 6, 6 }, { 13, 13 } }
			};
		}
		cursorX = (Key == 0) ? cursorX + 1 : cursorX - 1;
		if (Key == 408 && screenName == ""){
			menusHistory.pop_back();
			menuIndex = 0;
			tmpFile = SD.open(cropName + "/" + getMenuHistory());
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			lcd.clear();
			printDisplayNames(menus.front());
			printScrollArrows();
		}
		if (Key == 0 || Key == 408 && screenName != ""){
			screenMatrix();
		}
		delay(250);
	}
	if (Key == 99 || Key == 255) {
		//Up & Down
		int dir = (Key == 99) ? 1 : -1;
		int lgdir = (Key == 99) ? 10 : -10;
		if (screenName == "NewCrop"){
			renameCrop(dir);
		}
		if (screenName == "PPM"){
			setPPMRangeValues(dir);
		}
		if (screenName == "PH"){
			setPHRange(dir);
		}
		if (screenName == "PHCHL"){
			setPHChannels(dir);
		}
		if (screenName == "DATETIME"){
			setDateTime(dir);
		}
		if (screenName == "CHNUM"){
			setChannelNumber(dir);
		}
		if (screenName == "CHDOSES"){
			setSessionNumber(dir);
		}
		if (screenName == "CHSIZE"){
			setChannelSize(lgdir);
		}
		if (screenName == "CHCALIB"){
			setCalibrationSize(dir);
		}
		if (screenName == "AMT"){
			setSessionAmount(lgdir);
		}
		if (screenName == "STR"){
			setDateTime(dir);
		}
		if (screenName == "DLY"){
			setSessionDelay(dir);
		}
		if (screenName == "RPT"){
			setSessionRepeat(dir);
		}
		if (screenName == ""){
			scrollMenus(dir);
		}
		delay(250);
	}
	if (Key == 639) {
		//Select
		if (screenName == ""){
			lcd.clear();
			String history = getMenuHistory();
			tmpFile = SD.open(cropName + "/" + history + "/" + menus[menuIndex]);
			menusHistory.push_back(menus[menuIndex]);
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			if (menus.size() > 0){
				menuIndex = 0;
				lcd.clear();
				tmpFile.close();
				printDisplayNames(menus.front());
				printScrollArrows();
			} else {
				screenName = menusHistory.back();
				if (screenName == "DATETIME"){
					lcd.blink();
					lcd.clear();
					lcd.home();
					captureDateTime();
					char monthsBuffer[8];
					
					lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + meridiem + " " + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[4]]))) + " " + String(tmpDisplay[3]));
					lcd.setCursor(0, 1);
					lcd.print(String(tmpInts[5]) + F(" <back> <ok>"));
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "NEWCROP"){
					startNewCrop();
					menus.clear();
					menusHistory.clear();
					currentAlphaIndex = 0;
					currentChannelIndex = 0;
					currentSessionIndex = 0;
					menuIndex = 0;
					cursorX = cursorY = 0;
					lcd.home();
				}
				if (screenName == "PPM"){
					lcd.blink();
					lcd.clear();
					lcd.home();
					lcd.print(String(minPPM) + "-" + String(maxPPM) + F(" EC/PPM"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "PH"){
					lcd.blink();
					lcd.clear();
					lcd.home();
					JsonObject& data = getCropData();
					tmpFloats[0] = data["ph"].asArray()[0];
					tmpFloats[1] = data["ph"].asArray()[1];
					lcd.print(String(tmpFloats[0]));
					lcd.write(byte(1));
					lcd.print(" " + String(tmpFloats[1]));
					lcd.write(byte(0));
					lcd.print(F(" PH"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>    <next>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "CHNUM"){
					lcd.clear();
					lcd.home();
					cursorX = 1;
					cursorY = 0;
					JsonObject& data = getCropData();
					int total = data["totalChannels"];
					tmpInts[0] = total;
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" CHANNELS"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
				if (screenName == "CHDOSES"){
					lcd.clear();
					lcd.home();
					cursorX = 1;
					cursorY = 0;
					JsonObject& data = getChannelData();
					tmpInts[0] = data["sessionsTotal"];
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" # OF SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					//delay(5000);
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
				if (screenName == "CHSIZE"){
					lcd.clear();
					JsonObject& data = getChannelData();
					tmpInts[0] = data["size"];
					lcd.clear();
					String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(channelSize + F(" (ml) volume"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
				if (screenName == "CHCALIB"){
					lcd.clear();
					lcd.home();
					JsonObject& data = getChannelData();
					tmpInts[0] = data["size"];
					tmpInts[1] = data["calibration"];
					String targetSize = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);
					String rotsSize = (tmpInts[1] >= 10 && tmpInts[1] <= 99) ? "0" + String(tmpInts[1]) : (tmpInts[1] < 10 && tmpInts[1] >= 0) ? "00" + String(tmpInts[1]) : String(tmpInts[1]);
					lcd.print(targetSize + F("(ml) ") + rotsSize + F(" rots"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.blink();
					cursorX = 10;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "AMT"){
					lcd.clear();
					lcd.home();
					JsonObject& data = getSessionData();
					tmpInts[0] = data["amount"];
					String displayAmount = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);

					lcd.print(displayAmount + F("(ml) volume"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
				if (screenName == "STR"){
					lcd.clear();
					lcd.home();
					captureDateTime();
					char monthsBuffer[8];
					lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + meridiem + " " + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[4]]))) + " " + String(tmpDisplay[3]));
					lcd.setCursor(0, 1);
					lcd.print(String(tmpInts[5]) + F(" <back> <ok>"));
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
					lcd.blink();
				}
				if (screenName == "DLY"){
					lcd.clear();
					lcd.home();
					JsonObject& data = getSessionData();
					tmpInts[0] = data["delay"];
					String displayDelay = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);

					lcd.print(displayDelay + F("(sec)  delay"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
				if (screenName == "RPT"){
					lcd.clear();
					lcd.home();
					JsonObject& data = getSessionData();
					tmpInts[0] = data["repeat"];
					tmpInts[1] = data["repeatBy"];

					String displayRepeat = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);
					String displayRepeatBy = displayRepeats[tmpInts[1]];

					lcd.print("Repeated: " + displayRepeatBy);
					lcd.setCursor(0, 1);
					lcd.print(displayRepeat + F("x <back> <ok>"));
					cursorX = 13;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
			}
			delay(350);
		}
		//Saves
		if (screenName == "NewCrop"){
			if (cursorX == 11 && cursorY == 1){
				buildCrop();
			}
		}
		if (screenName == "PPM"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getCropData();
				data["ppm"].asArray()[0] = minPPM;
				data["ppm"].asArray()[1] = maxPPM;
				setCropData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "PH"){
			if (cursorX == 11 && cursorY == 1){
				JsonObject& data = getCropData();
				data["minPH"] = tmpFloats[0];
				data["maxPH"] = tmpFloats[1];
				setCropData(data);
				tmpFloats[0] = tmpFloats[1] = 0.0;
				screenName = "PHCHL";
				delay(250);

				lcd.clear();
				lcd.home();
				tmpInts[0] = data["phChannels"].asArray()[0];
				tmpInts[1] = data["phChannels"].asArray()[1];

				String UpDisplay;
				String DownDisplay;
				UpDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
				DownDisplay = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);
				lcd.print(F("PH"));
				lcd.write(byte(1));
				lcd.print(F("CH"));
				lcd.print(UpDisplay);
				lcd.print(F(" PH"));
				lcd.write(byte(0));
				lcd.print("CH" + DownDisplay);
				lcd.setCursor(0, 1);
				lcd.print(F("<back>      <ok>"));
				cursorX = 6;
				cursorY = 0;
				lcd.setCursor(6, 0);
			}
			if (cursorX == 1 && cursorY == 1){
				tmpFloats[0] = tmpFloats[1] = 0.0;
				screenName = "";
				exitScreen();
			}
		}
		if (screenName == "PHCHL"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getCropData();
				data["phChannels"].asArray()[0] = tmpInts[0];
				data["phChannels"].asArray()[1] = tmpInts[1];
				setCropData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = tmpInts[1] = 0;
				exitScreen();
			}
		}
		if (screenName == "DATETIME"){
			if (cursorX == 13 && cursorY == 1){
				rtc.setTime(tmpInts[2], tmpInts[1], 0);
				rtc.setDate(tmpInts[3], tmpInts[4], tmpInts[5]);
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				tmpDisplay[0] = ""; //suffix
				tmpDisplay[1] = ""; //hour
				tmpDisplay[2] = ""; //min
				tmpDisplay[3] = ""; //day
				exitScreen();
			}
		}
		if (screenName == "CHNUM"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getCropData();
				int total = data["totalChannels"];
				data["totalChannels"] = tmpInts[0];
				setCropData(data);
				if (total + 1 > tmpInts[0]){ trimChannels(total + 1, tmpInts[0]); }
				if (total < tmpInts[0]){ addChannels(total + 1, tmpInts[0]); }
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHDOSES"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getChannelData();
				if (data["sessionsTotal"] > tmpInts[0]){ //we are adding sessions
					trimSessions(data["sessionsTotal"], tmpInts[0]);
				}else if (data["sessionsTotal"] < tmpInts[0]){ //we are removing sessions
					addSessions(data["sessionsTotal"], tmpInts[0]);
				}
				data["sessionsTotal"] = tmpInts[0];
				setChannelData(data);

				JsonObject& cropData = getCropData();
				cropData["sessionIds"].asArray()[currentChannelIndex - 1].asArray()[1] = tmpInts[0];
				setCropData(cropData);
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHSIZE"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getChannelData();
				data["size"] = tmpInts[0];
				setChannelData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHCALIB"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getChannelData();
				data["calibration"] = tmpInts[1];
				setChannelData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[1] = 0;
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "AMT"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getSessionData();
				data["amount"] = tmpInts[0];
				setSessionData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "STR"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getSessionData();
				data.createNestedArray("date");
				data.createNestedArray("time");
				JsonArray& time = data["time"];
				JsonArray& date = data["date"];

				time.add(tmpInts[0]);
				time.add(tmpInts[1]);
				time.add(tmpInts[2]);

				date.add(tmpInts[3]);
				date.add(tmpInts[4]);
				date.add(tmpInts[5]);

				tm time_in = { tmpInts[0], tmpInts[1], tmpInts[2], // second, minute, hour
					tmpInts[3], tmpInts[4], tmpInts[5] - tmpInts[5] }; // 1-based day, 0-based month, year since 1900
				time_t time_temp = mktime(&time_in);
				tm const *time_out = localtime(&time_temp);
				date.add(time_out->tm_wday);
				setSessionData(data);
				tmpDisplay[0] = tmpDisplay[1] = tmpDisplay[2] = tmpDisplay[3] = ""; //suffix hour min day

			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				menusHistory.pop_back();
				menuIndex = 0;
				screenName = "";
				tmpFile = SD.open(cropName + "/" + getMenuHistory());
				getDirectoryMenus(tmpFile);
				lcd.clear();
				lcd.noBlink();
				tmpFile.close();
				printDisplayNames(menus.front());
				printScrollArrows();
				delay(350);
			}
		}
		if (screenName == "DLY"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getSessionData();
				data["delay"] = tmpInts[0];
				setSessionData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "RPT"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getSessionData();
				data["repeatBy"] = tmpInts[1];
				data["repeat"] = tmpInts[0];
				setSessionData(data);
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				tmpInts[1] = 0;
				tmpInts[0] = 0;
				exitScreen();
			}
		}
	}
}

void openHomeScreen(){
	captureDateTime();
	lcd.clear();
	char monthsBuffer[8];
	
	lcd.print(tmpDisplay[1] + ":" + tmpDisplay[2] + meridiem + " " + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[rtc.getTime().mon]))) + " " + tmpDisplay[3]);
	lcd.setCursor(0, 1);
	lcd.print(F("PPM:"));
	lcd.print(String(analogRead(9)));
	lcd.print(F(" PH:"));
	lcd.print(String(analogRead(10)));
	lcd.home();
	lcd.noBlink();
	turing(); //the heart of it all, thank you Allen
}

void screenMatrix(){
	int row0Size = matrix[0].size();
	int row1Size = matrix[1].size();

	int frontFrontFront = matrix.front().front().front();
	int frontBackBack = matrix.front().back().back();

	int backBackBack = matrix.back().back().back();
	int backFrontFront = matrix.back().front().front();
	//Check Top Row
	if (cursorY == 0){
		if (cursorX < frontFrontFront){
			cursorX = backBackBack;
			cursorY = 1;
		}
		else if (cursorX > frontBackBack){
			cursorY = 1;
			cursorX = backFrontFront;
		}
		for (int i = 0; i < row0Size; i++){ //top columns
			if (cursorX < matrix.front()[i].front() && cursorX > matrix.front()[i - 1].back() && Key == 408 && cursorY == 0){
				cursorX = matrix.front()[i - 1].back();
			}
			if (cursorX > matrix.front()[i].back() && cursorX < matrix.front()[i + 1].front() && Key == 0 && cursorY == 0){
				cursorX = matrix.front()[i + 1].front();
			}
		}
	}
	else if (cursorY == 1){
		if (cursorX < backFrontFront){
			cursorX = frontBackBack;
			cursorY = 0;
		}
		else if (cursorX > backBackBack){
			cursorY = 0;
			cursorX = frontFrontFront;
		}
		for (int i = 0; i < row1Size; i++){ //top columns
			if (cursorX < matrix.back()[i].front() && cursorX > matrix.back()[i - 1].back() && Key == 408 && cursorY == 1){
				cursorX = matrix.back()[i - 1].back();
			}
			if (cursorX > matrix.back()[i].back() && cursorX < matrix.back()[i + 1].front() && Key == 0 && cursorY == 1){
				cursorX = matrix.back()[i + 1].front();
			}
		}
	}
	lcd.setCursor(cursorX, cursorY);
}

void coreInit(){
	JsonObject& coreData = getCoreData();
	if (coreData.success()){ //load core settings / crop
		cropName = coreData["crop"].asString();
		if (SD.exists(cropName)){
			//Loading up exisiting core file's crop directory
			screenName = "";
			loadCrop();
			File cropFile = SD.open("/" + cropName);
			getDirectoryMenus(cropFile);
			cropFile.close();
		}
		else{
			//we have core file with crop, but no crop directory. //VERY CORNER CASE!
			startNewCrop();
		}
	} else { //else, setup new crop
		tmpFile = SD.open("core.dro", FILE_WRITE);
		char buffer[32];
		DynamicJsonBuffer coreBuffer;
		JsonObject& settings = coreBuffer.createObject();
		settings["crop"];
		settings.createNestedArray("crops");
		settings.printTo(buffer, sizeof(buffer));
		tmpFile.print(buffer);
		tmpFile.close();

		lcd.print(F("New Drive Found!"));
		lcd.setCursor(0, 1);
		lcd.print(F(" Please Hold... "));
		delay(1000);
		lcd.clear();
		lcd.home();
		lcd.print(F("Setup New Crop"));
		delay(1000);
		lcd.clear();
		startNewCrop();
	}
}

void startNewCrop(){
	screenName = "NewCrop";
	lcd.clear();
	lcd.setCursor(0, 1);
	lcd.print(F("Crop Name <done>"));
	lcd.home();
	lcd.blink();
}

void scrollAlpha(int dir){
	currentAlphaIndex = currentAlphaIndex + dir;
	if (currentAlphaIndex > 36){
		currentAlphaIndex = 0;
	}
	else if (currentAlphaIndex < 0){
		currentAlphaIndex = 36;
	}
	char alphaBuffer[2];
	lcd.print(strcpy_P(alphaBuffer, (char*)pgm_read_word(&(alphabet[currentAlphaIndex]))));
	lcd.setCursor(cursorX, cursorY);
	delay(50);
}

void scrollMenus(int dir){
	menuIndex = menuIndex + dir;
	int currentSize = menus.size() - 1;
	if (menuIndex < 0){
		menuIndex = currentSize;
	}
	else if (menuIndex > currentSize){
		menuIndex = 0;
	}
	lcd.clear();
	printDisplayNames(menus[menuIndex]);
	printScrollArrows();
}

void exitScreen(){
	menusHistory.pop_back();
	menuIndex = 0;
	screenName = "";
	tmpFile = SD.open(cropName + "/" + getMenuHistory());
	getDirectoryMenus(tmpFile);
	lcd.clear();
	lcd.noBlink();
	tmpFile.close();
	printDisplayNames(menus.front());
	printScrollArrows();
	delay(350);
}

void printScrollArrows(){
	lcd.setCursor(15, 0);
	lcd.write(byte(0));
	lcd.setCursor(15, 15);
	lcd.write(byte(1));
	lcd.home();
}

void getDirectoryMenus(File dir) {
	menus.clear();
	while (true) {
		tmpFile = dir.openNextFile();
		if (!tmpFile) {
			// no more files
			break;
		}
		if (tmpFile.isDirectory()){
			menus.push_back(tmpFile.name());
		}
		tmpFile.close();
	}
}

String getMenuHistory(){
	String join;
	int size = menusHistory.size();
	for (int i = 0; i < size; i++){
		join = join + "/" + menusHistory.at(i);
	}
	delay(10); //fixes race condtion with PROGMEM strings
	return join;
}

void printDisplayNames(String menu){
	lcd.home();
	bool hasMatch = false;
	const int isChannel = strstr(menu.c_str(), "SYSCH") != NULL;
	const int isSession = strstr(menu.c_str(), "CHSES") != NULL;
	const String index = String(menuIndex + 1);
	if (isChannel){
		lcd.print(F("SYSTEM"));
		lcd.setCursor(0, 1);
		lcd.print(F("CHANNEL "));
		lcd.print(index);
		currentChannelIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}
	else if (isSession){
		lcd.print(F("CHANNEL"));
		lcd.setCursor(0, 1);
		lcd.print(F("SESSION "));
		lcd.print(index);
		currentSessionIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}
	else{
		for (int i = 0; i < 18; i++){
			char match1Buffer[18];
			char match2Buffer[18];
			char match3Buffer[18];
			String match1 = strcpy_P(match1Buffer, (char*)pgm_read_word(&(displayNames[i][0])));
			String match2 = strcpy_P(match2Buffer, (char*)pgm_read_word(&(displayNames[i][1])));
			String match3 = strcpy_P(match2Buffer, (char*)pgm_read_word(&(displayNames[i][2])));
			
			if (menu == match1){
				hasMatch = true;
				lcd.print(match2);
				lcd.setCursor(0, 1);
				lcd.print(match3);
				break;
			}
		}
	}
	if (hasMatch == false){
		lcd.print(menu);
	}
}

void renameCrop(int dir){
	cropName = "";
	if (dir != NULL){
		scrollAlpha(dir);
		char alphaBuffer[2];
		nameArry[cursorX] = strcpy_P(alphaBuffer, (char*)pgm_read_word(&(alphabet[currentAlphaIndex])));
	}
	//Compile/Collapse
	for (int i = 0; i < 15; i++){
		cropName = cropName + nameArry[i];
	}
	lcd.setCursor(cursorX, cursorY);
}

void makeNewFile(String path, JsonObject& data){
	char buffer[512];
	tmpFile = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}

void setPPMRangeValues(int dir){
	if ((dir == 1) ? cursorX == 3 : cursorX == 8){
		(dir == 1) ? minPPM = minPPM + 10 : maxPPM = maxPPM - 10;
		if ((dir == 1) ? minPPM > (maxPPM - 50) : maxPPM < (minPPM + 50)){
			(dir == 1) ? maxPPM = maxPPM + 10 : minPPM = minPPM - 10;
		}
	}
	if ((dir == 1) ? cursorX == 8 : cursorX == 3) {
		(dir == 1) ? maxPPM = maxPPM + 10 : minPPM = minPPM - 10;
	}

	lcd.clear();
	lcd.print(String(minPPM) + "-" + String(maxPPM) + F(" EC/PPM"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}

void setDateTime(int dir){
	int maxDaysInMonth = days[tmpInts[4]];
	if (cursorY == 0){
		if (cursorX == 1){
			(dir == 1) ? (tmpInts[2] = (tmpInts[2] + 1 > 24) ? 1 : tmpInts[2] + 1) : (tmpInts[2] = (tmpInts[2] - 1 < 1) ? 24 : tmpInts[2] - 1);
		}
		if (cursorX == 4){
			(dir == 1) ? (tmpInts[1] = (tmpInts[1] + 1 > 59) ? 0 : tmpInts[1] + 1) : (tmpInts[1] = (tmpInts[1] - 1 < 0) ? 59 : tmpInts[1] - 1);
		}
		if (cursorX == 10){
			(dir == 1) ? (tmpInts[4] = (tmpInts[4] + 1 > 11) ? 0 : tmpInts[4] + 1) : (tmpInts[4] = (tmpInts[4] - 1 < 0) ? 11 : tmpInts[4] - 1);
			tmpInts[3] = 1;
		}
		if (cursorX == 13){
			(dir == 1) ? (tmpInts[3] = (tmpInts[3] + 1 > maxDaysInMonth) ? 1 : tmpInts[3] + 1) : (tmpInts[3] = (tmpInts[3] - 1 < 1) ? maxDaysInMonth : tmpInts[3] - 1);
		}
	}
	else{
		if (cursorX == 3){
			tmpInts[5] = (dir == 1) ? tmpInts[5] + 1 : tmpInts[5] - 1;
		}
	}

	lcd.clear();
	captureDateTimeDisplays();
	char monthsBuffer[8];
	
	lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + meridiem + " " + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[4]]))) + " " + String(tmpDisplay[3]));
	lcd.setCursor(0, 1);
	lcd.print(String(tmpInts[5]) + F(" <back> <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void captureDateTime(){
	tmpInts[2] = rtc.getTime().hour;
	tmpInts[1] = rtc.getTime().min;
	tmpInts[0] = rtc.getTime().sec;
	tmpInts[3] = rtc.getTime().date;
	tmpInts[4] = rtc.getTime().mon;
	tmpInts[5] = rtc.getTime().year;
	captureDateTimeDisplays();
}

void captureDateTimeDisplays(){
	int hourConversion = (tmpInts[2] == 0) ? 12 : tmpInts[2];

	//Thanks Romans...
	int hoursKey[12][2] = {
		{ 13, 1 }, { 14, 2 }, { 15, 3 }, { 16, 4 }, { 17, 5 }, { 18, 6 }, { 19, 7 }, { 20, 8 }, { 21, 9 }, { 22, 10 }, { 23, 11 }, { 24, 12 }
	};
	if (hourConversion > 12){
		for (int i = 0; i < 12; i++){
			if (hourConversion == hoursKey[i][0]){
				hourConversion = hoursKey[i][1];
			}
		}
	}

	meridiem = (tmpInts[2] >= 12 && tmpInts[2] < 24) ? "PM" : "AM";
	tmpDisplay[0] = (tmpInts[3] == 1 || tmpInts[3] == 21 || tmpInts[3] == 31) ? "st" : (tmpInts[3] == 2 || tmpInts[3] == 22) ? "nd" : (tmpInts[3] == 3 || tmpInts[3] == 23) ? "rd" : "th";
	tmpDisplay[1] = (hourConversion < 10) ? "0" + String(hourConversion) : String(hourConversion);
	tmpDisplay[2] = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);
	tmpDisplay[3] = (tmpInts[3] < 10) ? "0" + String(tmpInts[3]) + tmpDisplay[0] : String(tmpInts[3]) + tmpDisplay[0];

}

void setChannelSize(int dir){
	if (cursorY == 0){
		if (cursorX == 2){
			tmpInts[0] = tmpInts[0] + dir;
			lcd.clear();
			tmpInts[0] = (tmpInts[0] < 0) ? 0 : (tmpInts[0] > 500) ? 500 : tmpInts[0];
			String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
			lcd.print(channelSize + F(" (ml) volume"));
			lcd.setCursor(0, 1);
			lcd.print(F("<back>      <ok>"));
			lcd.setCursor(cursorX, cursorY);

		}
	}
}

void setCalibrationSize(int dir){
	if (cursorX == 10){
		lcd.clear();
		String displayRots = "00";
		String displaySize = "00";
		tmpInts[1] = tmpInts[1] + dir;
		if (tmpInts[1] < 0) { tmpInts[1] = 0; }
		if (tmpInts[1] >= 10 && tmpInts[1] < 100) { displayRots = "0"; }
		if (tmpInts[1] >= 100) { displayRots = ""; }
		displayRots = displayRots + String(tmpInts[1]);

		if (tmpInts[0] < 0) { tmpInts[0] = 0; }
		if (tmpInts[0] >= 10 && tmpInts[0] < 100) { displaySize = "0"; }
		if (tmpInts[0] >= 100) { displaySize = ""; }
		displaySize = displaySize + String(tmpInts[0]);

		lcd.home();
		lcd.print(displaySize + F("(ml) ") + displayRots + F(" rots"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionAmount(int dir){
	if (cursorX == 2){
		lcd.clear();
		String prefix = "00";
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 10){
			tmpInts[0] == 10;
		}
		if (tmpInts[0] >= 10 && tmpInts[0] < 100){
			prefix = "0";
		}
		if (tmpInts[0] >= 100){
			prefix = "";
		}
		String displayAmount = prefix + String(tmpInts[0]);
		lcd.print(displayAmount + F("(ml) volume"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionDelay(int dir){
	if (cursorX == 2){
		lcd.clear();
		String prefix = "00";
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 10){
			tmpInts[0] == 10;
		}
		if (tmpInts[0] >= 10 && tmpInts[0] < 100){
			prefix = "0";
		}
		if (tmpInts[0] >= 100){
			prefix = "";
		}
		String displayDelay = prefix + String(tmpInts[0]);
		lcd.print(displayDelay + F("(sec)  delay"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionRepeat(int dir) {
	lcd.clear();
	lcd.home();

	if (cursorX == 13 && cursorY == 0){
		tmpInts[1] = tmpInts[1] + dir;
		if (tmpInts[1] < 0 || tmpInts[1] > 4){
			tmpInts[1] = 0;
		}
	}
	if (cursorX == 2 && cursorY == 1){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 0 || tmpInts[0] >= 1000){
			tmpInts[0] = 0;
		}
	}

	String displayRepeat = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);
	String displayRepeatBy = displayRepeats[tmpInts[1]];

	lcd.print(F("Repeated: "));
	lcd.print(displayRepeatBy);
	lcd.setCursor(0, 1);
	lcd.print(displayRepeat + F("x <back> <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void setPHRange(double dir){
	float minMaxDiff = 0.01;
	if ((dir == 1) ? cursorX == 3 : cursorX == 9){
		(dir == 1) ? tmpFloats[0] = tmpFloats[0] + minMaxDiff : tmpFloats[1] = tmpFloats[1] - minMaxDiff;
		if ((dir == 1) ? tmpFloats[0] > (tmpFloats[1] - minMaxDiff) : tmpFloats[1] < (tmpFloats[0] + minMaxDiff)){
			(dir == 1) ? tmpFloats[1] = tmpFloats[1] + minMaxDiff : tmpFloats[0] = tmpFloats[0] - minMaxDiff;
		}
	}
	if ((dir == 1) ? cursorX == 9 : cursorX == 3) {
		(dir == 1) ? tmpFloats[1] = tmpFloats[1] + minMaxDiff : tmpFloats[0] = tmpFloats[0] - minMaxDiff;
	}

	lcd.clear();
	lcd.print(String(tmpFloats[0]));
	lcd.write(byte(1));
	lcd.print(F(" "));
	lcd.print(String(tmpFloats[1]));
	lcd.write(byte(0));
	lcd.print(F(" PH"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}

void setPHChannels(int dir){
	if (cursorX == 6 && cursorY == 0){
		tmpInts[0] = (tmpInts[0] + dir < 1) ? 1 : (tmpInts[0] + dir > 10) ? 10 : tmpInts[0] + dir;
	}
	if (cursorX == 14 && cursorY == 0){
		tmpInts[1] = (tmpInts[1] + dir < 1) ? 1 : (tmpInts[1] + dir > 10) ? 10 : tmpInts[1] + dir;
	}
	String Up = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	String Down = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);

	lcd.clear();
	lcd.home();
	lcd.print(F("PH"));
	lcd.write(byte(1));
	lcd.print(F("CH"));
	lcd.print(Up);
	lcd.print(F(" PH"));
	lcd.write(byte(0));
	lcd.print(F("CH"));
	lcd.print(Down);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void setChannelNumber(int dir){
	String totalDisplay;
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] > 10){ tmpInts[0] = 10; }
		if (tmpInts[0] < 2){ tmpInts[0] = 2; }
	}
	lcd.clear();
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	lcd.print(totalDisplay + F(" CHANNELS"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}

void makeChannel(String path, int channelId, int numberOfSessions, JsonObject& channelData, JsonObject& sessionData){
	String channelName = path;
	SD.mkdir(channelName);
	SD.mkdir(channelName + "/ChConf");
	SD.mkdir(channelName + "/ChConf/ChDoses");
	SD.mkdir(channelName + "/ChConf/ChSize");
	SD.mkdir(channelName + "/ChConf/ChCalib");

	makeNewFile(channelName + "/channel.dro", channelData);

	for (int j = 0; j < numberOfSessions; j++){
		makeSession(channelName + "/Sessions/ChSes" + String(j + 1), sessionData);
		Serial.flush();
	}
	Serial.flush();
}

void removeChannel(String path) {
	StaticJsonBuffer<256> buffer;
	File channelFile = SD.open(path + "/channel.dro");
	JsonObject& data = buffer.parseObject(channelFile.readString());
	int sessionTotal = data["sessionsTotal"];
	SD.rmdir(path + "/ChConf/ChCalib");
	SD.rmdir(path + "/ChConf/ChDoses");
	SD.rmdir(path + "/ChConf/ChSize");
	SD.rmdir(path + "/ChConf");
	SD.remove(path + "/Channel.dro");
	for (int i = 0; i < sessionTotal; i++){
		int number = i + 1;
		SD.rmdir(path + "/Sessions/ChSes" + number + "/Amt");
		SD.rmdir(path + "/Sessions/ChSes" + number + "/Dly");
		SD.rmdir(path + "/Sessions/ChSes" + number + "/Rpt");
		SD.rmdir(path + "/Sessions/ChSes" + number + "/Str");
		SD.remove(path + "/Sessions/ChSes" + number + "/Session.dro");
		SD.rmdir(path + "/Sessions/ChSes" + number);
	}
	SD.rmdir(path + "/Sessions");
	SD.rmdir(path);
}

void trimChannels(int currentSize, int trimAmount){
	lcd.clear();
	lcd.print(F("TRIMMING CHANNEL"));
	lcd.setCursor(0, 1);
	lcd.print(F(" PLEASE HOLD... "));

	JsonObject& data = getCropData();
	JsonArray& arry = data["sessionIds"].asArray();
	int length = (arry.size() - 1);
	String path = cropName + "/CHANNELS/SYSCH";

	for (int i = 0; i < currentSize; i++){
		if (i > trimAmount){
			removeChannel(path + String(i));
			arry[length].asArray()[0] = -1;
			arry[length].asArray()[1] = -1;
			length = length - 1;
		}
		Serial.flush();
	}
	setCropData(data);
}

void addChannels(int currentSize, int addAmount){
	lcd.clear();
	lcd.print(F(" ADDING CHANNEL "));
	lcd.setCursor(0, 1);
	lcd.print(F(" PLEASE HOLD... "));
	int loopAmount = (addAmount - currentSize) + 1;

	JsonObject& data = getCropData();
	for (int j = 0; j < loopAmount; j++){
		int index = (currentSize - 1) + j;
		data["sessionIds"].asArray()[index].asArray()[0] = 0;
		data["sessionIds"].asArray()[index].asArray()[1] = 2;
	}
	setCropData(data);

	//Build Channels Settings File
	StaticJsonBuffer<64> channelObjBuffer;
	JsonObject& channelData = channelObjBuffer.createObject();
	channelData["id"] = currentChannelIndex;
	channelData["size"] = 80;
	channelData["sessionsTotal"] = 3;
	channelData["calibration"] = 0;

	//Build Session's settings file
	StaticJsonBuffer<165> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	sessionData.createNestedArray("date");
	sessionData.createNestedArray("time");
	sessionData["channel"] = currentChannelIndex;
	sessionData["amount"] = 80;
	sessionData["date"].asArray().add(rtc.getTime().date);
	sessionData["date"].asArray().add(rtc.getTime().mon + 1);
	sessionData["date"].asArray().add(rtc.getTime().year);
	sessionData["time"].asArray().add(0);
	sessionData["time"].asArray().add(0);
	sessionData["time"].asArray().add(0);
	sessionData["delay"] = sessionData["repeat"] = sessionData["repeatBy"] = 0;

	for (int i = 0; i < loopAmount; i++){
		int index = currentSize + i;
		sessionData["channel"] = index;
		makeChannel(cropName + "/CHANNELS/SYSCH" + String(index), i, 4, channelData, sessionData);
		Serial.flush();
	}
}

void setSessionNumber(int dir){
	String totalDisplay;
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	lcd.clear();
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	lcd.print(totalDisplay + F(" # OF SESSIONS"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}

void makeSession(String path, JsonObject& data){
	SD.mkdir(path);
	SD.mkdir(path + "/Amt");
	SD.mkdir(path + "/Str");
	SD.mkdir(path + "/Dly");
	SD.mkdir(path + "/Rpt");

	char buffer[512];
	tmpFile = SD.open(path + "/session.dro", FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();

	Serial.flush();
}

void trimSessions(int currentSize, int trimAmount){
	lcd.clear();
	lcd.home();
	lcd.print(F(" TRIM SESSIONS "));
	lcd.setCursor(0, 1);
	lcd.print(F(" PLEASE HOLD... "));
	String path = cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/SESSIONS/CHSES";
	for (int i = 0; i <= currentSize; i++){
		if (i > trimAmount){
			SD.rmdir(path + String(i) + "/AMT");
			SD.rmdir(path + String(i) + "/DLY");
			SD.rmdir(path + String(i) + "/RPT");
			SD.rmdir(path + String(i) + "/STR");
			SD.remove(path + String(i) + "/SESSION.DRO");
			SD.rmdir(path + String(i));
		}
		Serial.flush();
	}
}

void addSessions(int currentSize, int addAmount){
	lcd.clear();
	lcd.home();
	lcd.print(F("ADDING SESSIONS"));
	lcd.setCursor(0, 1);
	lcd.print(F(" PLEASE HOLD... "));

	//Build Session's settings file
	StaticJsonBuffer<165> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	sessionData.createNestedArray("date");
	sessionData.createNestedArray("time");
	sessionData["channel"] = currentChannelIndex;
	sessionData["amount"] = 80;
	sessionData["date"].asArray().add(rtc.getTime().date);
	sessionData["date"].asArray().add(rtc.getTime().mon + 1);
	sessionData["date"].asArray().add(rtc.getTime().year);
	sessionData["time"].asArray().add(0);
	sessionData["time"].asArray().add(0);
	sessionData["time"].asArray().add(0);
	sessionData["delay"] = sessionData["repeat"] = sessionData["repeatBy"] = 0;

	//0 none, 1 = hourly, 2 = daily, 3 = weekly, 4 = monthly, 5 = yearly
	for (int i = 0; i <= addAmount; i++){
		if (i > currentSize){
			makeSession(cropName + "/channels/sysch" + currentChannelIndex + "/sessions/chses" + String(i), sessionData);
		}
		Serial.flush();
	}
}

void buildCrop(){
	String channelName;
	File channelSettingsFile;
	File sessionSettingsFile;
	int defaultChannelSize = 10;
	int defaultSessionSize = 3;
	int i;

	//Parse core file object
	JsonObject& core = getCoreData();
	core["crop"] = cropName;
	core["crops"].asArray().add(cropName);
	setCoreData(core);
	String path = cropName + "/Channels/SysCh";

	lcd.clear();
	lcd.home();
	lcd.print(F("Building Crop..."));
	SD.mkdir(cropName + "/SysConf");
	SD.mkdir(cropName + "/SysConf/DateTime");
	SD.mkdir(cropName + "/SysConf/ChNum");
	SD.mkdir(cropName + "/SysConf/PPM");
	SD.mkdir(cropName + "/SysConf/PH");
	SD.mkdir(cropName + "/SysConf/Open");
	SD.mkdir(cropName + "/SysConf/NewCrop");
	SD.mkdir(cropName + "/SysConf/Delete");
	SD.mkdir(cropName + "/Channels");

	//Build Crop Settings File
	StaticJsonBuffer<415> cropObjBuffer;
	JsonObject& cropSettings = cropObjBuffer.createObject();
	JsonArray& sessionIds = cropSettings.createNestedArray("sessionIds");
	JsonArray& ppmRange = cropSettings.createNestedArray("ppm");
	JsonArray& phRange = cropSettings.createNestedArray("ph");
	JsonArray& phChannels = cropSettings.createNestedArray("phChannels");
	ppmRange.add(1200);
	ppmRange.add(1600);
	phRange.add(5.6);
	phRange.add(6.2);
	phChannels.add(8);
	phChannels.add(9);
	cropSettings["totalChannels"] = 10;
	//Build Channels and their sub sessions
	lcd.setCursor(0, 1);

	//Build Channels Settings File
	StaticJsonBuffer<64> channelObjBuffer;
	JsonObject& channelData = channelObjBuffer.createObject();
	channelData["size"] = 80;
	channelData["sessionsTotal"] = 3;
	channelData["id"] = channelData["calibration"] = 0;

	//Build Session's settings file
	StaticJsonBuffer<165> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	sessionData.createNestedArray("date");
	sessionData.createNestedArray("time");
	sessionData["amount"] = 80;
	sessionData["date"].asArray().add(rtc.getTime().date);
	sessionData["date"].asArray().add(rtc.getTime().mon + 1);
	sessionData["date"].asArray().add(rtc.getTime().year);
	sessionData["time"].asArray().add(0);
	sessionData["time"].asArray().add(0);
	sessionData["time"].asArray().add(0);
	sessionData["channel"] = sessionData["delay"] = sessionData["repeat"] = sessionData["repeatBy"] = 0;

	for (i = 0; i < defaultChannelSize; i++){
		int index = i + 1;
		JsonArray& sessionIdRecord = sessionIds.createNestedArray();
		sessionIdRecord.add(1); //session id
		sessionIdRecord.add(defaultSessionSize);// out of
		channelData["id"] = sessionData["channel"] = index;
		makeChannel(path + String(index), index, defaultSessionSize, channelData, sessionData);
		lcd.print(F("*"));
		Serial.flush();
	}
	//makeNewFile must happen after for loop cause we are gathering session ids
	makeNewFile(cropName + "/crop.dro", cropSettings);
	screenName = "";
	lcd.noBlink();
	lcd.clear();
	File root = SD.open("/" + cropName);
	getDirectoryMenus(root);
	root.close();
	loadCrop();
}

void loadCrop(){
	lcd.home();

	JsonObject& cropData = getCropData();
	JsonArray& sessionIds = cropData["sessionIds"].asArray();
	int numberOfChannels = sessionIds.size();
	int id;
	for (int i = 0; i < numberOfChannels; i++){
		id = sessionIds[i].asArray()[0];
		tmpFile = SD.open(cropName + "/channels/sysch" + String(i + 1) + "/sessions/chses" + String(id + 1) + "/session.dro");
		DynamicJsonBuffer buffer;
		JsonObject& sessionData = buffer.parseObject(tmpFile.readString());
		tmpFile.close();

		sessionValues[i][0] = id;
		sessionValues[i][1] = sessionIds[i].asArray()[1];
		sessionValues[i][2] = sessionData["channel"];
		sessionValues[i][3] = sessionData["amount"];
		sessionValues[i][4] = sessionData["time"][0];
		sessionValues[i][5] = sessionData["time"][1];
		sessionValues[i][6] = sessionData["time"][2];
		sessionValues[i][7] = sessionData["date"][0];
		sessionValues[i][8] = sessionData["date"][1];
		sessionValues[i][9] = sessionData["date"][2];
		sessionValues[i][10] = sessionData["delay"];
		sessionValues[i][11] = sessionData["repeat"];
		sessionValues[i][12] = sessionData["repeatBy"];


		lcd.clear();
		lcd.home();
		lcd.print(F(" LOADING  CROP "));
		lcd.setCursor(0, 1);
		//lcd.clear();
		//lcd.print(freeMemory());
		//delay(1000);
		for (int k = 0; k < i; k++){
			lcd.setCursor(k, 1);
			lcd.print(F("*"));
		}
		if (i == numberOfChannels){
			openHomeScreen();
		}
		Serial.flush();
	}
}

void turing(){
	int sessionSize = sizeof(&sessionValues) / sizeof(&sessionValues);
	for (int i = 0; i < sessionSize; i++){
		//do stuff
		//int repeatBy = sessionValues[i][12];
		//int repeat = sessionValues[i][11];
		//int delay = sessionValues[i][10];
		//int amount = sessionValues[i][3];
		//int channelId = sessionValues[i][2];
		//int outof = sessionValues[i][1];
		//int sessionId = sessionValues[i][0];

		if (sessionValues[i][9] == rtc.getTime().year){
			if (sessionValues[i][8] == rtc.getTime().mon){
				if (sessionValues[i][7] == rtc.getTime().date){
					if (sessionValues[i][4] == rtc.getTime().hour){
						if (sessionValues[i][5] == rtc.getTime().min){
							lcd.clear();
							lcd.print(F("Session"));
							lcd.print(String(sessionValues[i][0]));
							lcd.print(F(" Dose"));
							if (sessionValues[i][0] < sessionValues[i][1]){
								setNextSession(sessionValues[i][0] + 1, sessionValues[i][2]);
							}
						}
					}
				}
			}
		}
	}
	delay(250);
}

void setNextSession(int sessionId, int channelId){
	tmpFile = SD.open(cropName + "/channels/sysch" + channelId + "/sessions/chses" + sessionId + "/session.dro");
	DynamicJsonBuffer buffer;
	JsonObject& sessionData = buffer.parseObject(tmpFile.readString());
	tmpFile.close();

	sessionValues[channelId][0] = sessionData["id"];
	sessionValues[channelId][1] = sessionData["channel"];
	sessionValues[channelId][2] = sessionData["amount"];
	sessionValues[channelId][3] = sessionData["time"][0];
	sessionValues[channelId][4] = sessionData["time"][1];
	sessionValues[channelId][5] = sessionData["time"][2];
	sessionValues[channelId][6] = sessionData["date"][0];
	sessionValues[channelId][7] = sessionData["date"][1];
	sessionValues[channelId][8] = sessionData["date"][2];
	sessionValues[channelId][9] = sessionData["delay"];
	sessionValues[channelId][10] = sessionData["repeat"];
	sessionValues[channelId][11] = sessionData["repeatBy"];

	JsonObject& data = getCropData();
	data["sessionIds"].asArray()[channelId] = sessionId;
	setCropData(data);
}
