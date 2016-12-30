// Core.h

#ifndef _CORE_h
#define _CORE_h

#include "ArduinoJson\ArduinoJson.h" //Arduno Json (aka epic)
#include <SPI.h> //Suppoting lib for SD card
#include <SD.h> //SD card API
class CoreClass
{
 protected:
	File tmpFile;
	String cropName, screenName;
	int channelIndex;
	int sessionIndex;
	int(&loadedSessions)[10][12];

	void reloadCrop(){
		DynamicJsonBuffer jsonBuffer;
		JsonObject& crop = getCropData(jsonBuffer);
		JsonArray& cropFileSessions = crop["sessions"];
		byte cropSessionSize, i;

		cropSessionSize = cropFileSessions.size();
		for (i = 0; i < cropSessionSize; i++){
			JsonArray& session = cropFileSessions[i].asArray();
			loadedSessions[i][0] = session[0];		//year
			loadedSessions[i][1] = session[1];		//month
			loadedSessions[i][2] = session[2];		//day
			loadedSessions[i][3] = session[3];		//day of week
			loadedSessions[i][4] = session[4];		//hour
			loadedSessions[i][5] = session[5];		//min
			loadedSessions[i][6] = session[6];		//dose amount
			loadedSessions[i][7] = session[7];		//channel calibration
			loadedSessions[i][8] = session[8];		//channel size
			loadedSessions[i][9] = session[9];		//repeat number
			loadedSessions[i][10] = session[10];	//repeat by
			loadedSessions[i][11] = session[11];	//delay
			loadedSessions[i][12] = session[12];	//id
			loadedSessions[i][13] = session[13];	//out of
		}
	}

 public:
	 //Constructor
	 CoreClass(String NAME, String SCREEN, int CHANNELINDEX, int SESSIONINDEX, int(&SESSIONS)[10][12])
		 : loadedSessions(SESSIONS)
	 {
		 cropName = NAME;
		 screenName = SCREEN;
		 channelIndex = CHANNELINDEX;
		 sessionIndex = SESSIONINDEX;
	 }

	 //Init
	 void Init(){
		 if (SD.exists("dromatic")){ //has OS already been setup?
			 DynamicJsonBuffer coreBuffer;
			 JsonObject& coreData = getCoreData(coreBuffer);

			 cropName = coreData["crop"].asString();
			 if (cropName != "" && SD.exists("dromatic/" + cropName)){ //Loading up exisiting core file's crop directory
				 screenName = "";
				 loadCrop();
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
		 tmpFile = SD.open("dromatic/" + cropName + "/crop.dro");
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
		 reloadCrop(); //reload crop sessions into memory
	 }

	 JsonObject& getChannelData(JsonBuffer& b){
		 tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", FILE_READ);
		 JsonObject& d = b.parseObject(tmpFile.readString());
		 tmpFile.close();
		 return d;
	 }

	 void setChannelData(JsonObject& d){
		 char b[256];
		 tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/channel.dro", O_WRITE | O_TRUNC);
		 d.printTo(b, sizeof(b));
		 tmpFile.print(b);
		 tmpFile.close();
		 reloadCrop(); //reload crop sessions into memory
	 }

	 JsonObject& getSessionData(JsonBuffer& b){
		 tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/sessions/chses" + sessionIndex + "/session.dro", FILE_READ);
		 JsonObject& d = b.parseObject(tmpFile.readString());
		 tmpFile.close();
		 return d;
	 }

	 void setSessionData(JsonObject& d){
		 char b[256];
		 tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + channelIndex + "/sessions/chses" + sessionIndex + "/session.dro", O_WRITE | O_TRUNC);
		 d.printTo(b, sizeof(b));
		 tmpFile.print(b);
		 tmpFile.close();
		 reloadCrop(); //reload crop sessions into memory
	 }
};

extern CoreClass Core;

#endif

