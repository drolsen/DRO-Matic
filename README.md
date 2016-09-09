# DROMatic

This is a Arduino based Operating System built for Hydroponic farmers looking to automate both the Nutrient and PH dosing portion of their crops.

The OS minimum hardware requirments are as follows:
- Arduino Mega 2650 R3
![alt tag](https://images-na.ssl-images-amazon.com/images/I/517bETdtT4L.jpg) 

- LCD 16x2 DFRobot Keypad Shield 
![alt tag](https://images-na.ssl-images-amazon.com/images/I/61XwiY78d3L._SX522_.jpg) 

- DS3231 RTC Chip 
![alt tag](https://images-na.ssl-images-amazon.com/images/I/61GHli1sjJL._SX522_.jpg) 

- Arduino Micro SD Card Adapter 
![alt tag](https://images-na.ssl-images-amazon.com/images/I/61oB9UmTXZL._SX522_.jpg) 

The DROMatic OS was built to communicate directly with 10 stepper motors or pumps, allowing you to automate / configure up to 10 different hydroponic solutions and save them as "Crops" or individual projects on a SD card. The doser hardware is not something found commercially for this OS and must be built by hand. Here is a basic parts list for the actual doser hardware:

- 16 Channel Relay Switch
- 10 BigEasy step driver chips
- 10 Stepper motors or Dosing pumps
- Breadboards and Jump Cables

Please see https://www.youtube.com/watch?v=vrFCVSegT5M a demonstration on one example of a doser unit with this OS. Please note that in the video, we use syringes with stepper motors setup like an actuator, but you could easliy use Peristaltic pump instead, just pleaes be aware Peristaltic pumps can be very expensive and you will need 10 of them.

