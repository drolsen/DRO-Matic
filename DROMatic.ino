/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 30, 2016
*  devin@devinrolsen.com
*/
#include <LiquidCrystal.h> //lib for interfacing with LCD screen
#include <SPI.h> //Suppoting lib for SD card
#include <SD.h> //SD card API
#include <StandardCplusplus.h> //STD
#include <StandardCplusplus\vector> //Vectors
#include <StandardCplusplus\ctime> //Time helper
#include <ArduinoJson\ArduinoJson.h> //Arduno Json (aka epic)
#include <DS3231.h> //Real time clock lib
#include <avr/pgmspace.h> //native to arduino no lib
#include <Stepper.h> //Stepper motoer lib
#include <math.h>

const int stepsPerRevolution = 100;
const int stepperSpeed = 800;
Stepper myStepper(stepsPerRevolution, 15, 14);
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

DS3231  rtc(SDA, SCL);
using namespace std;

//Global vars
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
unsigned long previousMillis;  //stores last time
unsigned long currentMillis;
int Key, menuIndex, currentMinute;
int minPPM = 1200;
int maxPPM = 1600;
int tmpInts[6];

String cropName;
String nameArry[15];
String screenName;
float tmpFloats[2];
String tmpDisplay[5]; //suffix, hour, min, day
byte cursorX;
byte cursorY;
byte days[12] = { 31, ((tmpInts[5] % 4 == 0 && tmpInts[5] % 100 != 0) || (tmpInts[5] % 400 == 0)) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
byte currentChannelIndex;
byte currentSessionIndex;
byte currentAlphaIndex = 0;
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
vector<vector<vector<byte>>> matrix;
File tmpFile;

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


//Get and Sets
JsonObject& getCoreData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/core.dro");
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setCoreData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/core.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

JsonObject& getCropData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/crop.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setCropData(JsonObject& d){
	char b[512];
	tmpFile = SD.open("dromatic/" + cropName + "/crop.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	openHomeScreen();
}

JsonObject& getChannelData(JsonBuffer& b, int channelIndex = currentChannelIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setChannelData(JsonObject& d, int channelIndex = currentChannelIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		openHomeScreen();
	}
}

JsonObject& getSessionData(JsonBuffer& b, int channelIndex = currentChannelIndex, int sessionIndex = currentSessionIndex){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/sessions/chses" + sessionIndex + "/session.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setSessionData(JsonObject& d, int channelIndex = currentChannelIndex, int sessionIndex = currentSessionIndex, bool returnHome = true){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/sessions/chses" + sessionIndex + "/session.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		openHomeScreen();
	}
}

void setup()
{
	lcd.createChar(0, upArrow);
	lcd.createChar(1, downArrow);
	lcd.begin(16, 2);
	Serial.begin(9600);
	rtc.begin();
	captureDateTime();
	String relayName;
	myStepper.setSpeed(stepperSpeed);
	previousMillis = 0;

	pinMode(MS1MS2, OUTPUT);
	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);
	pinMode(RELAY4, OUTPUT);
	pinMode(RELAY5, OUTPUT);
	pinMode(RELAY6, OUTPUT);
	pinMode(RELAY7, OUTPUT);
	pinMode(RELAY8, OUTPUT);
	pinMode(RELAY9, OUTPUT);
	pinMode(RELAY10, OUTPUT);

	digitalWrite(MS1MS2, LOW);
	digitalWrite(RELAY1, HIGH);
	digitalWrite(RELAY2, HIGH);
	digitalWrite(RELAY3, HIGH);
	digitalWrite(RELAY4, HIGH);
	digitalWrite(RELAY5, HIGH);
	digitalWrite(RELAY6, HIGH);
	digitalWrite(RELAY7, HIGH);
	digitalWrite(RELAY8, HIGH);
	digitalWrite(RELAY9, HIGH);
	digitalWrite(RELAY10, HIGH);

	if (!SD.begin(53)){
		lcd.print(F("SD Card Required"));
		lcd.setCursor(0, 1);
		lcd.print(F("Insert And Rest"));
		screenName = "REQUIREDSD";
	} 
	else 
	{
		coreInit();
	}
}

void loop()
{
	Key = analogRead(0);
	currentMillis = millis();
	if (Key >= 0 && Key <= 650){
		previousMillis = currentMillis;
	}

	//30 seconds
	if ((currentMillis - previousMillis) >= 30000) {
		if (screenName == "") {
			previousMillis = currentMillis;
			openHomeScreen();
		}
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
		if (screenName == "OPEN"){
			matrix = {
				{ { 0, 0 } },
				{ { 1, 1 }, { 9, 9 } }
			};
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
		if (screenName == "AMT" || screenName == "DLY"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHSIZE"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 9, 9 }, { 12, 12 } }
			};
		}
		if (screenName == "CHCALIB"){
			matrix = {
				{ { 10, 10 } },
				{ { 1, 1 }, { 9, 9 }, { 12, 12 } }
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
			tmpFile = SD.open("dromatic/" + cropName + "/" + getMenuHistory());
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
		if (screenName == "OPEN"){
			if (cursorX == 0 && cursorY == 0){
				lcd.clear();
				scrollMenus(dir);
				lcd.setCursor(0, 1);
				lcd.print(F("<back>  <open>"));
				lcd.home();
			}
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
			tmpFile = SD.open("dromatic/" + cropName + "/" + history + "/" + menus[menuIndex]);
			menusHistory.push_back(menus[menuIndex]);
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			if (menus.size() > 0){
				menuIndex = 0;
				tmpFile.close();
				printDisplayNames(menus.front());
				printScrollArrows();
			}
			else {
				screenName = menusHistory.back();
				lcd.blink();
				lcd.home();
				if (screenName == "DATETIME"){
					captureDateTime();
					char monthsBuffer[8];

					lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[1]]))) + F(" ") + tmpDisplay[1]);
					lcd.setCursor(0, 1);
					lcd.print(String(tmpInts[0]) + F(" <back> <ok>"));
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "NEWCROP"){
					menus.clear();
					menusHistory.clear();
					currentAlphaIndex = 0;
					currentChannelIndex = 0;
					currentSessionIndex = 0;
					menuIndex = 0;
					cursorX = cursorY = 0;
					startNewCrop();
				}
				if (screenName == "OPEN"){
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <open>"));
					lcd.home();
					cursorX = cursorY = 0;
					tmpFile = SD.open("dromatic/");
					getDirectoryMenus(tmpFile);
					menuIndex = 0;
					lcd.print(menus[menuIndex]);
					printScrollArrows();
					lcd.home();
				}
				if (screenName == "PPM"){
					lcd.print(String(minPPM) + F("-") + String(maxPPM) + F(" EC/PPM"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "PH"){
					DynamicJsonBuffer jsonBuffer;
					JsonObject& data = getCropData(jsonBuffer);
					tmpFloats[0] = data["ph"].asArray()[0];
					tmpFloats[1] = data["ph"].asArray()[1];
					lcd.print(String(tmpFloats[0]));
					lcd.write(byte(1));
					lcd.print(String(F(" ")) + String(tmpFloats[1]));
					lcd.write(byte(0));
					lcd.print(F(" PH"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>    <next>"));
					lcd.setCursor(3, 0);
					cursorX = 3;
					cursorY = 0;
				}
				if (screenName == "CHNUM"){
					cursorX = 1;
					cursorY = 0;
					DynamicJsonBuffer jsonBuffer;
					JsonObject& data = getCropData(jsonBuffer);
					int total = data["totalChannels"];
					tmpInts[0] = total;
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" CHANNELS"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CHDOSES"){
					cursorX = 1;
					cursorY = 0;
					DynamicJsonBuffer channelBuffer;
					JsonObject& data = getChannelData(channelBuffer);
					tmpInts[0] = data["sessionsTotal"];
					String totalDisplay;
					totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(totalDisplay + F(" # OF SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CHSIZE"){
					DynamicJsonBuffer channelBuffer;
					JsonObject& data = getChannelData(channelBuffer);
					tmpInts[0] = data["size"];
					String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
					lcd.print(channelSize + F(" (ml) volume"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <ok|all>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "CHCALIB"){
					DynamicJsonBuffer channelBuffer;
					JsonObject& data = getChannelData(channelBuffer);
					tmpInts[0] = data["size"];
					tmpInts[1] = data["calibration"];
					String targetSize = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);
					String rotsSize = (tmpInts[1] >= 10 && tmpInts[1] <= 99) ? "0" + String(tmpInts[1]) : (tmpInts[1] < 10 && tmpInts[1] >= 0) ? "00" + String(tmpInts[1]) : String(tmpInts[1]);
					lcd.print(targetSize + F("(ml) ") + rotsSize + F(" rots"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>  <ok|all>"));
					cursorX = 10;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "AMT"){
					DynamicJsonBuffer sessionBuffer;
					JsonObject& data = getSessionData(sessionBuffer);
					tmpInts[0] = data["amount"];
					String displayAmount = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);

					lcd.print(displayAmount + F("(ml) volume"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "STR"){
					captureSessionDateTime();
					char monthsBuffer[8];
					lcd.clear();
					lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[1]]))) + F(" ") + tmpDisplay[1]);
					lcd.setCursor(0, 1);
					lcd.print(String(tmpInts[0]) + F(" <back> <ok>"));
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "DLY"){
					DynamicJsonBuffer sessionBuffer;
					JsonObject& data = getSessionData(sessionBuffer);
					tmpInts[0] = data["delay"];
					String displayDelay = (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) : String(tmpInts[0]);

					lcd.print(displayDelay + F("(sec)  delay"));
					lcd.setCursor(0, 1);
					lcd.print(F("<back>      <ok>"));
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
				if (screenName == "RPT"){
					char repeatsBuffer[8];
					DynamicJsonBuffer sessionBuffer;
					JsonObject& data = getSessionData(sessionBuffer);
					tmpInts[0] = data["repeat"];
					tmpInts[1] = data["repeatBy"];

					String displayRepeat = (tmpInts[0] == -1)? "Inf." : (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) + "x" : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) + "x" : String(tmpInts[0]) + "x";
					String displayRepeatBy = strcpy_P(repeatsBuffer, (char*)pgm_read_word(&(displayRepeats[tmpInts[1]])));

					lcd.print("Repeats: ");
					lcd.print(displayRepeatBy);
					lcd.setCursor(0, 1);
					lcd.print(displayRepeat + F(" <back> <ok>"));
					cursorX = 13;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
				}
			}
			delay(350);
		}
		//Saves
		if (screenName == "NewCrop"){
			if (cursorX == 11 && cursorY == 1){
				String nameConfirm;
				for (int i = 0; i < 15; i++){
					nameConfirm = nameConfirm + nameArry[i];
				}
				if (nameConfirm == ""){
					lcd.clear();
					lcd.home();
					lcd.print(F("Sorry, No Blank"));
					lcd.setCursor(0, 1);
					lcd.print(F("Crop Names"));
					delay(3000);
					lcd.clear();
					lcd.setCursor(0, 1);
					lcd.print(F("Crop Name <done>"));
					cursorX = cursorY = 0;
					lcd.home();
				}
				else if (SD.exists("dromatic/" + nameConfirm)){
					lcd.clear();
					lcd.home();
					lcd.print(F("Sorry, Crop"));
					lcd.setCursor(0, 1);
					lcd.print(F("Already Exists"));
					delay(3000);
					lcd.clear();
					lcd.print(nameConfirm);
					lcd.setCursor(0, 1);
					lcd.print(F("Crop Name <done>"));
					cursorX = cursorY = 0;
					lcd.home();
				}
				else {
					buildCrop();
				}
			}
		}
		if (screenName == "OPEN"){
			if (cursorX == 9 && cursorY == 1){
				if (menus[menuIndex] != cropName){
					changeCrop();
				}
				else{
					exitScreen();
				}
			}
			if (cursorX == 1 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "PPM"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
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
				DynamicJsonBuffer jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
				data["ph"].asArray()[0].set(tmpFloats[0]);
				data["ph"].asArray()[1].set(tmpFloats[1]);

				setCropData(data);
				tmpFloats[0] = tmpFloats[1] = 0.0;
				screenName = "PHCHL";
				delay(250);

				lcd.clear();
				lcd.home();

				tmpInts[0] = data["phChannels"].asArray()[0];
				tmpInts[1] = data["phChannels"].asArray()[1];

				String UpDisplay = (tmpInts[0] < 10) ? String(F("0")) + String(tmpInts[0]) : String(tmpInts[0]);
				String DownDisplay = (tmpInts[1] < 10) ? String(F("0")) + String(tmpInts[1]) : String(tmpInts[1]);

				lcd.print(F("PH"));
				lcd.write(byte(1));
				lcd.print(F("CH"));
				lcd.print(UpDisplay);
				lcd.print(F(" PH"));
				lcd.write(byte(0));
				lcd.print(String(F("CH")) + DownDisplay);
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
				DynamicJsonBuffer jsonBuffer;
				JsonObject& data = getCropData(jsonBuffer);
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
				//hour, min, seconds
				rtc.setTime(tmpInts[4], tmpInts[5], 0);
				//day, month, year
				rtc.setDate(tmpInts[2], tmpInts[1], tmpInts[0]);
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
				lcd.clear();
				int i, j, currentTotal, newTotal, dir;
				Time currentTime = rtc.getTime();
				DynamicJsonBuffer jsonBuffer;
				JsonObject& cropData = getCropData(jsonBuffer);

				currentTotal = cropData["totalChannels"]; //capture orignal total
				newTotal = tmpInts[0];
				dir = (currentTotal < newTotal) ? 1 : -1;

				cropData["totalChannels"] = newTotal; //then set new total
				setCropData(cropData);
				if (dir < 0){
					lcd.print(F(" TRIM CHANNELS "));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					trimChannels(currentTotal + 1, newTotal);
				}

				if (dir > 0){
					lcd.print(F("ADDING CHANNELS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					addChannels(currentTotal, newTotal + 1);
				}
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHDOSES"){
			if (cursorX == 13 && cursorY == 1){
				//Get channel data
				DynamicJsonBuffer channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);
				lcd.clear();
				lcd.home();

				if (channelData["sessionsTotal"] > tmpInts[0]){ //we are trimming sessions
					lcd.print(F("TRIM SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					trimSessions(channelData["sessionsTotal"], tmpInts[0]);
				}
				else if (channelData["sessionsTotal"] < tmpInts[0]){ //we are adding sessions
					lcd.print(F("ADDING SESSIONS"));
					lcd.setCursor(0, 1);
					lcd.print(F(" PLEASE HOLD... "));
					addSessions(channelData["sessionsTotal"], tmpInts[0]);
				}
				channelData["sessionsTotal"] = tmpInts[0]; //update channel's session total
				setChannelData(channelData);
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHSIZE"){
			if (cursorX == 9 && cursorY == 1){ //single channel save
				DynamicJsonBuffer channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);

				channelData["size"] = tmpInts[0];
				setChannelData(channelData);
			}
			if (cursorX == 12 && cursorY == 1){ //all channel save
				lcd.clear();
				lcd.home();
				lcd.print("  SAVING ALL  ");
				lcd.setCursor(0, 1);
				lcd.print("  PLEASE WAIT ");

				DynamicJsonBuffer cropBuffer;
				JsonObject& cropData = getCropData(cropBuffer);
				int channelSize = cropData["totalChannels"];
				while (channelSize--){
					DynamicJsonBuffer channelBuffer;
					JsonObject& channelData = getChannelData(channelBuffer, channelSize);
					channelData["size"] = tmpInts[0];
					setChannelData(channelData, channelSize, false);
				}
			}
			if (cursorX == 1 || cursorX == 9 || cursorX == 12 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHCALIB"){
			if (cursorX == 9 && cursorY == 1){ //single channel save
				DynamicJsonBuffer channelBuffer;
				JsonObject& channelData = getChannelData(channelBuffer);

				channelData["calibration"] = tmpInts[1];
				setChannelData(channelData);
				tmpInts[1] = 0;
				tmpInts[0] = 0;
				exitScreen();
			}
			if (cursorX == 12 && cursorY == 1){ //all channel save
				lcd.clear();
				lcd.home();
				lcd.print("  SAVING ALL  ");
				lcd.setCursor(0, 1);
				lcd.print("  PLEASE WAIT ");
				
				DynamicJsonBuffer cropBuffer;
				JsonObject& cropData = getCropData(cropBuffer);
				int channelSize = cropData["totalChannels"];
				while (channelSize--){
					DynamicJsonBuffer channelBuffer;
					JsonObject& channelData = getChannelData(channelBuffer, channelSize);
					channelData["calibration"] = tmpInts[1];
					setChannelData(channelData, channelSize, false);
				}
			}
			if (cursorX == 1 || cursorX == 9 || cursorX == 12 && cursorY == 1){
				tmpInts[1] = 0;
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "AMT"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);
				sessionData["amount"] = tmpInts[0];
				setSessionData(sessionData);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "STR"){
			if (cursorX == 13 && cursorY == 1){
				lcd.clear();
				lcd.noBlink();
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);
				JsonArray& date = sessionData["date"];
				JsonArray& time = sessionData["time"];

				date[0] = tmpInts[0]; //year
				date[1] = tmpInts[1]; //month
				date[2] = tmpInts[2]; //day
				date[3] = tmpInts[3]; //day of week
				time[0] = tmpInts[4]; //hour
				time[1] = tmpInts[5]; //min

				//Leap year help
				tm time_in = { 
					0,			// second
					tmpInts[5], // minute
					tmpInts[4], // hour
					tmpInts[2], // 1-based day
					tmpInts[1], // 0-based month
					tmpInts[0] - tmpInts[0] //year since 1900
				};
				time_t time_temp = mktime(&time_in);
				tm const *time_out = localtime(&time_temp);
				sessionData["date"].asArray()[3] = time_out->tm_wday;
				setSessionData(sessionData); //save session data
				tmpDisplay[0] = tmpDisplay[1] = tmpDisplay[2] = tmpDisplay[3] = ""; //Clear out tmpDisplays
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				exitScreen();
			}
		}
		if (screenName == "DLY"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);

				sessionData["delay"] = tmpInts[0];
				setSessionData(sessionData);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "RPT"){
			if (cursorX == 13 && cursorY == 1){
				DynamicJsonBuffer sessionBuffer;
				JsonObject& sessionData = getSessionData(sessionBuffer);

				sessionData["repeatBy"] = tmpInts[1];
				sessionData["repeat"] = tmpInts[0];
				sessionData["repeated"] = tmpInts[0]; //we use this as a changeable variable so we can use repeat value for crop resets

				setSessionData(sessionData);
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				tmpInts[1] = 0;
				tmpInts[0] = 0;
				exitScreen();
			}
		}
	}
}

void changeCrop(){
	lcd.clear();
	lcd.noBlink();
	lcd.home();
	lcd.print(F(" LOADING  CROP  "));
	DynamicJsonBuffer b;
	tmpFile = SD.open("dromatic/core.dro");
	JsonObject& core = b.parseObject(tmpFile.readString());
	core["crop"] = menus[menuIndex];
	tmpFile.close();
	setCoreData(core);
	menuIndex = currentChannelIndex = currentSessionIndex = currentAlphaIndex = 0;
	menus.clear();
	menusHistory.clear();
	coreInit();
}

void openHomeScreen	(){
	captureDateTime();
	lcd.clear();
	char monthsBuffer[8];
	//hour					//minute		  //AM/PM											//Month														//Day
	lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[rtc.getTime().mon]))) + F(" ") + tmpDisplay[1]);
	lcd.setCursor(0, 1);
	lcd.print(F("PPM:"));
	lcd.print(String((analogRead(9) / 100) * 1000));
	lcd.print(F(" PH:"));
	lcd.print(String(analogRead(9)* 14.00 / 1024, 2));
	lcd.home();
	lcd.noBlink();
	turing(); //the heart of it all, thank you Allen
}

void screenMatrix(){
	byte row0Size = matrix[0].size();
	byte row1Size = matrix[1].size();

	byte frontFrontFront = matrix.front().front().front();
	byte frontBackBack = matrix.front().back().back();

	byte backBackBack = matrix.back().back().back();
	byte backFrontFront = matrix.back().front().front();
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
	if (SD.exists("dromatic")){ //has OS already been setup?
		DynamicJsonBuffer coreBuffer;
		JsonObject& coreData = getCoreData(coreBuffer);

		cropName = coreData["crop"].asString();
		if (cropName != "" && SD.exists("dromatic/" + cropName)){ //Loading up exisiting core file's crop directory
			screenName = "";
			openHomeScreen();
			File cropFile = SD.open("dromatic/" + cropName);
			getDirectoryMenus(cropFile);
			cropFile.close();
		}
		else{ //we have core file with crop, but no crop directory. //VERY CORNER CASE!
			startNewCrop();
		}
	}
	else { //if OS has not been setup, lets build out core OS file / directory
		SD.mkdir("dromatic");
		lcd.print(F("New Drive Found!"));
		lcd.setCursor(0, 1);
		lcd.print(F(" Please Hold... "));
		tmpFile = SD.open("dromatic/core.dro", FILE_WRITE);
		char buffer[64];
		DynamicJsonBuffer coreBuffer;
		JsonObject& settings = coreBuffer.createObject();
		settings["crop"] = "";
		settings.printTo(buffer, sizeof(buffer));
		tmpFile.print(buffer);
		tmpFile.close();

		lcd.clear();
		lcd.home();
		lcd.print(F("New Crop Setup"));
		lcd.setCursor(0, 1);
		lcd.print(F(" Please Hold... "));
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
	tmpFile = SD.open("dromatic/" + cropName + "/" + getMenuHistory());
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

void getDirectoryMenus(File directory) {
	menus.clear();
	while (true) {
		tmpFile = directory.openNextFile();
		if (!tmpFile) { // no more files
			break;
		}
		if (tmpFile.isDirectory()){
			menus.push_back(tmpFile.name());
		}
		tmpFile.close();
	}
}

int getCropCount(){
	File crops = SD.open("dromatic/");
	int count = 0;
	while (true) {
		tmpFile = crops.openNextFile();
		if (!tmpFile) { break; }
		if (tmpFile.isDirectory()){
			count = count + 1;
		}
		tmpFile.close();
	}
	return count;
}

String getMenuHistory(){
	byte i, size;
	String join;
	size = menusHistory.size();
	for (i = 0; i < size; i++){
		join = join + "/" + menusHistory.at(i);
	}
	return join;
}

void printDisplayNames(String menu){
	lcd.home();
	bool hasMatch = false;
	const byte isChannel = strstr(menu.c_str(), "SYSCH") != NULL;
	const byte isSession = strstr(menu.c_str(), "CHSES") != NULL;
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
	else if (isSession) {
		lcd.print(F("CHANNEL"));
		lcd.setCursor(0, 1);
		lcd.print(F("SESSION "));
		lcd.print(index);
		currentSessionIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}
	else {
		byte i;
		for (i = 0; i < 18; i++){
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
	byte i;
	cropName = "";
	if (dir != NULL){
		scrollAlpha(dir);
		char alphaBuffer[2];
		nameArry[cursorX] = strcpy_P(alphaBuffer, (char*)pgm_read_word(&(alphabet[currentAlphaIndex])));
	}
	//Compile/Collapse
	for (i = 0; i < 15; i++){
		cropName = cropName + nameArry[i];
	}
	lcd.setCursor(cursorX, cursorY);
}

void makeNewFile(String path, JsonObject& data){
	char buffer[1024];
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
	lcd.print(String(minPPM) + F("-") + String(maxPPM) + F(" EC/PPM"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}

void setDateTime(int dir){
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

	lcd.clear();
	captureDateTimeDisplays();
	char monthsBuffer[8];

	lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[tmpInts[1]]))) + F(" ") + tmpDisplay[1]);
	lcd.setCursor(0, 1);
	lcd.print(String(tmpInts[0]) + F(" <back> <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void captureDateTime(){
	Time current = rtc.getTime();
	tmpInts[0] = current.year;
	tmpInts[1] = current.mon;
	tmpInts[2] = current.date;
	tmpInts[3] = current.dow;
	tmpInts[4] = current.hour;
	tmpInts[5] = current.min;
	captureDateTimeDisplays();
}

void captureSessionDateTime(){
	DynamicJsonBuffer sessionBuffer;
	JsonObject& data = getSessionData(sessionBuffer);
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

void captureDateTimeDisplays(){
	byte i, maxDaysInMonth, hourConversion;

	maxDaysInMonth = days[tmpInts[1]];
	hourConversion = (tmpInts[4] == 0) ? 12 : tmpInts[4];

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
	tmpDisplay[0] = (tmpInts[2] == 1 || tmpInts[2] == 21 || tmpInts[2] == 31) ? "st" : (tmpInts[2] == 2 || tmpInts[2] == 22) ? "nd" : (tmpInts[2] == 3 || tmpInts[2] == 23) ? "rd" : "th";
	//day
	tmpDisplay[1] = (tmpInts[2] > maxDaysInMonth) ? "01" + String(tmpDisplay[0]) : (tmpInts[2] < 10) ? "0" + String(tmpInts[2]) + String(tmpDisplay[0]) : String(tmpInts[2]) + String(tmpDisplay[0]);
	//hour
	tmpDisplay[2] = (hourConversion < 10) ? "0" + String(hourConversion) : String(hourConversion);
	//minute
	tmpDisplay[3] = (tmpInts[5] < 10) ? "0" + String(tmpInts[5]) : String(tmpInts[5]);
	//AM/PM
	tmpDisplay[4] = (tmpInts[4] >= 12 && tmpInts[4] < 24) ? "PM" : "AM";
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

void setChannelSize(int dir){
	if (cursorY == 0){
		if (cursorX == 2){
			tmpInts[0] = tmpInts[0] + dir;
			lcd.clear();
			tmpInts[0] = (tmpInts[0] < -1) ? -1 : (tmpInts[0] > 500) ? 500 : tmpInts[0]; //we must cap channel max size to 500ml
			String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? (tmpInts[0] == -1) ? "Infinte" : "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
			lcd.print(channelSize + F(" (ml) volume"));
			lcd.setCursor(0, 1);
			lcd.print(F("<back>  <ok|all>"));
			lcd.setCursor(cursorX, cursorY);
		}
	}
}

void setCalibrationSize(int dir){
	if (cursorX == 10){
		lcd.clear();
		String displayRots, displaySize;
		displayRots = displaySize = "00";

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
		lcd.print(F("<back>  <ok|all>"));
		lcd.setCursor(cursorX, cursorY);
		float timeTurning = 31.5;

		RelayToggle(currentChannelIndex, true); //turn replay gate power on
		myStepper.step((dir == -1) ? 800 : -800);
		RelayToggle(currentChannelIndex, false); //turn replay gate power off

	}
}

void setSessionAmount(int dir){
	if (cursorX == 2){
		lcd.clear();
		String prefix, displayAmount;
		prefix = "00";
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
		displayAmount = prefix + String(tmpInts[0]);
		lcd.print(displayAmount + F("(ml) volume"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionDelay(int dir){
	if (cursorX == 2){
		lcd.clear();
		String prefix, displayDelay;

		prefix = "00";
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
		displayDelay = prefix + String(tmpInts[0]);
		lcd.print(displayDelay + F("(sec)  delay"));
		lcd.setCursor(0, 1);
		lcd.print(F("<back>      <ok>"));
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionRepeat(int dir) {
	char repeatsBuffer[8];
	String displayRepeat, displayRepeatBy;
	lcd.clear();
	lcd.home();

	if (cursorX == 13 && cursorY == 0){
		tmpInts[1] = tmpInts[1] + dir;
		if (tmpInts[1] < 0 || tmpInts[1] > 5){
			tmpInts[1] = 0;
		}
	}
	if (cursorX == 2 && cursorY == 1){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < -1 || tmpInts[0] >= 1000){
			tmpInts[0] = -1;
		}
	}

	displayRepeat = (tmpInts[0] == -1)? "Inf." : (tmpInts[0] >= 10 && tmpInts[0] <= 99) ? "0" + String(tmpInts[0]) + "x" : (tmpInts[0] < 10 && tmpInts[0] >= 0) ? "00" + String(tmpInts[0]) + "x" : String(tmpInts[0]) + "x";
	displayRepeatBy = strcpy_P(repeatsBuffer, (char*)pgm_read_word(&(displayRepeats[tmpInts[1]])));

	lcd.print(F("Repeats: "));
	lcd.print(displayRepeatBy);
	lcd.setCursor(0, 1);
	lcd.print(displayRepeat + F(" <back> <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void setPHRange(double dir){
	String min, max;
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
	min = String(tmpFloats[0]);
	max = String(tmpFloats[1]);

	lcd.clear();
	lcd.print(min);
	lcd.write(byte(1));
	lcd.print(F(" "));
	lcd.print(max);
	lcd.write(byte(0));
	lcd.print(F(" PH"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>    <next>"));
	lcd.setCursor(cursorX, 0);
}

void setPHChannels(int dir){
	String up, down;
	if (cursorX == 6 && cursorY == 0){
		tmpInts[0] = (tmpInts[0] + dir < 1) ? 1 : (tmpInts[0] + dir > 10) ? 10 : tmpInts[0] + dir;
	}
	if (cursorX == 14 && cursorY == 0){
		tmpInts[1] = (tmpInts[1] + dir < 1) ? 1 : (tmpInts[1] + dir > 10) ? 10 : tmpInts[1] + dir;
	}
	up = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	down = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);

	lcd.clear();
	lcd.home();
	lcd.print(F("PH"));
	lcd.write(byte(1));
	lcd.print(F("CH"));
	lcd.print(up);
	lcd.print(F(" PH"));
	lcd.write(byte(0));
	lcd.print(F("CH"));
	lcd.print(down);
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, cursorY);
}

void setChannelNumber(int dir){
	String totalDisplay;
	byte min, max;
	min = 2; max = 10;
	if (cursorX == 1 && cursorY == 0){
		//Min / max checking
		tmpInts[0] = (tmpInts[0] + dir > max) ? max : (tmpInts[0] + dir < min) ? min : tmpInts[0] + dir;
	}
	lcd.clear();
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	lcd.print(totalDisplay + F(" CHANNELS"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(1, 0);
}

void makeChannel(String path, int numberOfSessions, JsonObject& channelData, JsonObject& sessionData){
	byte j;
	String channelName = path;
	SD.mkdir(channelName);
	SD.mkdir(channelName + "/ChConf");
	SD.mkdir(channelName + "/ChConf/ChDoses");
	SD.mkdir(channelName + "/ChConf/ChSize");
	SD.mkdir(channelName + "/ChConf/ChCalib");
	makeNewFile(channelName + "/channel.dro", channelData);

	for (j = 0; j < numberOfSessions; j++){
		sessionData["id"] = j;
		sessionData["outof"] = numberOfSessions;
		makeSession(channelName + "/Sessions/ChSes" + String(j + 1), sessionData);
		Serial.flush();
	}
	Serial.flush();
}

void removeChannel(String path) {
	byte i, sessionTotal;
	String loopedPath;
	StaticJsonBuffer<256> buffer;

	tmpFile = SD.open(path + "/channel.dro");
	JsonObject& data = buffer.parseObject(tmpFile.readString());
	tmpFile.close();

	sessionTotal = data["sessionsTotal"];
	SD.rmdir(path + "/ChConf/ChCalib");
	SD.rmdir(path + "/ChConf/ChDoses");
	SD.rmdir(path + "/ChConf/ChSize");
	SD.rmdir(path + "/ChConf");
	SD.remove(path + "/Channel.dro");

	for (i = 0; i < (sessionTotal + 1); i++){
		loopedPath = path + "/Sessions/ChSes" + String(i + 1);
		SD.rmdir(loopedPath + "/Amt");
		SD.rmdir(loopedPath + "/Dly");
		SD.rmdir(loopedPath + "/Rpt");
		SD.rmdir(loopedPath + "/Str");
		SD.remove(loopedPath + "/Session.dro");
		SD.rmdir(loopedPath);
	}

	SD.rmdir(path + "/Sessions");
	SD.rmdir(path);
}

void trimChannels(int currentSize, int trimAmount){
	byte length, i;
	String path = "dromatic/" + cropName + "/channels/sysch";

	for (i = 0; i < currentSize; i++){
		if (i > trimAmount){
			removeChannel(path + i);
		}
		Serial.flush();
	}
}

void addChannels(int currentSize, int addAmount){
	byte j, index, loopAmount;
	loopAmount = addAmount - currentSize;

	//Build Channels Settings File
	StaticJsonBuffer<64> channelObjBuffer;
	JsonObject& channelData = channelObjBuffer.createObject();
	channelData["id"] = currentChannelIndex;
	channelData["size"] = 80;
	channelData["sessionsTotal"] = 3;
	channelData["calibration"] = 0;

	//Build Session's settings file
	StaticJsonBuffer<185> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	sessionData.createNestedArray("date");
	sessionData.createNestedArray("time");
	sessionData["channel"] = currentChannelIndex;
	sessionData["amount"] = 80;
	sessionData["date"].asArray().add(rtc.getTime().year);
	sessionData["date"].asArray().add(rtc.getTime().mon + 1);
	sessionData["date"].asArray().add(rtc.getTime().date);
	sessionData["date"].asArray().add(rtc.getTime().dow);
	sessionData["time"].asArray().add(rtc.getTime().hour);
	sessionData["time"].asArray().add(rtc.getTime().min);
	sessionData["delay"] = sessionData["repeatBy"] = sessionData["repeat"] = 0;

	for (j = 1; j < loopAmount; j++){
		index = currentSize + j;
		sessionData["channel"] = index;
		makeChannel("dromatic/" + cropName + "/channels/sysch" + String(index), 2, channelData, sessionData);
		Serial.flush();
	}
}

void setSessionNumber(int dir){
	String totalDisplay;
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);

	lcd.clear();
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
	byte i;
	String path = "dromatic/" + cropName + "/CHANNELS/SYSCH" + String(currentChannelIndex) + "/SESSIONS/CHSES";
	for (i = 0; i <= currentSize; i++){
		if (i > trimAmount){
			SD.rmdir(path + i + "/AMT");
			SD.rmdir(path + i + "/DLY");
			SD.rmdir(path + i + "/RPT");
			SD.rmdir(path + i + "/STR");
			SD.remove(path + i + "/SESSION.DRO");
			SD.rmdir(path + i);
		}
		Serial.flush();
	}
}

void addSessions(int currentSize, int addAmount){
	byte i;
	//Build Session's settings file
	StaticJsonBuffer<165> buffer; //DO NOT LOWER THAN 512 (buildCrop() can get away with this at 128, but not addSessions() )
	JsonObject& sessionData = buffer.createObject();
	JsonArray& date = sessionData.createNestedArray("date");
	JsonArray& time = sessionData.createNestedArray("time");
	sessionData["channel"] = currentChannelIndex;
	sessionData["amount"] = 80;
	int currentYear = rtc.getTime().year;
	int nextYear = rtc.getTime().year + 1;
	int currentMonth = rtc.getTime().mon;
	int nextMonth = (rtc.getTime().mon + 1 > 11) ? 0 : rtc.getTime().mon + 1;

	date.add((currentMonth > nextMonth) ? nextYear : currentYear);		//year
	date.add(nextMonth);												//month
	date.add(rtc.getTime().date);		//day
	date.add(rtc.getTime().dow);		//day of week

	time.add(0); //hour
	time.add(0); //min

	sessionData["delay"] = sessionData["repeat"] = sessionData["repeatBy"] = sessionData["repeated"] = 0;

	//repeatBy developer key
	//0 none, 1 = hourly, 2 = daily, 3 = weekly, 4 = monthly, 5 = yearly
	for (i = 0; i <= addAmount; i++){
		if (i > currentSize){
			makeSession("dromatic/" + cropName + "/channels/sysch" + String(currentChannelIndex) + "/sessions/chses" + String(i), sessionData);
		}
		Serial.flush();
	}
}

void buildCrop(){
	String channelName;
	File channelSettingsFile;
	File sessionSettingsFile;
	byte i, defaultChannelSize, defaultSessionSize;
	defaultChannelSize = 10;
	defaultSessionSize = 3;

	//Parse core file object
	DynamicJsonBuffer coreBuffer;
	JsonObject& core = getCoreData(coreBuffer);
	core["crop"] = cropName;
	setCoreData(core);

	String path = "dromatic/" + cropName + "/channels/sysch";

	lcd.clear();
	lcd.home();
	lcd.print(F("Building Crop..."));
	SD.mkdir("dromatic/" + cropName + "/SysConf");
	SD.mkdir("dromatic/" + cropName + "/SysConf/DateTime");
	SD.mkdir("dromatic/" + cropName + "/SysConf/ChNum");
	SD.mkdir("dromatic/" + cropName + "/SysConf/PPM");
	SD.mkdir("dromatic/" + cropName + "/SysConf/PH");
	SD.mkdir("dromatic/" + cropName + "/SysConf/Open");
	SD.mkdir("dromatic/" + cropName + "/SysConf/NewCrop");
	SD.mkdir("dromatic/" + cropName + "/SysConf/Delete");
	SD.mkdir("dromatic/" + cropName + "/Channels");

	///////////////////
	//Build Crop file//
	///////////////////
	StaticJsonBuffer<512> cropBuffer;
	JsonObject& crop = cropBuffer.createObject();
	JsonArray& ecRange = crop.createNestedArray("ec");
	JsonArray& phRange = crop.createNestedArray("ph");
	JsonArray& phChannels = crop.createNestedArray("phChannels");

	//Defualt EC Range
	ecRange.add(1200);
	ecRange.add(1600);

	//Default PH Range
	phRange.add(5.6);
	phRange.add(6.2);

	//Default PH channels
	phChannels.add(1);
	phChannels.add(2);

	crop["totalChannels"] = 10;
	lcd.setCursor(0, 1);

	///////////////////////
	//Build Channels file//
	///////////////////////
	StaticJsonBuffer<64> channelBuffer;
	JsonObject& channel = channelBuffer.createObject();
	channel["size"] = 80;
	channel["sessionsTotal"] = defaultSessionSize;
	channel["id"] = channel["calibration"] = 0;

	////////////////////////
	//Build Session's file//
	////////////////////////
	StaticJsonBuffer<200> sessionBuffer;
	JsonObject& session = sessionBuffer.createObject();
	JsonArray& sessionDate = session.createNestedArray("date");
	JsonArray& sessionTime = session.createNestedArray("time");
	session["expired"] = false;
	session["amount"] = 80;
	captureDateTime();

	int currentYear = tmpInts[0];
	int nextYear = currentYear + 1;
	int currentMonth = tmpInts[1];
	int nextMonth = ((currentMonth + 1) > 11) ? 0 : currentMonth + 1;

	sessionDate.add((currentMonth > nextMonth)? nextYear : currentYear);		//year
	sessionDate.add(nextMonth);													//month
	sessionDate.add(tmpInts[2]);		//day
	sessionDate.add(tmpInts[3]);		//day of week
	sessionTime.add(tmpInts[4]);		//hour
	sessionTime.add(tmpInts[5]);		//min

	session["channel"] = session["delay"] = session["repeat"] = session["repeatBy"] = 0;

	for (i = 0; i < defaultChannelSize; i++){
		channel["id"] = session["channel"] = i;
		makeChannel(path + (i + 1), defaultSessionSize, channel, session);
		lcd.print(F("*"));
		Serial.flush();
	}
	//makeNewFile must happen after for loop cause we are gathering session ids
	makeNewFile("dromatic/" + cropName + "/crop.dro", crop);
	screenName = "";
	lcd.noBlink();
	lcd.clear();
	File root = SD.open("dromatic/" + cropName);
	getDirectoryMenus(root);
	root.close();
	openHomeScreen();
}

void RelayToggle(int channel, bool gate) {
	if (gate == true){
		switch (channel){
		case 1:
			digitalWrite(RELAY1, LOW);
			break;
		case 2:
			digitalWrite(RELAY2, LOW);
			break;
		case 3:
			digitalWrite(RELAY3, LOW);
			break;
		case 4:
			digitalWrite(RELAY4, LOW);
			break;
		case 5:
			digitalWrite(RELAY5, LOW);
			break;
		case 6:
			digitalWrite(RELAY6, LOW);
			break;
		case 7:
			digitalWrite(RELAY7, LOW);
			break;
		case 8:
			digitalWrite(RELAY8, LOW);
			break;
		case 9:
			digitalWrite(RELAY9, LOW);
			break;
		case 10:
			digitalWrite(RELAY10, LOW);
			break;
		}
	}
	if (gate == false){
		switch (channel){
		case 1:
			digitalWrite(RELAY1, HIGH);
			break;
		case 2:
			digitalWrite(RELAY2, HIGH);
			break;
		case 3:
			digitalWrite(RELAY3, HIGH);
			break;
		case 4:
			digitalWrite(RELAY4, HIGH);
			break;
		case 5:
			digitalWrite(RELAY5, HIGH);
			break;
		case 6:
			digitalWrite(RELAY6, HIGH);
			break;
		case 7:
			digitalWrite(RELAY7, HIGH);
			break;
		case 8:
			digitalWrite(RELAY8, HIGH);
			break;
		case 9:
			digitalWrite(RELAY9, HIGH);
			break;
		case 10:
			digitalWrite(RELAY10, HIGH);
			break;
		}
	}
}

void turing(){
	bool debug = false;
	captureDateTime();
	bool valid;
	int i, j, 
		setDOY,		currentDOY,
		setYear,	currentYear,
		setMonth,	currentMonth,
		setDay,		currentDay, 
		setDOW,		currentDOW,
		setHour,	currentHour, 
		setMin,		currentMin,
		repeatCount, repeatedCount, repeatType,
		setAmount, setCalibration, setSize, setDelay,
		id, outof, calib, direction;
	
	//We start by looping over channesl
	for (i = 1; i < 10; i++){
		if (analogRead(0) >= 0 && analogRead(0) <= 650){ 
			break; 
		}
		StaticJsonBuffer<256> channelBuffer;
		JsonObject& channel = getChannelData(channelBuffer, i);
		int channelSessionTotal = channel["sessionsTotal"];
		for (j = 1; j < channelSessionTotal+1; j++){
			if (analogRead(0) >= 0 && analogRead(0) <= 650){ 
				break; 
			}
			valid = true; //set a validation flag to true
			StaticJsonBuffer<512> sessionBuffer;
			JsonObject& session = getSessionData(sessionBuffer, i, j);
			JsonArray& sessionDate = session["date"];
			JsonArray& sessionTime = session["time"];

			if (session["expried"] != false) continue; //lets skip this session if it has already expired.

			//Capture session's set data
			setYear = sessionDate[0];
			setMonth = sessionDate[1];
			setDay = sessionDate[2];
			setDOW = sessionDate[3];
			setHour = sessionTime[0];
			setMin = sessionTime[1];
			setAmount = session["amount"];
			setCalibration = channel["calibration"];
			setSize = channel["size"];
			repeatCount = session["repeat"];
			repeatedCount = session["repeated"];
			repeatType = session["repeatBy"];
			setDelay = session["delay"];
			id = session["id"];
			outof = session["outof"];

			//Capture current date/time data
			currentYear = tmpInts[0];
			currentMonth = tmpInts[1];
			currentDay = tmpInts[2];
			currentDOW = tmpInts[3];
			currentHour = tmpInts[4];
			currentMin = tmpInts[5];
			lcd.home();

			//Validation of set data vs current data
			//Year
			if (setYear != currentYear){	//year
				if (repeatType != 5){		//should it repeat yearly?
					valid = false;
					if (debug == true){
						lcd.clear();
						lcd.print("Year not valid");
						lcd.setCursor(0, 1);
						lcd.print("Ch");
						lcd.print(i);
						lcd.print(" ");
						lcd.print(setYear);
						lcd.home();
						delay(1000);
					}
				}
			}

			//Month
			if (setMonth != currentMonth){		//month
				if (repeatType != 4){			//should it repeat monthly OR weekly?
					valid = false;
					if (debug == true){
						lcd.clear();
						lcd.print("Month not valid");
						lcd.setCursor(0, 1);
						lcd.print("Ch");
						lcd.print(i);
						lcd.print(" ");
						lcd.print(setMonth);
						lcd.home();
						delay(1000);
					}
				}
			}

			//Day
			if (setDay != currentDay){						//day
				if (repeatType != 3 || repeatType != 2){	//should it repeat weekly OR daily?
					valid = false;
					if (debug == true){
						lcd.clear();
						lcd.print("Day not valid");
						lcd.setCursor(0, 1);
						lcd.print("Ch");
						lcd.print(i);
						lcd.print(" ");
						lcd.print(setDay);
						lcd.home();
						delay(1000);
					}
				}
			}

			//Hour
			if (setHour != currentHour){	//hour
				if (repeatType != 1){		//should it repeat hourly?
					valid = false;
					if (debug == true){
						lcd.clear();
						lcd.print("Hour not valid");
						lcd.setCursor(0, 1);
						lcd.print("Ch");
						lcd.print(i);
						lcd.print(" ");
						lcd.print(setHour);
						lcd.home();
						delay(1000);
					}
				}
			}

			//Min
			if (setMin != currentMin){	//min (in no right setup would you ever need a dose to repeat, nor is the physicaly possible every minute)
				valid = false;
				if (debug == true){
					lcd.clear();
					lcd.print("Min not valid");
					lcd.setCursor(0, 1);
					lcd.print("Ch");
					lcd.print(i);
					lcd.print(" ");
					lcd.print(setMin);
					lcd.home();
					delay(1000);
				}
			}

			//LET THE DOSING BEGIN!!
			if (valid){
				if (repeatType == 0){ //repeat type is = to none (most basic type of session)
					pumpSpin(setAmount, setCalibration, setSize, i); //do pump spin
					session["expried"] = true; //set session to expired
				} else {
					if (repeatCount == -1){ //repeat count is = to infinite
						pumpSpin(setAmount, setCalibration, setSize, i); //do pump spin
					}else{
						if ((repeatedCount - 1) > 0) {
							repeatedCount = repeatedCount - 1;
						}else{
							repeatedCount = 0;
							session["expired"] = true;
						}
						pumpSpin(setAmount, setCalibration, setSize, i); //do pump spin
					}
				}
			}
		}
	}
}

void pumpSpin(int setAmount, int setCalibration, int channelSize, int channelNumber){

	RelayToggle(channelNumber, true); //turn channel gate power on

	if (channelSize > 0){ //fixed channel system (aka syrings or not)?
		vector<long int> pumpSessions;
		long int totalRotations = stepsPerRevolution * setCalibration; //7400
		long double intPart, fractionPart, pumpingRounds;
		pumpingRounds = (float)setAmount / (float)channelSize; // 2.35 (235ml)
		fractionPart = pumpingRounds - (long)pumpingRounds; // 0.35
		intPart = pumpingRounds - fractionPart; // 2.0
		fractionPart = 1 / fractionPart; //2.857142857142857

		for (int i = 0; i < intPart; i++){
			pumpSessions.push_back(totalRotations); //7400
		}
		pumpSessions.push_back(floor(totalRotations / fractionPart)); //2590

		for (int j = 0; j < pumpSessions.size(); j++){
			int turnCounts = (pumpSessions[j] / channelSize);
			int reverseCount = turnCounts;
			while (turnCounts--){
				myStepper.step(-800); //pull fluids
				lcd.clear();
				lcd.print(turnCounts);
				if (turnCounts == 0){
					while (reverseCount--){
						myStepper.step(800); //push fluids

					}
				}
			}
		}
	} else { //no fixed channel size but a fixed configuration size of 100ml (aka flow control system or pump)
		myStepper.step(stepsPerRevolution * (setAmount / setCalibration));
	}

	RelayToggle(channelNumber, false); //turn channel gate power on
}