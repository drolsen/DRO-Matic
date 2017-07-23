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
	
	if (flowInRate > 0.01){ return; } //we are not allowed to topoff plant water if rsvr is filling up flowInRate
	if (feedingType != 2) { return; } //only after we have dosed our reservoir with topoff concentrates can we being to correct EC drift on plants
	if (((millis() - phPlantMillis) < phWaitPeriord) || ((millis() - ecMillis) < ecWaitPeriord)) { return; } //has we waited long enough since eiher last pH adjustment or EC adjustment?
	
	tmpFloats[0] = getPHProbeValue(1);
	delay(250); //too much power consumed if you don't delay between probe requests
	tmpFloats[1] = getPHProbeValue(3);
	delay(250); //too much power consumed if you don't delay between probe requests
	
	if (tmpFloats[1] > maxPH || tmpFloats[1] < minPH) { return; } //sorry, no EC correction while reservoir pH is out of range
	if (tmpFloats[0] > maxPH || tmpFloats[0] < minPH) { return; } //sorry, no EC correction while plants pH is out of range
	
	tmpInts[0] = getECProbeValue(0); 
	delay(250); //too much power consumed if you don't delay between probe requests

	if ((tmpInts[0] > maxPPM || tmpInts[0] < minPPM)){
		unsigned long topOffWait = millis(); //10 seconds
		StaticJsonBuffer<cropBufferSize> cropBuffer;
		JsonObject& cropData = getCropData(cropBuffer);

		lcd.clear();
		lcd.home();
		lcd.print(F("TOPPING OFF EC"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEASE HOLD!!!"));
		flowInRate = pulseInFlowCount = 0;
		RelayToggle(11, true); //open up in-valve
		while (flowInRate < 0.01){
			if ((millis() - topOffWait) > 5000){ //keeps in-valve open for 5 seconds, then close.
				RelayToggle(11, false);
			}
			if ((millis() - topOffWait) > 35000){ //after 20 seconds, open out-valve for 5 seconds, then close.
				RelayToggle(12, true);
				break;
			}
			if ((millis() - topOffWait) > 40000){ //after 5 seconds, close up out-valve and finish EC correction.
				RelayToggle(12, false);
				break;
			}
			if ((millis() - flowMillis) >= 1000){ //after 1 second, we checkFlowRates();, then reset flowMillis time stamp.
				checkFlowRates();
				flowMillis = millis();
			}
		}
		ecMillis = millis(); //reset EC millis
	}
	tmpInts[0] = tmpFloats[1] = tmpFloats[3] = 0;
}

//is it time to fix ph dift of plant water?
void correctPlantPH(){
	//are we permitted to correct plant pH?
	if (flowOutRate > 0.01 || ((millis() - phPlantMillis) < phWaitPeriord) || feedingType == 0) { return; }
	tmpFloats[0] = getPHProbeValue(1);

	//is current ph is outside of configred ph ranges?
	if (tmpFloats[0] > maxPH || tmpFloats[0] < minPH){
		lcd.clear();
		lcd.home();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		if (tmpFloats[0] > maxPH){			//we must micro-ph-dose our plant water DOWN
			pumpSpin(1, 10, pumpCalibration);
		}else if (tmpFloats[0] < minPH){	//we must micro-ph-dose our plant water UP
			pumpSpin(1, 9, pumpCalibration);
		}
		phPlantMillis = millis();
	}
	tmpFloats[0] = 0;
}

//is it time fix reservoir pH drift?
void correctRsvrPH(){
	//are we permitted to correct reservoir pH?
	if ((flowInRate > 0.01 || flowOutRate > 0.01) || ((millis() - phRsvrMillis) < phWaitPeriord)) { return; }
	tmpFloats[0] = getPHProbeValue(3);

	//if current ph is outside of configred ph ranges
	if (tmpFloats[0] > maxPH || tmpFloats[0] < minPH){
		lcd.clear();
		lcd.home();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		pumpSpin(1, 8, pumpCalibration);
		phRsvrMillis = millis();
	}else{
		//time to dose reservoir?
		checkRegimenDosing();
	}
	tmpFloats[0] = 0;
}

//Open channel of tentical sheild to get probe reading value
void openWaterProbeChannel(byte channel) {
	Wire.beginTransmission(channel_ids[channel]);     // call the circuit by its ID number.
}

//Get either EC or pH live probe values
float getPHProbeValue(byte channel){ //default is channel 0 aka EC1. 0 = EC1, 1 = PH1, 2 = EC2, 3 = PH2
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
	return returnedValue.toFloat();
}

int getECProbeValue(byte channel){ //default is channel 0 aka EC1. 0 = EC1, 1 = PH1, 2 = EC2, 3 = PH2
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
	return returnedValue.toInt();
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

void pumpSpin(float setAmount, int pumpNumber, int pumpFlowRate = 100){
	int pumpLength = (setAmount / (pumpFlowRate / 60)) * 1000; //1ml / 1.6ml per second * 1000ms per second = 625ms
	unsigned long pumpMillis = millis();
	do {
		RelayToggle(pumpNumber, true); //keep pump turning
	} while ((millis() - pumpMillis) < pumpLength);
	RelayToggle(pumpNumber, false); //turn pump off
}

void makeNewFile(String path, JsonObject& data){
	char buffer[1024];
	tmpFile = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}