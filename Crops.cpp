/*
*  DROMatic.ino
*  DROMatic OS Crops
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Crops.h"
#include "Globals.h"
#include "Core.h"
#include "Channels.h"
#include "Menus.h"
#include "DatesTime.h"
#include "Screens.h"
#include "Regimens.h"
#include "Timers.h"
#include "Irrigation.h"

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


void cropChange(){
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
	menuIndex = currentChannelIndex = currentRegimenIndex = currentAlphaIndex = 0;
	menus.clear();
	menusHistory.clear();
	coreInit();
}

void cropCreate(){
	screenName = "NEW";
	lcd.clear();
	lcd.setCursor(0, 1);
	lcd.print(F("Crop Name <done>"));
	lcd.home();
	lcd.blink();
}

void cropRename(int dir){
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

void cropBuild(){
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
	SD.mkdir("dromatic/" + cropName);
	SD.mkdir("dromatic/" + cropName + "/Sys");
	SD.mkdir("dromatic/" + cropName + "/Sys/DateTime");
	SD.mkdir("dromatic/" + cropName + "/Sys/Open");
	SD.mkdir("dromatic/" + cropName + "/Sys/New");
	SD.mkdir("dromatic/" + cropName + "/Sys/Start");
	SD.mkdir("dromatic/" + cropName + "/Sys/Pause");
	SD.mkdir("dromatic/" + cropName + "/Sys/Delete");
	SD.mkdir("dromatic/" + cropName + "/Sys/Reset");
	SD.mkdir("dromatic/" + cropName + "/Sys/ECCal");
	SD.mkdir("dromatic/" + cropName + "/Sys/PHCal");
	SD.mkdir("dromatic/" + cropName + "/Sys/PumpCal");

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
	SD.mkdir("dromatic/" + cropName + "/Irri/TpfAmt");
	SD.mkdir("dromatic/" + cropName + "/Irri/TpfDly");
	SD.mkdir("dromatic/" + cropName + "/Irri/DrnTime");
	SD.mkdir("dromatic/" + cropName + "/Irri/FlowCal");
	SD.mkdir("dromatic/" + cropName + "/Irri/ManFlush");


	//////////////////////
	//Build System Files//
	//////////////////////

	//Crop file
	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& crop = cropBuffer.createObject();
	JsonArray& PH = crop.createNestedArray("ph");
	crop["ppm"] = 5;
	crop["maxRegimens"] = 12;
	crop["currentRegimen"] = 1;
	crop["status"] = 0; //0 = paused or stopped, 1 = running
	crop["feedingType"] = 0; //0 = full dose feeding, 1 = topoff dose feeding
	PH.add(5.70);
	PH.add(6.00);

	//? I dunno, might need it still.
	lcd.setCursor(0, 1);

	//Irrigation File
	StaticJsonBuffer<irrigateBufferSize> irrigateBuffer;
	JsonObject& irrigate = irrigateBuffer.createObject();
	JsonArray& flMeters = irrigate.createNestedArray("flMeters");
	irrigate["rsvrvol"] = 0;
	irrigate["tpfccnt"] = 2;
	irrigate["tpfdly"] = 5;
	irrigate["tpfamt"] = 1;
	irrigate["currentVol"] = 0;
	irrigate["drntime"] = 10;
	flMeters.add(2.25);
	flMeters.add(2.25);

	//Timers File
	StaticJsonBuffer<timerBufferSize> timerBuffer;
	JsonObject& timer = timerBuffer.createObject();
	JsonArray& weeks = timer.createNestedArray("weeks");
	JsonArray& currents = timer.createNestedArray("currents");
	weeks.add(1);
	weeks.add(1);
	weeks.add(1);
	weeks.add(1);
	currents.add(1);
	currents.add(1);
	currents.add(1);
	currents.add(1);

	//EC Range File
	StaticJsonBuffer<ecBufferSize> ecRangeBuffer;
	JsonObject& ecRange = ecRangeBuffer.createObject();
	JsonArray& ec = ecRange.createNestedArray("ec");
	ec.add(500);
	ec.add(700);

	//Channels file
	StaticJsonBuffer<channelBufferSize> channelBuffer;
	JsonObject& channel = channelBuffer.createObject();
	channel["calibration"] = 100; //in hundredths
	channel["delay"] = 3;

	//Regimen files (these live under each channel's session)
	StaticJsonBuffer<32> regimenBuffer;
	JsonObject& regimen = regimenBuffer.createObject();
	regimen["expired"] = false;
	regimen["ml"] = 0.0;

	//Make channel folders and their sessions files
	for (i = 1; i <= 8; i++){
		channel["id"] = i;
		channelCreate("dromatic/" + cropName + "/Chs/SysCh" + i, 12, regimen);
		lcd.print(F("*"));
		Serial.flush();
	}

	//Timer settings
	SD.mkdir("dromatic/" + cropName + "/Timers");
	for (i = 1; i <= 4; i++){ //4 receptacals
		SD.mkdir("dromatic/" + cropName + "/Timers/Recep0" + i);
		makeNewFile("dromatic/" + cropName + "/Timers/Recep0" + i + "/Week1.dro", newTimerSessionData());
	}

	//EC/PPM ranges
	for (i = 1; i <= 12; i++){ //4 receptacals
		makeNewFile("dromatic/" + cropName + "/Crop/EC/ECRng" + i + ".dro", ecRange);
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

void cropLoad(){
	//load timer currents
	StaticJsonBuffer<timerBufferSize> timersBuffer;
	JsonObject& timersData = getTimerData(timersBuffer);
	currentTimerSessions[0] = timersData["currents"].asArray()[0];
	currentTimerSessions[1] = timersData["currents"].asArray()[1];
	currentTimerSessions[2] = timersData["currents"].asArray()[2];
	currentTimerSessions[3] = timersData["currents"].asArray()[3];

	StaticJsonBuffer<cropBufferSize> cropBuffer;
	JsonObject& cropData = getCropData(cropBuffer);
	//load crops current regimen number
	currentRegimen = cropData["currentRegimen"];
	//load crops max regimen number
	maxRegimens = cropData["maxRegimens"];
	//load crops min pH setting
	minPH = cropData["ph"].asArray()[0];
	//load crops max pH setting
	maxPH = cropData["ph"].asArray()[1];
	//load current feeding type
	feedingType = cropData["feedingType"];

	StaticJsonBuffer<ecBufferSize> ecBuffer;
	JsonObject& ECData = getECData(ecBuffer, currentRegimen);
	//load current regimen min PPM
	minPPM = ECData["ec"].asArray()[0];
	//load current regimen max PPM
	maxPPM = ECData["ec"].asArray()[1];

	StaticJsonBuffer<irrigateBufferSize> irrigationBuffer;
	JsonObject& irrigationData = getIrrigationData(irrigationBuffer);
	//load reservoir volume
	rsvrVol = irrigationData["rsvrvol"];
	//load last knowen reservoir volume
	currentRsvrVol = irrigationData["currentvol"];
	//load plant water drain time
	drainTime = irrigationData["drntime"];
	//load topoff concentrate
	topOffConcentrate = irrigationData["tpfcnt"];
	//load topoff amounts
	topOffAmount = irrigationData["tpfamt"];
	//load topoff delays
	topOffDelay = irrigationData["tpfdly"];


	//load flow config
	flowMeterConfig[0] = irrigationData["flMeters"].asArray()[0];
	flowMeterConfig[1] = irrigationData["flMeters"].asArray()[1];

	//load current topoff count

	StaticJsonBuffer<channelBufferSize> channelsBuffer;
	JsonObject& channelsData = getChannelsData(channelsBuffer);
	//load pump config
	pumpCalibration = channelsData["calibration"];
	//load pump delay
	pumpDelay = channelsData["delay"];

	//Check recepticals before proceeding
	checkRecepticals();
}

//Saves
void saveECData(){
	if (cursorX == 11 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<ecBufferSize> ecBuffer;
		JsonObject& ecData = getECData(ecBuffer, tmpInts[2]);
		ecData["ec"].asArray()[0] = minPPM = tmpInts[0];
		ecData["ec"].asArray()[1] = maxPPM = tmpInts[1];
		setECData(ecData, tmpInts[2]);
	}
	if (cursorX == 1 || cursorX == 11 && cursorY == 1){
		exitScreen();
	}
}
void savePHData(){
	if (cursorX == 13 && cursorY == 1){
		lcd.clear();
		lcd.home();
		StaticJsonBuffer<cropBufferSize> jsonBuffer;
		JsonObject& data = getCropData(jsonBuffer);
		data["ph"].asArray()[0] = minPH = tmpFloats[0];
		data["ph"].asArray()[1] = maxPH = tmpFloats[1];
		setCropData(data, false);
	}
	if (cursorX == 1 || cursorX == 13 && cursorY == 1){
		exitScreen();
	}
}

//Prints
void printPHRange(double dir = 0){
	float minMaxDiff = 0.01;
	if (dir == 0){
		StaticJsonBuffer<cropBufferSize> buffer;
		JsonObject& data = getCropData(buffer);
		tmpFloats[0] = data["ph"].asArray()[0];
		tmpFloats[1] = data["ph"].asArray()[1];
		cursorX = 3;
		cursorY = 0;
	} else {
		if ((dir == 1) ? cursorX == 3 : cursorX == 9){
			(dir == 1) ? tmpFloats[0] = tmpFloats[0] + minMaxDiff : tmpFloats[1] = tmpFloats[1] - minMaxDiff;
			if ((dir == 1) ? tmpFloats[0] > (tmpFloats[1] - minMaxDiff) : tmpFloats[1] < (tmpFloats[0] + minMaxDiff)){
				(dir == 1) ? tmpFloats[1] = tmpFloats[1] + minMaxDiff : tmpFloats[0] = tmpFloats[0] - minMaxDiff;
			}
		}
		if ((dir == 1) ? cursorX == 9 : cursorX == 3) {
			(dir == 1) ? tmpFloats[1] = tmpFloats[1] + minMaxDiff : tmpFloats[0] = tmpFloats[0] - minMaxDiff;
		}
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
void printECRange(int dir = 0){
	String EC1Display, EC2Display;
	if (dir == 0){ //first print
		currentRegimenIndex = tmpInts[2] = 1;
		String EC1Display, EC2Display;
		StaticJsonBuffer<ecBufferSize> ecBuffer;
		JsonObject& ecData = getECData(ecBuffer, currentRegimenIndex);
		tmpInts[0] = ecData["ec"].asArray()[0];
		tmpInts[1] = ecData["ec"].asArray()[1];
	} else {//subsequent prints after first
		if ((dir == 1) ? cursorX == 3 : cursorX == 8){
			(dir == 1) ? tmpInts[0] = tmpInts[0] + 10 : tmpInts[1] = tmpInts[1] - 10;
			if ((dir == 1) ? tmpInts[0] > (tmpInts[1] - 50) : tmpInts[1] < (tmpInts[0] + 50)){
				(dir == 1) ? tmpInts[1] = tmpInts[1] + 10 : tmpInts[0] = tmpInts[0] - 10;
			}
		}
		if ((dir == 1) ? cursorX == 8 : cursorX == 3) {
			(dir == 1) ? tmpInts[1] = tmpInts[1] + 10 : tmpInts[0] = tmpInts[0] - 10;
		}
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
		lcd.clear();
		lcd.home();
		lcd.print((dir == 1) ? F("NEXT REGIMEN") : F("PREV REGIMEN"));

		lcd.setCursor(0, 1);
		lcd.print(F("PLEASE HOLD..."));
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

void printECCalibrations(String type, int dir = 0){
	if (dir != 0){
		if (cursorX == 10){
			if (dir > 0){ tmpInts[0]++; }
			else{ tmpInts[0]--; }
			if (tmpInts[0] > 9){ tmpInts[0] = 0; }
			if (tmpInts[0] < 0){ tmpInts[0] = 9; }
		}

		if (cursorX == 11){
			if (dir > 0){ tmpInts[1]++; }
			else { tmpInts[1]--; }
			if (tmpInts[1] > 9){ tmpInts[1] = 0; }
			if (tmpInts[1] < 0){ tmpInts[1] = 9; }
		}
		if (cursorX == 12){
			if (dir > 0){ tmpInts[2]++; }
			else { tmpInts[2]--; }
			if (tmpInts[2] > 9){ tmpInts[2] = 0; }
			if (tmpInts[2] < 0){ tmpInts[2] = 9; }
		}
		if (cursorX == 13){
			if (dir > 0){ tmpInts[3]++; }
			else { tmpInts[3]--; }
			if (tmpInts[3] > 9){ tmpInts[3] = 0; }
			if (tmpInts[3] < 0){ tmpInts[3] = 9; }
		}
		if (cursorX == 14){
			if (dir > 0){ tmpInts[4]++; }
			else { tmpInts[4]--; }
			if (tmpInts[4] > 9){ tmpInts[4] = 0; }
			if (tmpInts[4] < 0){ tmpInts[4] = 9; }
		}
		if (cursorX == 15){
			if (dir > 0){ tmpInts[5]++; }
			else { tmpInts[5]--; }
			if (tmpInts[5] > 9){ tmpInts[5] = 0; }
			if (tmpInts[5] < 0){ tmpInts[5] = 9; }
		}
	}
	else{
		lcd.noBlink();
		lcd.clear();
		lcd.home();
		lcd.print(type);
		lcd.print(F(" CALIBRATION"));
		lcd.setCursor(0, 1);
		lcd.print(F("STARTING NOW"));
		delay(5000);

		if (type == "DRY"){
			lcd.clear();
			lcd.print(F("REMOVE PROBES"));
			lcd.setCursor(0, 1);
			lcd.print(F("FROM WATER NOW!"));
			lcd.home();
			delay(5000);
		}

		if (type == "HIGH" || type == "LOW"){
			lcd.clear();
			lcd.home();
			lcd.print(F("PLACE EC PROBES"));
			lcd.setCursor(0, 1);
			lcd.print(F("IN "));
			lcd.print(type);
			lcd.print(F(" SOLUTION"));
			delay(5000);
		}
	}

	lcd.clear();
	lcd.home();
	if (type == "DRY" || type == "LOW"){
		lcd.print(type);
		lcd.print(F(" CAL - "));
	}
	if (type == "HIGH"){
		lcd.print(type);
		lcd.print(F(" CAL- "));
	}
	if (dir == 0){
		tmpInts[0] = tmpInts[1] = tmpInts[2] = tmpInts[3] = tmpInts[4] = tmpInts[5] = 0;
		lcd.print(F("000000"));
	} else {
		lcd.print(tmpInts[0]);
		lcd.print(tmpInts[1]);
		lcd.print(tmpInts[2]);
		lcd.print(tmpInts[3]);
		lcd.print(tmpInts[4]);
		lcd.print(tmpInts[5]);
	}

	lcd.setCursor(0, 1);
	if (type == "DRY"){
		lcd.print(F("<cancel>  <next>"));
	}
	if (type == "LOW"){
		lcd.print(F("<back>    <next>"));
	}
	if (type == "HIGH"){
		lcd.print(F("<back>    <done>"));
	}
	if (dir == 0){
		cursorX = 15;
		cursorY = 0;
	}
	lcd.setCursor(cursorX, cursorY);
}
void printPHCalibrations(String type, byte value){
	lcd.noBlink();
	lcd.clear();
	lcd.home();
	lcd.print(type);
	lcd.print(F(" CALIBRATION"));
	lcd.setCursor(0, 1);
	lcd.print(F("STARTING NOW"));
	delay(5000);

	lcd.clear();
	lcd.print(F("PLACE PROBES"));
	lcd.setCursor(0, 1);
	lcd.print(F("IN "));
	lcd.print(value);
	lcd.print(F(" SOLUTION"));
	lcd.home();
	delay(5000);

	lcd.clear();
	lcd.home();
	lcd.print(type);
	lcd.print(F(" CAL - "));
	lcd.print(value);
	lcd.print(F(".0"));
	lcd.setCursor(0, 1);
	if (type == "LOW"){
		lcd.print(F("<cancel>  <next>"));
	}else if (type == "HI"){
		lcd.print(F("<back>    <done>"));
	}else {
		lcd.print(F("<back>    <next>"));
	}
	cursorX = 0;
	cursorY = 0;
	lcd.setCursor(cursorX, cursorY);
	lcd.blink();
}
