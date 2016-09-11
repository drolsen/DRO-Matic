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
DS3231  rtc(SDA, SCL);
using namespace std;


//Global vars
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int Key;
const int chipSelect = 4;
unsigned long previousMillis = 0;  //stores last time

String cropName;
String renameArry[15] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "" };
String screenName;
int cursorX = 0;
int cursorY = 0;
int menuIndex = 0;
int minPPM = 1200;
int maxPPM = 1400;


int hour;
int minute;
int sec;
int day;
int month;
int year;
int days[12] = { 31, ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
char* meridiem;

int sessionHour;
int sessionMinute;
int sessionSec;
int sessionDay;
int sessionMonth;
int sessionYear;
char* sessionMeridiem;

int currentChannelIndex;
int currentSessionIndex;

int tmpInts[2];
float tmpFloats[2];
String tmpDisplay[4]; //suffix, hour, min, day
int currentAlphaIndex = 0;

const char* alphabet[37] = { 
	" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", 
	"1", "2", "3", "4", "5", "6", "7", "8", "9"
};
const char* displayNames[18][3] = {
	{ "SYSCONF", "SYSTEM", "SETTINGS" },
	{ "CHANNELS", "SYSTEM", "CHANNELS" },
	{ "DATETIME", "DATE & TIME", "CONFIGURATION" },
	{ "CHCONF", "CHANNEL", "SETTINGS" },
	{ "CHNUM", "NUMBER OF", "CHANNELS" },
	{ "PPM", "EC/PPM RANGE", "CONFIGURATION" },
	{ "PH", "PH RANGE", "CONFIGURATION" },
	{ "OPEN", "LOAD CROP" },
	{ "NEW", "NEW CROP" },
	{ "DELETE", "DELETE CROP" },
	{ "CHDOSES", "NUMBER OF", "SESSIONS" },
	{ "CHSIZE", "SIZE(ml) OF", "CHANNEL" },
	{ "CHCALIB", "CHANNEL", "CALIBRATION" },
	{ "SESSIONS", "CHANNEL", "SESSIONS" },
	{ "AMT", "SESSION", "AMOUNT(ml)" },
	{ "STR", "SESSION START", "DATE & TIME" },
	{ "DLY", "SESSION DELAY", "CONFIGURATION" },
	{ "RPT", "SESSION REPEAT", "CONFIGURATION" }
};
const char* months[12] = { "Jan", "Feb", "Mar", "Apl", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* displayRepeats[5] = { "Hrly", "Daly", "Wkly", "Mtly", "Yrly" };
const char* daysOfWeek[7] = { "Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat" };

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

//Data passing functions for core and crop
JsonObject& getCoreData(){
	StaticJsonBuffer<512> buffer;
	File core = SD.open("core.dro");
	return buffer.parseObject(core.readString());
}

void setCoreData(JsonObject& data){
	char buffer[128];
	File core = SD.open("core.dro", O_WRITE | O_TRUNC);
	data.printTo(buffer, sizeof(buffer));
	core.print(buffer);
	core.close();
}

JsonObject& getCropData(){
	StaticJsonBuffer<256> buffer;
	File crop = SD.open(cropName + "/crop.dro");
	return buffer.parseObject(crop.readString());
}

JsonObject& parseCropData(File cropFile){
	StaticJsonBuffer<512> buffer;
	return buffer.parseObject(cropFile.readString());
}

void setCropData(JsonObject& data){
	char buffer[512];
	File crop = SD.open(cropName + "/crop.dro", O_WRITE | O_TRUNC);
	data.printTo(buffer, sizeof(buffer));
	crop.print(buffer);
	crop.close();
}

JsonObject& getChannelData(){
	StaticJsonBuffer<512> buffer;
	File channel = SD.open(cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/CHANNEL.DRO", FILE_READ);
	return buffer.parseObject(channel.readString());
}

void setChannelData(JsonObject& data){
	char buffer[128];
	File channel = SD.open(cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/CHANNEL.DRO", O_WRITE | O_TRUNC);
	data.printTo(buffer, sizeof(buffer));
	channel.print(buffer);
	channel.close();
}

	JsonObject& getSessionData(){
		StaticJsonBuffer<512> buffer;
		File session = SD.open(cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/SESSIONS/CHSES" + currentSessionIndex + "/SESSION.DRO", FILE_READ);
		return buffer.parseObject(session.readString());
	}

		void setSessionData(JsonObject& data){
			char sessionBuffer[1024];
			File session = SD.open(cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/SESSIONS/CHSES" + currentSessionIndex + "/SESSION.DRO", O_WRITE | O_TRUNC);
			data.printTo(sessionBuffer, sizeof(sessionBuffer));
			session.print(sessionBuffer);
			session.close();
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
		lcd.print("SD Card Required");
		lcd.setCursor(0,1);
		lcd.print("Insert And Rest");
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
	}

	if (Key == 0){
		//Right
		if (screenName == "NewCrop"){
			matrix = {
				{ { 0, 15 } },
				{ { 11, 11 } }
			};
			renameCrop(NULL);
			lcd.blink();
		}
		if (screenName == "DATETIME"){
			matrix = {
				{ { 1, 1 }, { 4, 4 }, { 10, 10 }, {13, 13} },
				{ { 3, 3 }, { 6, 6 }, {13, 13} }
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
		if (screenName == "CHNUM"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHSIZE"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHDOSES"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHCALIB"){
			matrix = {
				{ { 10, 10 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "AMT"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "STR"){
			matrix = {
				{ { 1, 1 }, { 4, 4 }, { 10, 10 }, { 13, 13 } },
				{ { 3, 3 }, { 6, 6 }, { 13, 13 } }
			};
		}
		if (screenName == "DLY"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "RPT"){
			matrix = {
				{ {13, 13} },
				{ { 2, 2 }, { 6, 6 }, { 13, 13 } }
			};
		}
		cursorX = cursorX + 1;
		screenMatrix();
		delay(250);
	}
	if (Key == 99) {
		//Up
		if (screenName == "NewCrop"){
			renameCrop(1);
		}
		if (screenName == "PPM"){
			setPPMRangeValues(1);
		}
		if (screenName == "PH"){
			setPHRange(1);
		}
		if (screenName == "PHCHL"){
			setPHChannels(1);
		}
		if (screenName == "DATETIME"){
			setDateTime(1);
		}
		if (screenName == "CHNUM"){
			setChannelNumber(1);
		}
		if (screenName == "CHDOSES"){
			setSessionNumber(1);
		}
		if (screenName == "CHSIZE"){
			setChannelSize(10);
		}
		if (screenName == "CHCALIB"){
			setCalibrationSize(1);
		}
		if (screenName == "AMT"){
			setSessionAmount(10);
		}
		if (screenName == "STR"){
			setSessionDateTime(1);
		}
		if (screenName == "DLY"){
			setSessionDelay(1);
		}
		if (screenName == "RPT"){
			setSessionRepeat(1);
		}
		if (screenName == ""){
			scrollMenus(1);
		}
		delay(250);
	}
	if (Key == 255){
		//Down
		if (screenName == "NewCrop"){
			renameCrop(-1);
		}
		if (screenName == "PPM"){
			setPPMRangeValues(-1);
		}
		if (screenName == "PH"){
			setPHRange(-1);
		}
		if (screenName == "PHCHL"){
			setPHChannels(-1);
		}
		if (screenName == "DATETIME"){
			setDateTime(-1);
		}
		if (screenName == "CHNUM"){
			setChannelNumber(-1);
		}
		if (screenName == "CHDOSES"){
			setSessionNumber(-1);
		}
		if (screenName == "CHSIZE"){
			setChannelSize(-10);
		}
		if (screenName == "CHCALIB"){
			setCalibrationSize(-1);
		}
		if (screenName == "AMT"){
			setSessionAmount(-10);
		}
		if (screenName == "STR"){
			setSessionDateTime(-1);
		}
		if (screenName == "DLY"){
			setSessionDelay(-1);
		}
		if (screenName == "RPT"){
			setSessionRepeat(-1);
		}
		if (screenName == ""){
			scrollMenus(-1);
		}
		delay(250);
	}
	if (Key == 408) {
		//Left
		if (screenName == "NewCrop"){
			matrix = {
				{ { 0, 15 },
				{ 11, 11 } }
			};
		}
		if (screenName == "DATETIME"){
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
		if (screenName == "CHNUM"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHDOSES"){
			matrix = {
				{ { 1, 1 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "CHSIZE"){
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
		if (screenName == "AMT"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "STR"){
			matrix = {
				{ { 1, 1 }, { 4, 4 }, { 10, 10 }, { 13, 13 } },
				{ { 3, 3 }, { 6, 6 }, { 13, 13 } }
			};
		}
		if (screenName == "DLY"){
			matrix = {
				{ { 2, 2 } },
				{ { 1, 1 }, { 13, 13 } }
			};
		}
		if (screenName == "RPT"){
			matrix = {
				{ { 13, 13 } },
				{ { 2, 2 }, { 6, 6 }, { 13, 13 } }
			};
		}
		if (screenName == ""){
			menusHistory.pop_back();
			menuIndex = 0;
			File prevLvl = SD.open(cropName + "/" + getMenuHistory());
			getDirectoryMenus(prevLvl);
			lcd.clear();
			prevLvl.close();
			printDisplayNames(menus.front());
			printScrollArrows();
		} else {
			cursorX = cursorX - 1;
			screenMatrix();
		}
		delay(250);
	}
	if (Key == 639) {
		//Select
		if (screenName == ""){
			File nextLvl = SD.open(cropName + "/" + getMenuHistory() + "/" + menus[menuIndex]);
			menusHistory.push_back(menus[menuIndex]);
			getDirectoryMenus(nextLvl);
			if (menus.size() > 0){
				menuIndex = 0;
				lcd.clear();
				nextLvl.close();
				printDisplayNames(menus.front());
				printScrollArrows();
			}else{
				screenName = menusHistory.back();
				if (screenName == "DATETIME"){
					lcd.blink();
					lcd.clear();
					lcd.home();
					captureDateTime();
					lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + meridiem + " " + String(months[month]) + " " + String(tmpDisplay[3]));
					lcd.setCursor(0, 1);
					lcd.print(String(year)+" <back> <ok>");
					lcd.setCursor(1, 0);
					cursorX = 1;
					cursorY = 0;
				}
				if (screenName == "PPM"){
					lcd.blink();
					lcd.clear();
					lcd.home();
					lcd.print(String(minPPM) + "-" + String(maxPPM) + " EC/PPM");
					lcd.setCursor(0, 1);
					lcd.print("<back>      <ok>");
					lcd.setCursor(3, 0);
					cursorX = 3;
				}
				if (screenName == "PH"){
					lcd.blink();
					lcd.clear();
					lcd.home();
					JsonObject& data = getCropData();
					tmpFloats[0] = data["minPH"];
					tmpFloats[1] = data["maxPH"];
					lcd.print(String(tmpFloats[0]));
					lcd.write(byte(1));
					lcd.print(+" " + String(tmpFloats[1]));
					lcd.write(byte(0));
					lcd.print(" PH");
					lcd.setCursor(0, 1);
					lcd.print("<back>    <next>");
					cursorX = 3;
					lcd.setCursor(3, 0);
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
					lcd.print(totalDisplay + " CHANNELS");
					lcd.setCursor(0, 1);
					lcd.print("<back>      <ok>");
					//delay(5000);
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
					lcd.print(totalDisplay + " # OF SESSIONS");
					lcd.setCursor(0, 1);
					lcd.print("<back>      <ok>");
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
					lcd.print(channelSize+" (ml) volume");
					lcd.setCursor(0, 1);
					lcd.print("<back>      <ok>");
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
					lcd.print(targetSize + "(ml) " + rotsSize + " rots");
					lcd.setCursor(0,1);
					lcd.print("<back>      <ok>");
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

					lcd.print(displayAmount+"(ml) volume");
					lcd.setCursor(0, 1);
					lcd.print("<back>      <ok>");
					cursorX = 2;
					cursorY = 0;
					lcd.setCursor(cursorX, cursorY);
					lcd.blink();
				}
				if (screenName == "STR"){
					lcd.clear();
					lcd.home();
					captureSessionDateTime();
					lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + sessionMeridiem + " " + String(months[sessionMonth]) + " " + String(tmpDisplay[3]));
					lcd.setCursor(0, 1);
					lcd.print(String(sessionYear) + " <back> <ok>");
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

					lcd.print(displayDelay + "(sec)  delay");
					lcd.setCursor(0, 1);
					lcd.print("<back>      <ok>");
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

					lcd.print("Repeated: "+displayRepeatBy);
					lcd.setCursor(0, 1);
					lcd.print(displayRepeat+"x <back> <ok>");
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
				data["minPPM"] = minPPM;
				data["maxPPM"] = maxPPM;
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
				tmpFloats[0] = 0.0;
				tmpFloats[1] = 0.0;
				screenName = "PHCHL";
				delay(250);

				lcd.clear();
				lcd.home();
				tmpInts[0] = data["PHUpChannel"];
				tmpInts[1] = data["PHDownChannel"];

				String UpDisplay;
				String DownDisplay;
				UpDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
				DownDisplay = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);
				lcd.print("PH");
				lcd.write(byte(1));
				lcd.print("CH" + UpDisplay + " PH");
				lcd.write(byte(0));
				lcd.print("CH" + DownDisplay);
				lcd.setCursor(0, 1);
				lcd.print("<back>      <ok>");
				cursorX = 6;
				cursorY = 0;
				lcd.setCursor(6, 0);
			}
			if (cursorX == 1 && cursorY == 1){
				tmpFloats[0] = 0.0;
				tmpFloats[1] = 0.0;
				screenName = "";
				exitScreen();
			}
		}
		if (screenName == "PHCHL"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getCropData();
				data["PHUpChannel"] = tmpInts[0];
				data["PHDownChannel"] = tmpInts[1];
				setCropData(data);
			}
			if (cursorX == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				tmpInts[1] = 0;
				exitScreen();
			}
		}
		if (screenName == "DATETIME"){
			if (cursorX == 13 && cursorY == 1){
				rtc.setTime(hour, minute, 0);
				rtc.setDate(day, month, year);
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
				if (total+1 > tmpInts[0]){	trimChannels(total+1, tmpInts[0]);	}
				if (total < tmpInts[0]){	addChannels(total+1, tmpInts[0]);	}
			}
			if (cursorX == 1 && cursorY == 1 || cursorX == 13 && cursorY == 1){
				tmpInts[0] = 0;
				exitScreen();
			}
		}
		if (screenName == "CHDOSES"){
			if (cursorX == 13 && cursorY == 1){
				JsonObject& data = getChannelData();
				if (data["sessionsTotal"] > tmpInts[0]){
					//we are adding sessions
					trimChannelSessions(data["sessionsTotal"], tmpInts[0]);
				}
				if (data["sessionsTotal"] < tmpInts[0]){
					//we are removing sessions
					addChannelSessions(data["sessionsTotal"], tmpInts[0]);
				}
				data["sessionsTotal"] = tmpInts[0];
				setChannelData(data);
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
				//learn to compile sessionTime into string that can be churned over
				JsonObject& data = getSessionData();

				StaticJsonBuffer<64> dateBuffer;
				StaticJsonBuffer<32> timeBuffer;
				JsonArray& date = dateBuffer.createArray();
				JsonArray& time = timeBuffer.createArray();
				time.add(sessionHour);
				time.add(sessionMinute);
				time.add(sessionSec);

				date.add(sessionDay);
				date.add(sessionMonth);
				date.add(sessionYear);

				tm time_in = { sessionSec, sessionMinute, sessionHour, // second, minute, hour
					sessionDay, sessionMonth, sessionYear - sessionYear }; // 1-based day, 0-based month, year since 1900
				time_t time_temp = mktime(&time_in);
				tm const *time_out = localtime(&time_temp);
				date.add(time_out->tm_wday);

				data["date"] = date;
				data["time"] = time;
				setSessionData(data);
				tmpDisplay[0] = ""; //suffix
				tmpDisplay[1] = ""; //hour
				tmpDisplay[2] = ""; //min
				tmpDisplay[3] = ""; //day
			}
			if (cursorX == 6 || cursorX == 13 && cursorY == 1){
				menusHistory.pop_back();
				menuIndex = 0;
				screenName = "";
				File prevLvl = SD.open(cropName + "/" + getMenuHistory());
				getDirectoryMenus(prevLvl);
				lcd.clear();
				lcd.noBlink();
				prevLvl.close();
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

int division(int sum, int size)
{
	return sum / size;
}

void openHomeScreen(){
	captureDateTime();
	lcd.clear();
	lcd.print(tmpDisplay[1] + ":" + tmpDisplay[2] + meridiem + " " + months[rtc.getTime().mon] + " " + tmpDisplay[3]);
	lcd.setCursor(0, 1);
	lcd.print("PPM:1275 PH:6.0");
	lcd.home();
	lcd.noBlink();
}

void screenMatrix(){
	int row0Size = matrix[0].size();
	int row1Size = matrix[1].size();

	//Check Top Row
	if (cursorY == 0){
		if (cursorX < matrix.front().front().front()){
			cursorX = matrix.back().back().back();
			cursorY = 1;
		}
		else if (cursorX > matrix.front().back().back()){
			cursorY = 1;
			cursorX = matrix.back().front().front();
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
		if (cursorX < matrix.back().front().front()){
			cursorX = matrix.front().back().back();
			cursorY = 0;
		}
		else if (cursorX > matrix.back().back().back()){
			cursorY = 0;
			cursorX = matrix.front().front().front();
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

int getChannelsTotal(){
	JsonObject& data = getCropData();
	return data["totalChannels"];
}

int getSessionTotal(){
	StaticJsonBuffer<512> buffer;
	File session = SD.open(cropName + "/Channels/Channel" + String(currentChannelIndex) + "/channel.dro");
	JsonObject& data = buffer.parseObject(session.readString());
	return data["sessionsTotal"];
}

void coreInit(){
	File coreFile = SD.open("core.dro");
	if (coreFile){ //load core settings / crop
		JsonObject& coreData = getCoreData();
		String name = coreData["crop"];
		cropName = name;

		if (SD.exists(cropName)){
			//Loading up exisiting core file's crop directory
			screenName = "";
			File cropFile = SD.open("/" + cropName);
			getDirectoryMenus(cropFile);
			JsonObject& cropData = parseCropData(cropFile);
			cropFile.close();
			openHomeScreen();
		}else{
			//we have core file with crop, but no crop directory. //VERY CORNER CASE!
			startNewCrop();
		}
	}else{ //else, setup new crop
		char buffer[512];
		StaticJsonBuffer<512> coreBuffer;
		JsonObject& settings = coreBuffer.createObject();
		coreFile = SD.open("core.dro", FILE_WRITE);
		settings["crop"];
		settings.printTo(buffer, sizeof(buffer));
		coreFile.print(buffer);
		coreFile.close();
		lcd.print("New drive Found!");
		lcd.setCursor(0, 1);
		lcd.print(" Please wait... ");
		delay(2000);
		lcd.clear();
		lcd.home();
		lcd.print("Setup new crop");
		delay(2000);
		lcd.clear();
		startNewCrop();
	}
	coreFile.close();
}

void startNewCrop(){
	screenName = "NewCrop";
	lcd.clear();
	lcd.setCursor(0, 1);
	lcd.print("Crop Name <done>");
	lcd.home();
	lcd.blink();
}

void scrollAlpha(int dir){
	currentAlphaIndex = currentAlphaIndex + dir;
	if (currentAlphaIndex > 36){
		currentAlphaIndex = 0;
	}else if (currentAlphaIndex < 0){
		currentAlphaIndex = 36;
	}
	lcd.print(alphabet[currentAlphaIndex]);
	lcd.setCursor(cursorX, cursorY);
	delay(50);
}

void scrollMenus(int dir){
	menuIndex = menuIndex + dir;
	int currentSize = menus.size() - 1;
	if (menuIndex < 0){
		menuIndex = currentSize;
	}else if (menuIndex > currentSize){
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
	File prevLvl = SD.open(cropName + "/" + getMenuHistory());
	getDirectoryMenus(prevLvl);
	lcd.clear();
	lcd.noBlink();
	prevLvl.close();
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
		File entry = dir.openNextFile();
		if (!entry) {
			// no more files
			break;
		}
		if (entry.isDirectory()){
			menus.push_back(entry.name());
		}
		entry.close();
	}
}

int getDirectoryMenusSize(File dir){
	int count = 0;
	while (true){
		File entry = dir.openNextFile();
		if (!entry){
			// no more files
			break;
		}
		if (entry.isDirectory()){
			count++;
		}
	}
	return count;
}

String getMenuHistory(){
	String concat;
	for (int i = 0; i < menusHistory.size(); i++){
		concat = concat +"/"+ menusHistory.at(i);
	}
	return concat;
}

void printDisplayNames(String menu){
	lcd.home();
	bool hasMatch = false;
	const int isChannel = strstr(menu.c_str(), "SYSCH") != NULL;
	const int isSession = strstr(menu.c_str(), "CHSES") != NULL;
	if (isChannel){
		lcd.print("SYSTEM");
		lcd.setCursor(0, 1);
		lcd.print("CHANNEL " + String(menuIndex+1));
		currentChannelIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}else if(isSession){
		lcd.print("CHANNEL");
		lcd.setCursor(0, 1);
		lcd.print("SESSION " + String(menuIndex+1));
		currentSessionIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}else{
		for (int i = 0; i < sizeof(displayNames) / sizeof(*displayNames); i++){
			if (menu == displayNames[i][0]){
				hasMatch = true;
				lcd.print(displayNames[i][1]);
				if (displayNames[i][2]) {
					lcd.setCursor(0, 1);
					lcd.print(displayNames[i][2]);
				}
			}
		}
	}
	if (hasMatch == false){
		lcd.print(menu);
	}
}

void removeDirectory(String path) {
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

void renameCrop(int dir){
	cropName = "";
	if (dir != NULL){
		scrollAlpha(dir);
		renameArry[cursorX] = alphabet[currentAlphaIndex];
	}
	//Compile/Collapse
	for (int i = 0; i < 15; i++){
		cropName = cropName + renameArry[i];
	}
	lcd.setCursor(cursorX, cursorY);
}

void makeNewFile(String path, JsonObject& data){
	char buffer[128];
	File file = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	file.print(buffer);
	file.close();
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
	lcd.print(String(minPPM) + "-" + String(maxPPM) + " EC/PPM");
	lcd.setCursor(0, 1);
	lcd.print("<back>      <ok>");
	lcd.setCursor(cursorX, 0);
}

void setDateTime(int dir){
	int maxDaysInMonth = days[month];
	if (cursorY == 0){
		if (cursorX == 1){
			(dir == 1) ? (hour = (hour + 1 > 24) ? 1 : hour + 1) : (hour = (hour - 1 < 1) ? 24 : hour - 1);
		}
		if (cursorX == 4){
			(dir == 1) ? (minute = (minute + 1 > 59) ? 0 : minute + 1) : (minute = (minute - 1 < 0) ? 59 : minute - 1);
		}
		if (cursorX == 10){
			(dir == 1) ? (month = (month + 1 > 11) ? 0 : month + 1) : (month = (month - 1 < 0) ? 11 : month - 1);
			day = 1;
		}
		if (cursorX == 13){
			(dir == 1) ? (day = (day + 1 > maxDaysInMonth) ? 1 : day + 1) : (day = (day - 1 < 1) ? maxDaysInMonth : day - 1);
		}
	}
	else{
		if (cursorX == 3){
			year = (dir == 1)? year + 1: year - 1;
		}
	}

	lcd.clear();
	captureDateTimeDisplays();

	lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + meridiem + " " + String(months[month]) + " " + String(tmpDisplay[3]));
	lcd.setCursor(0, 1);
	lcd.print(String(year)+" <back> <ok>");
	lcd.setCursor(cursorX, cursorY);
}
	
	void captureDateTime(){
		hour = rtc.getTime().hour;
		minute = rtc.getTime().min;
		sec = rtc.getTime().sec;
		day = rtc.getTime().date;
		month = rtc.getTime().mon;
		year = rtc.getTime().year;
		captureDateTimeDisplays();
	}
		
		void captureDateTimeDisplays(){
			int hourConversion = (hour == 0) ? 12 : hour;

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

			meridiem = (hour >= 12 && hour < 24) ? "PM" : "AM";
			tmpDisplay[0] = (day == 1 || day == 21 || day == 31) ? "st" : (day == 2 || day == 22) ? "nd" : (day == 3 || day == 23) ? "rd" : "th";
			tmpDisplay[1] = (hourConversion < 10) ? "0" + String(hourConversion) : String(hourConversion);
			tmpDisplay[2] = (minute < 10) ? "0" + String(minute) : String(minute);
			tmpDisplay[3] = (day < 10) ? "0" + String(day) + tmpDisplay[0] : String(day) + tmpDisplay[0];
			
		}

void setChannelSize(int dir){
	if (cursorY == 0){
		if (cursorX == 2){
			tmpInts[0] = tmpInts[0] + dir;
			lcd.clear();
			tmpInts[0] = (tmpInts[0] < 0) ? 0 : (tmpInts[0] > 500) ? 500 : tmpInts[0];
			String channelSize = (tmpInts[0] < 100) ? (tmpInts[0] < 10) ? "00" + String(tmpInts[0]) : "0" + String(tmpInts[0]) : String(tmpInts[0]);
			lcd.print(channelSize + " (ml) volume");
			lcd.setCursor(0, 1);
			lcd.print("<back>      <ok>");
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
		lcd.print(displaySize+"(ml) " + displayRots + " rots");
		lcd.setCursor(0, 1);
		lcd.print("<back>      <ok>");
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
		lcd.print(displayAmount + "(ml) volume");
		lcd.setCursor(0, 1);
		lcd.print("<back>      <ok>");
		lcd.setCursor(cursorX, cursorY);
	}
}

void setSessionDateTime(int dir){
	int maxDaysInMonth = days[month];
	if (cursorY == 0){
		if (cursorX == 1){
			(dir == 1) ? (sessionHour = (sessionHour + 1 > 24) ? 1 : sessionHour + 1) : (sessionHour = (sessionHour - 1 < 1) ? 24 : sessionHour - 1);
		}
		if (cursorX == 4){
			(dir == 1) ? (sessionMinute = (sessionMinute + 1 > 59) ? 0 : sessionMinute + 1) : (sessionMinute = (sessionMinute - 1 < 0) ? 59 : sessionMinute - 1);
		}
		if (cursorX == 10){
			(dir == 1) ? (sessionMonth = (sessionMonth + 1 > 11) ? 0 : sessionMonth + 1) : (sessionMonth = (sessionMonth - 1 < 0) ? 11 : sessionMonth - 1);
			sessionDay = 1;
		}
		if (cursorX == 13){
			(dir == 1) ? (sessionDay = (sessionDay + 1 > maxDaysInMonth) ? 1 : sessionDay + 1) : (sessionDay = (sessionDay - 1 < 1) ? maxDaysInMonth : sessionDay - 1);
		}
	}
	else{
		if (cursorX == 3){
			sessionYear = (dir == 1) ? sessionYear + 1 : sessionYear - 1;
		}
	}

	lcd.clear();
	captureSessionDateTimeDisplays();

	lcd.print(String(tmpDisplay[1]) + ":" + String(tmpDisplay[2]) + sessionMeridiem + " " + String(months[sessionMonth]) + " " + String(tmpDisplay[3]));
	lcd.setCursor(0, 1);
	lcd.print(String(sessionYear) + " <back> <ok>");
	lcd.setCursor(cursorX, cursorY);
}

	void captureSessionDateTime(){
		JsonObject& data = getSessionData();
		sessionHour = data["time"][0];
		sessionMinute = data["time"][1];
		sessionSec = data["time"][2];
		sessionDay = data["date"][0];
		sessionMonth = data["date"][1];
		sessionYear = data["date"][2];
		captureSessionDateTimeDisplays();
	}

		void captureSessionDateTimeDisplays(){
			int hourConversion = (sessionHour == 0) ? 12 : sessionHour;

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

			sessionMeridiem = (sessionHour >= 12 && sessionHour < 24) ? "PM" : "AM";
			tmpDisplay[0] = (sessionDay == 1 || sessionDay == 21 || sessionDay == 31) ? "st" : (sessionDay == 2 || sessionDay == 22) ? "nd" : (sessionDay == 3 || sessionDay == 23) ? "rd" : "th";
			tmpDisplay[1] = (hourConversion < 10) ? "0" + String(hourConversion) : String(hourConversion);
			tmpDisplay[2] = (sessionMinute < 10) ? "0" + String(sessionMinute) : String(sessionMinute);
			tmpDisplay[3] = (sessionDay < 10) ? "0" + String(sessionDay) + tmpDisplay[0] : String(sessionDay) + tmpDisplay[0];
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
		lcd.print(displayDelay + "(sec)  delay");
		lcd.setCursor(0, 1);
		lcd.print("<back>      <ok>");
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

	lcd.print("Repeated: " + displayRepeatBy);
	lcd.setCursor(0, 1);
	lcd.print(displayRepeat + "x <back> <ok>");
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
	lcd.print(+" " + String(tmpFloats[1]));
	lcd.write(byte(0));
	lcd.print(" PH");
	lcd.setCursor(0, 1);
	lcd.print("<back>      <ok>");
	lcd.setCursor(cursorX, 0);
}

	void setPHChannels(int dir){
		String UpDisplay;
		String DownDisplay;
		if (cursorX == 6 && cursorY == 0){
			tmpInts[0] = (tmpInts[0] + dir < 1) ? 1 : (tmpInts[0] + dir > 10) ? 10 : tmpInts[0] + dir;
		}
		if (cursorX == 14 && cursorY == 0){
			tmpInts[1] = (tmpInts[1] + dir < 1) ? 1 : (tmpInts[1] + dir > 10) ? 10 : tmpInts[1] + dir;
		}
		UpDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
		DownDisplay = (tmpInts[1] < 10) ? "0" + String(tmpInts[1]) : String(tmpInts[1]);

		lcd.clear();
		lcd.home();
		lcd.print("PH");
		lcd.write(byte(1));
		lcd.print("CH"+UpDisplay+" PH");
		lcd.write(byte(0));
		lcd.print("CH"+DownDisplay);
		lcd.setCursor(0, 1);
		lcd.print("<back>      <ok>");
		lcd.setCursor(cursorX, cursorY);
	}

void setChannelNumber(int dir){
	String totalDisplay;
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] > 10){ tmpInts[0] = 10; }
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	lcd.clear();
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	lcd.print(totalDisplay + " CHANNELS");
	lcd.setCursor(0, 1);
	lcd.print("<back>      <ok>");
	lcd.setCursor(1, 0);
}

void setSessionNumber(int dir){
	String totalDisplay;
	if (cursorX == 1 && cursorY == 0){
		tmpInts[0] = tmpInts[0] + dir;
		if (tmpInts[0] < 1){ tmpInts[0] = 1; }
	}
	lcd.clear();
	totalDisplay = (tmpInts[0] < 10) ? "0" + String(tmpInts[0]) : String(tmpInts[0]);
	lcd.print(totalDisplay + " # OF SESSIONS");
	lcd.setCursor(0, 1);
	lcd.print("<back>      <ok>");
	lcd.setCursor(1, 0);
}

void trimChannelSessions(int currentSize, int trimAmount){
	for (int i = 0; i < currentSize; i++){
		if (i > trimAmount){
			SD.remove(cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/SESSIONS/CHSES" + i);
			lcd.print("*");
		}
		Serial.flush();
	}
}
void addChannelSessions(int currentSize, int addAmount){
	int loopAmount = addAmount - currentSize;
	for (int i = 0; i < loopAmount; i++){
		makeSession(cropName + "/CHANNELS/SYSCH" + currentChannelIndex + "/SESSIONS/CHSES" + i, currentChannelIndex, currentSize+i);
		lcd.print("*");
		Serial.flush();
	}
}

void trimChannels(int currentSize, int trimAmount){
	lcd.clear();
	for (int i = 0; i < currentSize; i++){
		if (i > trimAmount){
			removeDirectory(cropName + "/CHANNELS/SYSCH" + String(i));
			lcd.print("*");
		}
		Serial.flush();
	}
}
void addChannels(int currentSize, int addAmount){
	lcd.clear();
	int loopAmount = (addAmount - currentSize) + 1;
	for (int i = 0; i < loopAmount; i++){
		makeChannel(cropName + "/CHANNELS/SYSCH" + String(currentSize + i), i, 4);
		lcd.print("*");
		Serial.flush();
	}
}

void buildCrop(){
	String channelName;
	
	File channelSettingsFile;
	File sessionSettingsFile;
	int defaultChannelSize = 10;
	int defaultSessionSize = 3;

	//Parse core file object
	JsonObject& core = getCoreData();
	core["crop"] = cropName;

	//Save change back out to core.
	setCoreData(core);

	lcd.clear();
	lcd.home();
	lcd.print("Building Crop...");
	SD.mkdir(cropName + "/SysConf");
	SD.mkdir(cropName + "/SysConf/DateTime");
	SD.mkdir(cropName + "/SysConf/ChNum");
	SD.mkdir(cropName + "/SysConf/PPM");
	SD.mkdir(cropName + "/SysConf/PH");
	SD.mkdir(cropName + "/SysConf/Open");
	SD.mkdir(cropName + "/SysConf/New");
	SD.mkdir(cropName + "/SysConf/Delete");
	SD.mkdir(cropName + "/Channels");

	//Build Crop Settings File
	StaticJsonBuffer<256> cropObjBuffer;
	JsonObject& cropSettings = cropObjBuffer.createObject();
	cropSettings["minPPM"] = 1200;
	cropSettings["maxPPM"] = 1400;
	cropSettings["minPH"] = 6.0;
	cropSettings["maxPH"] = 7.0;
	cropSettings["PHUpChannel"] = 8;
	cropSettings["PHDownChannel"] = 9;
	cropSettings["totalChannels"] = 10;
	makeNewFile(cropName + "/crop.dro", cropSettings);

	//Build Channels and their sub sessions
	lcd.setCursor(0, 1);
	for (int i = 0; i < defaultChannelSize; i++){
		makeChannel(cropName + "/Channels/SysCh" + String(i + 1), i, 4);
		lcd.print("*");
	}
	screenName = "";
	lcd.noBlink();
	lcd.clear();
	lcd.print("LOADING CROP");
	delay(1000);
	lcd.clear();
	File root = SD.open("/" + cropName);
	getDirectoryMenus(root);
	root.close();
	openHomeScreen();
}

void makeChannel(String path, int channelId, int numberOfSessions){
	const int bufferSize = 64;
	String channelName = path; 
	SD.mkdir(channelName);
	SD.mkdir(channelName + "/ChConf");
	SD.mkdir(channelName + "/ChConf/ChDoses");
	SD.mkdir(channelName + "/ChConf/ChSize");
	SD.mkdir(channelName + "/ChConf/ChCalib");

	//Build Channels Settings File
	StaticJsonBuffer<bufferSize> channelObjBuffer;
	JsonObject& channelSettings = channelObjBuffer.createObject();
	channelSettings["id"] = channelId;
	channelSettings["size"] = 80;
	channelSettings["sessionsTotal"] = 4;
	channelSettings["calibration"] = 0;

	makeNewFile(channelName + "/channel.dro", channelSettings);

	for (int j = 0; j < numberOfSessions; j++){
		makeSession(channelName + "/Sessions/ChSes" + String(j + 1), channelId, j);
	}
	Serial.flush();
}

void makeSession(String path, int channelId, int sessionId){
	const int bufferSize = 128;
	String sessionName;
	sessionName = path;
	SD.mkdir(sessionName);
	SD.mkdir(sessionName + "/Amt");
	SD.mkdir(sessionName + "/Str");
	SD.mkdir(sessionName + "/Dly");
	SD.mkdir(sessionName + "/Rpt");

	//Build Session's settings file
	StaticJsonBuffer<bufferSize> sessionObjBuffer;
	char sessionBuffer[bufferSize];
	JsonObject& sessionSettings = sessionObjBuffer.createObject();
	StaticJsonBuffer<128> dateBuffer;
	StaticJsonBuffer<128> timeBuffer;
	JsonArray& date = dateBuffer.createArray();
	date.add(rtc.getTime().date); //day
	date.add((rtc.getTime().mon + 1)); //month
	date.add(rtc.getTime().year); //year
	JsonArray& time = timeBuffer.createArray();
	time.add(0); //hour
	time.add(0); //min
	time.add(0); //sec
	sessionSettings["id"] = (sessionId+1);
	sessionSettings["channel"] = (channelId+1);
	sessionSettings["ammount"] = 80;
	sessionSettings["date"] = date;
	sessionSettings["time"] = time;
	sessionSettings["delay"] = 0;
	sessionSettings["repeat"] = 0;
	sessionSettings["repeatBy"] = 0; //0 none, 1 = hourly, 2 = daily, 3 = weekly, 4 = monthly, 5 = yearly
	makeNewFile(sessionName + "/session.dro", sessionSettings);
}
