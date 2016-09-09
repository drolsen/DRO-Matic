# DROMatic

This is a Arduino based Operating System built for Hydroponic farmers looking to automate both the Nutrient and PH dosing portion of their crops.

The OS minimum hardware requirments are as follows:
- <a href="https://www.amazon.com/gp/product/B016D5L7NY/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1">Arduino Mega 2650 R3 <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/517bETdtT4L.jpg" width="128"></a>

- <a href="https://www.amazon.com/gp/product/B01C466H1S/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1">LCD 16x2 DFRobot Keypad Shield <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/61XwiY78d3L._SX522_.jpg" width="128"> </a>

- <a href="https://www.amazon.com/gp/product/B00SL0QWDU/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1">DS3231 RTC (real time clock) Chip <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/61GHli1sjJL._SX522_.jpg" width="128"> </a>

- <a href="https://www.amazon.com/gp/product/B00HCB7VYS/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1">Arduino Micro SD Card Adapter <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/61oB9UmTXZL._SX522_.jpg" width="128"></a> 

The DROMatic OS was built to communicate directly with 10 stepper motors or pumps, allowing you to automate / configure up to 10 different hydroponic solutions and save them as "Crops" or individual projects on a SD card. The doser hardware is not something found commercially for this OS and must be built by hand. Here is a basic parts list for the actual doser hardware:

- 16 Channel Relay Switch
- 10 BigEasy step driver chips
- 10 Stepper motors or Dosing pumps
- Breadboards and Jump Cables

Please see https://www.youtube.com/watch?v=vrFCVSegT5M a demonstration on one example of a doser unit with this OS. Please note that in the video, we use syringes with stepper motors setup like an actuator, but you could easliy use Peristaltic pump instead, just pleaes be aware Peristaltic pumps can be very expensive and you will need 10 of them.

