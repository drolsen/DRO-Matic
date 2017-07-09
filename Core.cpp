/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#include "Core.h"
#include "Crops.h"
#include "Pumps.h"
#include "Regimens.h"
#include "Menus.h"
#include "Screens.h"
#include "DatesTime.h"
#include "Irrigation.h"
#include "Timers.h"

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

void coreInit(){
	if (SD.exists("dromatic")){ //has OS already been setup?
		StaticJsonBuffer<coreBufferSize> coreBuffer;
		JsonObject& coreData = getCoreData(coreBuffer);

		cropName = coreData["crop"].asString();
		if (cropName != "" && SD.exists("dromatic/" + cropName)){ //Loading up exisiting core file's crop directory
			screenName = "";
			tmpFile = SD.open("dromatic/" + cropName);
			getDirectoryMenus(tmpFile);
			tmpFile.close();
			lcd.print(F("LOADING CROP..."));
			lcd.setCursor(0, 1);
			lcd.print(F("  PLEASE HOLD  "));
			lcd.home();
			cropLoad();
			printHomeScreen();
		}
		else{ //we have core file with crop, but no crop directory. //VERY CORNER CASE!
			cropCreate();
		}
	}
	else { //if OS has not been setup, lets build out core OS file / directory
		SD.mkdir("dromatic");
		lcd.print(F("New Drive Found!"));
		lcd.setCursor(0, 1);
		lcd.print(F(" Please Hold... "));
		tmpFile = SD.open("dromatic/core.dro", FILE_WRITE);
		char buffer[64];
		StaticJsonBuffer<64> coreBuffer;
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
		cropCreate();
	}
}

//converts tmpInts array into a whole number that send to our EC circuts.
int tmpIntsToInt(byte decimalPlaces){
	if (decimalPlaces > ((sizeof tmpInts) / (sizeof *tmpInts))){
		decimalPlaces = ((sizeof tmpInts) / (sizeof *tmpInts)); //we never want to exceed the size of our tmpInts array
	}
	String number;
	for (byte i = 0; i <= decimalPlaces; i++){
		number += tmpInts[i];
	}
	return number.toInt(); //return our string that is converte to int
}

//time feed plants some top off water?
void correctPlantEC(){
	if (flowInRate > 0){ return; } //we are not allowed to topoff plant water if rsvr is filling up flowInRate
	int PPM = getWaterProbeValue(0);
	if ((PPM > maxPPM || PPM < minPPM) && feedingType == 1){
		lcd.clear();
		lcd.home();

		Time current = rtc.getTime();
		StaticJsonBuffer<cropBufferSize> cropBuffer;
		JsonObject& cropData = getCropData(cropBuffer);

		if (lastFeedingWeek != current.dow && lastFeedingDay == calcDayOfWeek(current.year, current.mon, current.date)){
			//we seem to have run out of time for this feeding water before we have run out of feeding water.
			//full flushing must happen now
			fullFlushing();
			feedingType = 0; //now switch to full feeding for moving onto next regimen
			cropData["feedingType"] = feedingType;
			setCropData(cropData);
			return;
		}

		lcd.print(F("TOPPING OFF EC"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		while (currentRsvrVol > (currentRsvrVol - topOffAmount)){
			RelayToggle(11, true);
			detachInterrupt(digitalPinToInterrupt(FlowPinIn));
			detachInterrupt(digitalPinToInterrupt(FlowPinOut));
			checkFlowRates();
			if (flowInRate > 0){//OH CRAP! flowInRate is true, so we must of run out of reservoir topoff water.
				//but was EC correction successful?.. if not, time to flush plants!
				RelayToggle(11, false); //close up in-valve finish feeding early
				feedingType = 0; //now switch to full feeding for moving onto next regimen
				cropData["feedingType"] = feedingType;
				setCropData(cropData);
				break; //finally we break out of while loop for topOff feeding early
			}
			attachInterrupt(digitalPinToInterrupt(FlowPinIn), countRsvrFill, RISING);
			attachInterrupt(digitalPinToInterrupt(FlowPinOut), countRsvrDrain, RISING);
		}
		RelayToggle(11, false); //close up in-valve to finish feeding
	}
}

//is it time to fix ph dift of plant water?
void correctPlantPH(){
	//are we permitted to correct plant pH?
	if (flowOutRate > 0 && ((millis() - phPlantMillis) < 60000)) { return; }
	tmpFloats[0] = getWaterProbeValue(1);

	//is current ph is outside of configred ph ranges?
	if (tmpFloats[0] > maxPH || tmpFloats[0] < minPH){
		lcd.clear();
		lcd.home();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		if (tmpFloats[0] > maxPH){ //we must micro-ph-dose our plant water DOWN
			pumpSpin(1, 9, pumpCalibration);
			phPlantMillis = millis();
			tmpFloats[0] = 0;
			printHomeScreen();
		}
		if (tmpFloats[0] < minPH){ //we must micro-ph-dose our plant water UP
			pumpSpin(1, 10, pumpCalibration);
			phPlantMillis = millis();
			tmpFloats[0] = 0;
			printHomeScreen();
		}
	}
}

//is it time fix reservoir pH drift?
void correctRsvrPH(){
	//are we permitted to correct reservoir pH?
	if ((flowInRate > 0 || flowOutRate > 0) && ((millis() - phRsvrMillis) < 60000)) { return; }
	tmpFloats[0] = getWaterProbeValue(1);

	//if current ph is outside of configred ph ranges
	if (tmpFloats[0] > maxPH || tmpFloats[0] < minPH){
		lcd.clear();
		lcd.home();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		pumpSpin(1, 8, pumpCalibration);
		phRsvrMillis = millis();
		tmpFloats[0] = 0;
		printHomeScreen();
	}else{
		//if ph corrected and 5 mins has past since last pH dosing
		if ((millis() - phRsvrMillis) > 600000){
			checkRegimenDosing();
		}
	}
}

//Open channel of tentical sheild to get probe reading value
void openWaterProbeChannel(int channel) {
	Wire.beginTransmission(channel_ids[channel]);     // call the circuit by its ID number.
}

//Get either EC or pH live probe values
int getWaterProbeValue(byte channel = 111){ //default is channel 0 aka EC1. 0 = EC1, 1 = PH1, 2 = EC2, 3 = PH2
	int data;
	openWaterProbeChannel(channel);     // open EC1 tentical shield channel
	Wire.write('r');                          // request a reading by sending 'r'
	Wire.endTransmission();                         // end the I2C data transmission.
	delay(1000);  // AS circuits need a 1 second before the reading is ready
	
	sensor_bytes_received = 0;                        // reset data counter
	memset(sensordata, 0, sizeof(sensordata));        // clear sensordata array;

	Wire.requestFrom(channel_ids[channel], 48, 1);    // call the circuit and request 48 bytes (this is more then we need).
	code = Wire.read();
	while (Wire.available()) {          // are there bytes to receive?
		in_char = Wire.read();            // receive a byte.
		if (in_char == 0) {               // null character indicates end of command
			Wire.endTransmission();         // end the I2C data transmission.
			break;                          // exit the while loop, we're done here
		}
		else {
			sensordata[sensor_bytes_received] = in_char;      // append this byte to the sensor data array.
			sensor_bytes_received++;
		}
	}
	String returnedValue;
	for (byte i = 0; i < 4; i++){
		returnedValue += sensordata[i];
	}
	if (channel == 111 || channel == 113){
		return returnedValue.toInt();
	}
	else{
		return returnedValue.toFloat();
	}
}

//Three point pH water probe calibration
void setPHWaterProbeCalibration(byte channel, int value, char type){
	openWaterProbeChannel(channel);
	delay(100);
	if (type == 'low'){
		Wire.write("Cal,low," + value);  // Send the command from OS to the Atlas Scientific device for low calibration of pH probe
		Wire.write("\r"); // <CR> carriage return to terminate message
	}
	if (type == 'mid'){
		Wire.write("Cal,mid," + value);  // Send the command from OS to the Atlas Scientific device for mid calibration of pH probe
		Wire.write("\r"); // <CR> carriage return to terminate message
	}
	if (type == 'high'){
		Wire.write("Cal,high," + value);  // Send the command from OS to the Atlas Scientific device for high calibration of pH probe
		Wire.write("\r"); // <CR> carriage return to terminate message
	}
}

//Three point EC water probe calibration
void setECWaterProbeCalibration(byte channel, int value, char type){
	openWaterProbeChannel(channel);
	delay(100);
	if (type == 'dry'){
		Wire.write("Cal,dry,0");  // Manufacture says this calibration only needs to happen once, but never said it can't happen more than once, so we include it in all EC probrobe calibrations
		Wire.write("\r"); // <CR> carriage return to terminate message
	}
	if (type == 'low'){
		Wire.write("Cal,low," + value);  // Send the command from OS to the Atlas Scientific device for mid calibration of pH probe
		Wire.write("\r"); // <CR> carriage return to terminate message
	}
	if (type == 'high'){
		Wire.write("Cal," + value);  // There is no "high" value for this command cause this calibration only has low + high, or high as single point calibration
		Wire.write("\r"); // <CR> carriage return to terminate message
	}
}

//is it time to turn on/off timer
void checkRecepticals(){
	byte currentHour, currentDOW, startHour, endHour, currentReceptical, currentRecepticalWeek;
	currentHour = rtc.getTime().hour;
	currentDOW = rtc.getTime().dow;
	for (byte i = 0; i < 4; i++){
		StaticJsonBuffer<timerSessionBufferSize> timerSessionBuffer;
		currentReceptical = 13 + i;
		JsonObject& sessionData = getTimerSessionData(timerSessionBuffer, (i + 1), currentTimerSessions[i]);
		startHour = sessionData["times"].asArray()[currentDOW-1].asArray()[0];
		endHour = sessionData["times"].asArray()[currentDOW-1].asArray()[1];
		if (currentHour >= startHour && currentHour < endHour){
			RelayToggle(currentReceptical, true);
		}else{
			RelayToggle(currentReceptical, false);
		}
	}
}

//Helpers
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
		case 11:
			digitalWrite(RELAY11, LOW);
			break;
		case 12:
			digitalWrite(RELAY12, LOW);
			break;
		case 13:
			digitalWrite(RELAY13, LOW);
			break;
		case 14:
			digitalWrite(RELAY14, LOW);
			break;
		case 15:
			digitalWrite(RELAY15, LOW);
			break;
		case 16:
			digitalWrite(RELAY16, LOW);
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
		case 11:
			digitalWrite(RELAY11, HIGH);
			break;
		case 12:
			digitalWrite(RELAY12, HIGH);
			break;
		case 13:
			digitalWrite(RELAY13, HIGH);
			break;
		case 14:
			digitalWrite(RELAY14, HIGH);
			break;
		case 15:
			digitalWrite(RELAY15, HIGH);
			break;
		case 16:
			digitalWrite(RELAY16, HIGH);
			break;
		}
	}
}

void pumpSpin(int setAmount, int pumpNumber, int pumpFlowRate = 100){
	RelayToggle(pumpNumber, true); //turn pump gate power on
	//int setAmount, int setCalibration, int pumpSize, int pumpNumber
	int mlPerSec = pumpFlowRate / 60; //100m (per minute) / 60sec = 1.6ml per seconds
	int pumpLength = setAmount / mlPerSec; //25ml target / 1.6ml per seconds = 15.625 seconds

	for (int i = 0; i < pumpLength; i++){
		delay(1000);
		//printHomeScreen();
		Serial.flush();
	}
	RelayToggle(pumpNumber, false); //turn pump gate power on
}

void makeNewFile(String path, JsonObject& data){
	char buffer[1024];
	tmpFile = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}