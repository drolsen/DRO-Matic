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
			screenName = F("");
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
	if (((millis() - phPlantMillis) < (phDelay * 60000)) || ((millis() - ecMillis) < (topOffDelay * 60000))) { return; } //has we waited long enough since eiher last pH adjustment or EC adjustment?
	
	float plantpH = getPHProbeValue(PLANTPH);
	float rsvrpH = getPHProbeValue(RSVRPH);
	
	if (rsvrpH > maxPH || rsvrpH < minPH) { return; } //sorry, no EC correction while reservoir pH is out of range
	if (plantpH > maxPH || plantpH < minPH) { return; } //sorry, no EC correction while plants pH is out of range
	
	int EC = getECProbeValue(PLANTEC); 

	if ((EC > maxPPM || EC < minPPM)){
		StaticJsonBuffer<cropBufferSize> cropBuffer;
		JsonObject& cropData = getCropData(cropBuffer);

		lcd.clear();
		lcd.print(F("TOPPING OFF EC"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEASE HOLD!!!"));
		flowOutRate = flowInRate = pulseInFlowCount = 0;

		float secondsPerGallon = (60 / flowMeterConfig[1]) * 3.78; //60sec / meter's liters per min * 3.78 lpg = 41 seconds per gallon
		int feedingSeconds = secondsPerGallon * topOffAmount; //41 seconds * topOff gallons
		int drainingSeconds = secondsPerGallon * topOffAmount; //41 seconds * topOff gallons

		//Drain configured gallons first by drainingSeconds
		while (drainingSeconds--){
			RelayToggle(12, false); //out
			RelayToggle(11, true); //in
			lcd.clear();
			if (drainingSeconds < 10){
				lcd.print(F("00"));
			}
			else if (drainingSeconds < 100){
				lcd.print(F("0"));
			}
			lcd.print(drainingSeconds);
			lcd.print(" SECONDS IN");
			lcd.setCursor(0, 1);
			lcd.print("DRAINING REMAIN");
			delay(1000);
		}

		//Feed configured gallons next by feedingSeconds
		while (feedingSeconds--){
			RelayToggle(12, true); //out
			RelayToggle(11, false); //in
			checkFlowRates();
			lcd.clear();
			if (feedingSeconds < 10){
				lcd.print(F("00"));
			}
			else if (feedingSeconds < 100){
				lcd.print(F("0"));
			}
			lcd.print(feedingSeconds);
			lcd.print(" SECONDS IN");
			lcd.setCursor(0,1);
			lcd.print("FEEDING REMAIN");
			//have we run out of topoff water?
			if (flowInRate > 0.05 && feedingType == 2){ //Moving into next regimen
				lcd.clear();
				lcd.print("MOVING ONTO");
				lcd.setCursor(0, 1);
				lcd.print("NEXT REGIMEN");
				currentRegimen++;
				currentRegimen = (currentRegimen > maxRegimens) ? maxRegimens : currentRegimen;
				StaticJsonBuffer<cropBufferSize> cropBuffer;
				JsonObject& cropData = getCropData(cropBuffer);
				cropData["currentReg"] = currentRegimen;
				cropData["feedType"] = feedingType = 0;

				//are we loading new regimen ranges, or continuing last ones?
				if (currentRegimen != maxRegimens){
					//load EC Conductivity ranges
					StaticJsonBuffer<ecBufferSize> ecBuffer;
					JsonObject& ECData = getECData(ecBuffer, currentRegimen);
					minPPM = ECData["ec"].asArray()[0];
					maxPPM = ECData["ec"].asArray()[1];
				}
				setCropData(cropData);
				break;
			}
			delay(1000);
		}
		RelayToggle(11, false);
		RelayToggle(12, false);
		ecMillis = millis(); //reset EC millis
	}
}

//is it time to fix ph dift of plant water?
void correctPlantPH(){
	if (flowOutRate > 0.01) { return; } //no pH corrections while feeding or topoff is taking place
	if (feedingType == 0) { return; } //we don't want to correct plant pH while under full feeding type
	if (((millis() - phPlantMillis) < (phDelay * 60000))) { return; } // have we waited long enough since our last pH correction?
	float pH = getPHProbeValue(PLANTPH);

	//is current ph is outside of configred ph ranges?
	if (pH > maxPH || pH < minPH){
		lcd.clear();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		if (pH > maxPH){			//we must micro-ph-dose our plant water DOWN
			pumpSpin(phAmount, 10);
		}
		else if (pH < minPH){	//we must micro-ph-dose our plant water UP
			pumpSpin(phAmount, 9);
		}
		phPlantMillis = millis();
	}
}

//is it time fix reservoir pH drift?
void correctRsvrPH(){
	//are we permitted to correct reservoir pH?
	if ((flowInRate > 0.01 || flowOutRate > 0.01) || ((millis() - phRsvrMillis) < (phDelay * 60000))) { return; }
	float pH = getPHProbeValue(RSVRPH);
	if (pH > maxPH || pH < minPH){ //if current ph is outside of configred ph ranges
		lcd.clear();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLEAES HOLD!!!"));
		pumpSpin(phAmount, 8);
		phRsvrMillis = millis();
	}
}

//Open channel of tentical sheild to get probe reading value
void openWaterProbeChannel(byte channel) {
	Wire.beginTransmission(channel_ids[channel]);     // call the circuit by its ID number.
}

//Get either EC or pH live probe values
float getPHProbeValue(byte channel){
	openWaterProbeChannel(channel);     // open EC1 tentical shield channel
	Wire.write('r');                    // request a reading by sending 'r'
	Wire.endTransmission();             // end the I2C data transmission.
	delay(1000);						// AS circuits need a 1 second before the reading is ready
	
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

void pumpSpin(float setAmount, int pumpNumber){
	//Cast a float to avoid implicit int rounding
	float mlPerSec = ((float)pumpCalibration) / 60; //100ml per min / 60 seconds = 1.6ml per second
	//Now it is ok to rounding to whole number
	int pumpTimeLength = (setAmount / mlPerSec); //amount / mlPerSec = total ml time in seconds
	while (pumpTimeLength--){
		RelayToggle(pumpNumber, true); //keep pump turning
		delay(1000);
	}
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