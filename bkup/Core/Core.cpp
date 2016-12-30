#include "Core.h"
//Constructor
CoreClass::CoreClass(String NAME, int CHANNELINDEX, int SESSIONINDEX, int(&SESSIONS)[10][12])
: loadedSessions(SESSIONS)
{
	cropName = NAME;
	channelIndex = CHANNELINDEX;
	sessionIndex = SESSIONINDEX;
}

void CoreClass::Init(){

}

//Get and Sets
JsonObject& CoreClass::getCoreData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/core.dro");
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void CoreClass::setCoreData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/core.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
}

JsonObject& CoreClass::getCropData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/crop.dro");
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void CoreClass::setCropData(JsonObject& d){
	char b[512];
	tmpFile = SD.open("dromatic/" + cropName + "/crop.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	reloadCrop(cropName); //reload crop sessions into memory
}

JsonObject& CoreClass::getChannelData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + currentChannelIndex + "/channel.dro", FILE_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void CoreClass::setChannelData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + currentChannelIndex + "/channel.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	reloadCrop(); //reload crop sessions into memory
}

JsonObject& CoreClass::getSessionData(JsonBuffer& b){
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + currentChannelIndex + "/sessions/chses" + currentSessionIndex + "/session.dro", FILE_READ);
	JsonObject& d = b.parseObject(tmpFile.readString());
	tmpFile.close();
	return d;
}

void CoreClass::setSessionData(JsonObject& d){
	char b[256];
	tmpFile = SD.open("dromatic/" + cropName + "/channels/sysch" + currentChannelIndex + "/sessions/chses" + currentSessionIndex + "/session.dro", O_WRITE | O_TRUNC);
	d.printTo(b, sizeof(b));
	tmpFile.print(b);
	tmpFile.close();
	reloadCrop(); //reload crop sessions into memory
}

CoreClass Core;