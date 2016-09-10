// DS3231_UnixTime
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS3231-library to 
// convert date and time to UnixTime
//
// To use the hardware I2C (TWI) interface of the chipKit you must connect
// the pins as follows:
//
// chipKit Uno32/uC32:
// ----------------------
// DS3231:  SDA pin   -> Analog 4
//          SCL pin   -> Analog 5
// *** Please note that JP6 and JP8 must be in the I2C position (closest to the analog pins)
//
// chipKit Max32:
// ----------------------
// DS3231:  SDA pin   -> Digital 20 (the pin labeled SDA)
//          SCL pin   -> Digital 21 (the pin labeled SCL)
//
// The chipKit boards does not have pull-up resistors on the hardware I2C interface
// so external pull-up resistors on the data and clock signals are required.
//
// You can connect the DS3231 to any available pin but if you use any
// other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access 
// to the pins used.
//

#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

Time t;

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // Initialize the rtc object
  rtc.begin();
}

void loop()
{
  // Send Current time
  Serial.print("Current Time.............................: ");
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr());

  // Send Unixtime
  // ** Note that there may be a one second difference between the current time **
  // ** and current unixtime show if the second changes between the two calls   **
  Serial.print("Current Unixtime.........................: ");
  Serial.println(rtc.getUnixTime(rtc.getTime()));
  
  // Send Unixtime for 00:00:00 on January 1th 2014
  Serial.print("Unixtime for 00:00:00 on January 1th 2014: ");
  t.hour = 0;
  t.min = 0;
  t.sec = 0;
  t.year = 2014;
  t.mon = 1;
  t.date = 1;
  Serial.println(rtc.getUnixTime(t));

  // Wait indefinitely
  while (1) {};
}
