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

//time feed plants some top off water?
void correctPlantEC(){
	if (flowInRate > 0.025){ return; } //we are not allowed to topoff plant water if rsvr is filling up flowInRate
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

		//Drain plant water by time
		drainPlants(0, 10);

		//Feed plants by time
		feedPlants(0, 10);

		//Preventive measures must be taken
		RelayToggle(11, false);
		RelayToggle(12, false);

		//Proventative measures
		resetTimestamps();
	}
}

//is it time to fix ph dift of plant water?
void correctPlantPH(){
	if (flowOutRate > 0.025) { return; } //no pH corrections while feeding or topoff is taking place
	if (feedingType == 0) { return; } //we don't want to correct plant pH while under full feeding type
	if (((millis() - phPlantMillis) < (phDelay * 60000))) { return; } // have we waited long enough since our last pH correction?
	float pH = getPHProbeValue(PLANTPH);

	//is current ph is outside of configred ph ranges?
	if (pH > maxPH || pH < minPH){
		lcd.clear();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("PLANT PH:"));
		lcd.print(pH);
		if (pH > maxPH){			//we must micro-ph-dose our plant water DOWN
			pumpSpin(phAmount, 10);
		}
		else if (pH < minPH){	//we must micro-ph-dose our plant water UP
			pumpSpin(phAmount, 9);
		}
		//Proventative measures
		resetTimestamps();
	}

}

//is it time fix reservoir pH drift?
void correctRsvrPH(){
	//can't proceed if we have a flow rate in either direction
	if ((flowInRate > 0.025 || flowOutRate > 0.025)) { return; }
	//can't proceed if we have not waited long enough since last time we pH corrected
	if (((millis() - phRsvrMillis) < (phDelay * 60000))) { return; }

	float pH = getPHProbeValue(RSVRPH);
	if (pH > maxPH || pH < minPH){ //if current ph is outside of configred ph ranges
		lcd.clear();
		lcd.print(F("PH DRIFT FIXING"));
		lcd.setCursor(0, 1);
		lcd.print(F("RSVR PH:"));
		lcd.print(pH);
		pumpSpin(phAmount, 8);
		//Proventative measures
		resetTimestamps();
	}
}

//Get either pH probe values
float getPHProbeValue(byte channel){
	Wire.beginTransmission(channel_ids[channel]);			// open EC1 tentical shield channel
	Wire.write('r');										// request a reading by sending 'r'
	Wire.endTransmission();									// end the I2C data transmission.
	delay(1000);											// AS circuits need a 1 second before the reading is ready
	
	sensor_bytes_received = 0;								// reset data counter
	memset(sensordata, 0, sizeof(sensordata));				// clear sensordata array;

	Wire.requestFrom(channel_ids[channel], 32, 1);			// call the circuit and request 48 bytes (this is more then we need).
	code = Wire.read();
	while (Wire.available()) {								// are there bytes to receive?
		in_char = Wire.read();								// receive a byte.
		if (in_char == 0) {									// null character indicates end of command
			break;											// exit the while loop, we're done here
		}
		else {
			sensordata[sensor_bytes_received] = in_char;	// append this byte to the sensor data array.
			sensor_bytes_received++;
		}
	}
	String returnedValue;
	for (byte i = 0; i < 4; i++){
		returnedValue += sensordata[i];
	}
	return returnedValue.toFloat();
}

//Get either EC probe values
int getECProbeValue(byte channel){					
	Wire.beginTransmission(channel_ids[channel]);			// open a EC channel
	Wire.write('r');										// request a reading by sending 'r'
	Wire.endTransmission();									// end the I2C data transmission.
	delay(1000);											// AS circuits need a 1 second before the reading is ready

	sensor_bytes_received = 0;								// reset data counter
	memset(sensordata, 0, sizeof(sensordata));				// clear sensordata array;

	Wire.requestFrom(channel_ids[channel], 32, 1);			// call the circuit and request 48 bytes (this is more then we need).
	code = Wire.read();
	while (Wire.available()) {								// are there bytes to receive?
		in_char = Wire.read();								// receive a byte.
		if (in_char == 0) {									// null character indicates end of command
			break;											// exit the while loop, we're done here
		}
		else {
			sensordata[sensor_bytes_received] = in_char;	// append this byte to the sensor data array.
			sensor_bytes_received++;
		}
	}
	String returnedValue;
	for (byte i = 0; i < 4; i++){
		returnedValue += sensordata[i];
	}
	return returnedValue.toInt();
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
void makeNewFile(String path, JsonObject& data){
	char buffer[1024];
	tmpFile = SD.open(path, FILE_WRITE);
	data.printTo(buffer, sizeof(buffer));
	tmpFile.print(buffer);
	tmpFile.close();
	Serial.flush();
}
void resetTimestamps(){
	ecMillis = phRsvrMillis = phPlantMillis = flowMillis = millis();
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