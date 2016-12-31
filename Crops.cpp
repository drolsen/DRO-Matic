/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Crops.h"
#include "Globals.h"
#include "Core.h"
#include "Channels.h"
#include "Menus.h"
#include "DatesTime.h"
#include "Screens.h"
#include "Sessions.h"

String cropName;

JsonObject& getCropData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/crop.dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setCropData(JsonObject& d, bool returnHome = true){
	char b[512];
	tmpFile = SD.open("dromatic/" + cropName + "/crop.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	if (returnHome){
		openHomeScreen();
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

void startNewCrop(){
	screenName = "NewCrop";
	lcd.clear();
	lcd.setCursor(0, 1);
	lcd.print(F("Crop Name <done>"));
	lcd.home();
	lcd.blink();
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

	sessionDate.add((currentMonth > nextMonth) ? nextYear : currentYear);		//year
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