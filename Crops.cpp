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
#include "Timers.h"

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
		printHomeScreen();
	}
}

JsonObject& getECData(JsonBuffer& b, byte ecRangeIndex = 1){
	tmpFile = SD.open("dromatic/" + cropName + "/Crop/EC/ECRng" + ecRangeIndex + ".dro", O_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void setECData(JsonObject& d, byte ecRangeIndex = 1){
	char b[512];
	tmpFile = SD.open("dromatic/" + cropName + "/Crop/EC/ECRng" + ecRangeIndex + ".dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
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
	screenName = "NEW";
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
		cropName += nameArry[i];
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
	byte i, j, defaultRegimensSize, defaultTimerSize;
	defaultRegimensSize = 12;
	defaultTimerSize = 4;

	//Parse core file object
	StaticJsonBuffer<coreBufferSize> coreBuffer;
	JsonObject& core = getCoreData(coreBuffer);
	core["crop"] = cropName;
	setCoreData(core);

	lcd.clear();
	lcd.home();
	lcd.print(F("Building Crop..."));

	/////////////////////////////////////////////////////////////////////////
	//Build files structure that we use for menu system					   //
	//(file system is abbreveated and translated via globals.cpp's PROGMEM)//
	/////////////////////////////////////////////////////////////////////////

	//System Settings
	SD.mkdir("dromatic/" + cropName + "/Sys");
	SD.mkdir("dromatic/" + cropName + "/Sys/DateTime");
	SD.mkdir("dromatic/" + cropName + "/Sys/Open");
	SD.mkdir("dromatic/" + cropName + "/Sys/New");
	SD.mkdir("dromatic/" + cropName + "/Sys/Delete");
	SD.mkdir("dromatic/" + cropName + "/Sys/Reset");
	SD.mkdir("dromatic/" + cropName + "/Sys/ECCal");
	SD.mkdir("dromatic/" + cropName + "/Sys/PHCal");

	//Crop Settings
	SD.mkdir("dromatic/" + cropName + "/Crop");
	SD.mkdir("dromatic/" + cropName + "/Crop/EC");
	SD.mkdir("dromatic/" + cropName + "/Crop/PH");
	SD.mkdir("dromatic/" + cropName + "/Crop/Doses");

	//Channels and volumes for each channel's regimen
	SD.mkdir("dromatic/" + cropName + "/Chs");

	//Irrigation settings
	SD.mkdir("dromatic/" + cropName + "/Irri");
	SD.mkdir("dromatic/" + cropName + "/Irri/RsvrVol");
	SD.mkdir("dromatic/" + cropName + "/Irri/TpfCcnt");
	SD.mkdir("dromatic/" + cropName + "/Irri/TpfVol");
	SD.mkdir("dromatic/" + cropName + "/Irri/DrnTime");
	SD.mkdir("dromatic/" + cropName + "/Irri/FlowCal");



	//////////////////////
	//Build System Files//
	//////////////////////

	//Crop file
	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& crop = cropBuffer.createObject();
	JsonArray& EC = crop.createNestedArray("ec");
	JsonArray& PH = crop.createNestedArray("ph");
	crop["ppm"] = 5;
	crop["regimens"] = 12;
	crop["status"] = "paused";

	//Default EC/PH Ranges for both reservoir and plant water
	EC.add(1200);
	EC.add(1600);

	PH.add(5.70);
	PH.add(6.00);



	//? I dunno, might need it still.
	lcd.setCursor(0, 1);

	//Irrigation File
	StaticJsonBuffer<irrigateBufferSize> irrigateBuffer;
	JsonObject& irrigate = irrigateBuffer.createObject();
	JsonArray& flMeters = irrigate.createNestedArray("flMeters");
	irrigate["rsvrvol"] = 25;
	irrigate["tpfccnt"] = 2;
	irrigate["tpfvol"] = 2;
	irrigate["drntime"] = 10;
	flMeters.add(2.25);
	flMeters.add(2.25);

	//Timers File
	StaticJsonBuffer<timerBufferSize> timerBuffer;
	JsonObject& timer = timerBuffer.createObject();
	JsonArray& weeks = timer.createNestedArray("weeks");
	JsonArray& currents = timer.createNestedArray("currents");

	//EC Range File
	StaticJsonBuffer<ecBufferSize> ecRangeBuffer;
	JsonObject& ecRange = ecRangeBuffer.createObject();
	JsonArray& ec = ecRange.createNestedArray("ec");
	ec.add(500);
	ec.add(700);

	//Channels file
	StaticJsonBuffer<channelBufferSize> channelBuffer;
	JsonObject& channel = channelBuffer.createObject();
	JsonArray& currentRegimens = channel.createNestedArray("currents");
	for (i = 1; i <= 8; i++){
		currentRegimens.add(1);
	}
	channel["calibration"] = 100;

	//Session files (these live under each channel's session)
	StaticJsonBuffer<32> sessionBuffer;
	JsonObject& session = sessionBuffer.createObject();
	session["expired"] = false;
	session["ml"] = 0.0;

	//Make channel folders and their sessions files
	for (i = 1; i <= 8; i++){
		channel["id"] = i;
		makeChannel("dromatic/" + cropName + "/Chs/SysCh" + i, 12, session);
		lcd.print(F("*"));
		Serial.flush();
	}

	//Timer settings
	SD.mkdir("dromatic/" + cropName + "/Timers");
	for (i = 1; i <= 4; i++){ //4 receptacals
		SD.mkdir("dromatic/" + cropName + "/Timers/Recep0" + i);
		makeTimerSessionFile("dromatic/" + cropName + "/Timers/Recep0" + i, i);
	}

	//EC/PPM ranges
	for (i = 1; i <= 12; i++){ //4 receptacals
		makeECRangeFile("dromatic/" + cropName + "/Crop/EC", ecRange, i);
	}

	//Now we make the actual files 
	//(must happen after for loop cause we are gathering session ids above first)
	makeNewFile("dromatic/" + cropName + "/crop.dro", crop);
	makeNewFile("dromatic/" + cropName + "/irrigate.dro", irrigate);
	makeNewFile("dromatic/" + cropName + "/channel.dro", channel);
	makeNewFile("dromatic/" + cropName + "/Timer.dro", timer);

	screenName = "";
	lcd.noBlink();
	lcd.clear();
	File root = SD.open("dromatic/" + cropName);
	getDirectoryMenus(root);
	root.close();
	printHomeScreen();
}

void makeECRangeFile(String path, JsonObject& data, int index){
	char buffer[24];
	tmpFile = SD.open(path + "/ECRng" + index + ".dro", FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
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
	lcd.print(tmpFloats[0]);
	lcd.write(byte(1));
	lcd.print(F(" "));
	lcd.print(tmpFloats[1]);
	lcd.write(byte(0));
	lcd.print(F(" PH"));
	lcd.setCursor(0, 1);
	lcd.print(F("<back>      <ok>"));
	lcd.setCursor(cursorX, 0);
}

void setECRange(int dir){
	lcd.clear();
	lcd.print(maxRegimens);
	delay(2000);
	lcd.home();
	String EC1Display, EC2Display;

	if ((dir == 1) ? cursorX == 3 : cursorX == 8){
		(dir == 1) ? tmpInts[0] = tmpInts[0] + 10 : tmpInts[1] = tmpInts[1] - 10;
		if ((dir == 1) ? tmpInts[0] > (tmpInts[1] - 50) : tmpInts[1] < (tmpInts[0] + 50)){
			(dir == 1) ? tmpInts[1] = tmpInts[1] + 10 : tmpInts[0] = tmpInts[0] - 10;
		}
	}
	if ((dir == 1) ? cursorX == 8 : cursorX == 3) {
		(dir == 1) ? tmpInts[1] = tmpInts[1] + 10 : tmpInts[0] = tmpInts[0] - 10;
	}

	//min \ max check
	if (tmpInts[0] < 0){
		tmpInts[0] = 0;
	}
	else if (tmpInts[0] > 9950){
		tmpInts[0] = 9950;
	}

	if (tmpInts[1] < 50){
		tmpInts[1] = 50;
	}
	else if (tmpInts[1] > 9999){
		tmpInts[1] = 9999;
	}

	if (cursorX == 15 && cursorY == 0){
		StaticJsonBuffer<ecBufferSize> ecSaveBuffer;
		JsonObject& ecSaveData = getECData(ecSaveBuffer, tmpInts[2]);
		ecSaveData["ec"].asArray()[0] = tmpInts[0];
		ecSaveData["ec"].asArray()[1] = tmpInts[1];
		setECData(ecSaveData, tmpInts[2]);

		(dir == 1) ? ((tmpInts[2] + dir) >= maxRegimens) ? tmpInts[2] = maxRegimens : tmpInts[2] += dir : (tmpInts[2] + dir < 1) ? 1 : tmpInts[2] += dir;

		StaticJsonBuffer<ecBufferSize> ecOpenBuffer;
		JsonObject& ecOpenData = getECData(ecOpenBuffer, tmpInts[2]);
		tmpInts[0] = ecOpenData["ec"].asArray()[0];
		tmpInts[1] = ecOpenData["ec"].asArray()[1];
	}

	EC1Display = (tmpInts[0] >= 1000) ? F("") : (tmpInts[0] >= 100) ? F("0") : (tmpInts[0] >= 10) ? F("00") : F("000");
	EC2Display = (tmpInts[1] >= 1000) ? F("") : (tmpInts[1] >= 100) ? F("0") : (tmpInts[1] >= 10) ? F("00") : F("000");

	lcd.clear();
	lcd.print(EC1Display + String(tmpInts[0]));
	lcd.write(byte(1));
	lcd.print(EC2Display + String(tmpInts[1]));
	lcd.write(byte(0));
	if (tmpInts[2] >= 10){
		lcd.print(F(" EC/"));
		lcd.print(tmpInts[2]);
	}
	else{
		lcd.print(F(" EC/0"));
		lcd.print(tmpInts[2]);
	}
	lcd.setCursor(0, 1);
	lcd.print(F("<back>    <done>"));
	lcd.setCursor(cursorX, 0);
}

void setPPMHundredth(int dir){
	PPMHundredth = ((dir == 1)) ? 7 : 5;
}