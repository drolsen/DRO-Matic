# DROMatic
# H1 DROMatic
The Arduino based Operating System built for hydroponic farmers who want to fully automate both nutrient and PH dosing. 
The DROMatic has been built to allow users to create, configure and share what are called "crops" by building crops onto an external MicroSD card. 
A single DROMatic crop disk size (SD card) can very, but in extreme instances large crop configurations were found to be under 3MB.


#H2 The OS minimum hardware requirments are as follows:
- <a href="https://www.amazon.com/gp/product/B016D5L7NY/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1">Arduino Mega 2650 R3 <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/517bETdtT4L.jpg" width="128"></a>

- <a href="https://www.amazon.com/gp/product/B01C466H1S/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1">LCD 16x2 DFRobot Keypad Shield <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/61XwiY78d3L._SX522_.jpg" width="128"> </a>

- <a href="https://www.amazon.com/gp/product/B00SL0QWDU/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1">DS3231 RTC (real time clock) Chip <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/61GHli1sjJL._SX522_.jpg" width="128"> </a>

- <a href="https://www.amazon.com/gp/product/B00HCB7VYS/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1">Arduino Micro SD Card Adapter <br>
<img src="https://images-na.ssl-images-amazon.com/images/I/61oB9UmTXZL._SX522_.jpg" width="128"></a> 

DROMatic has been built to allow you to not be limited in any way on how you would like to configure the dosings of your hydroponic solutions. 
Thus, there is a lot of a screens to this little OS:

#H3 Home Screen
- Displays current date/time, current PPM levels and current PH levels

#H3 System Settings
- **Date/Time Configuration** = Allows you to set the date and time of the DROMatic OS
- **Number of Channels** = Allows you to set how many total solutions this crop will be dosed with (max is 10)
- **EC/PPM Range Configuration** = Sets the desired range of EC/PPM you would like your doseing to maintain for this crop
- **Load Crop** = Load an existing crop project on your SD card
- **New Crop** = Start a brand new crop project on your SD card
- **Delete Crop** = Delete the available listed crops on SD card (except for currently loaded crop)

#H3 System Channels
- Lists all the available crop's channels (aka dosing solutions)

#H3 Channel Settings
- **Number of Sessions** = Set the number of dosing sessions you would like this crop channel to perform
- **Size of Channel** = Set the total size of this channel in ml
- **Channel Calibration** = Calibrate how many rotations a motor or pump takes to reach channels total size

#H3 Channel Sessions
- Lists all the available channel's dosing sessions configured under channel settings

#H3 Session Settings
- **Session Amount** = How much of this channels solution should this dosing session dose
- **Session Start Date/Time** = What time/day should this dosing session start
- **Session Delay Configuration** = How many seconds would you like to delay the dosing start time (optional and 999sec max)
- **Session Repeat Configuration** = If/How/When this dosing session should be repeated. This is good for when you want to repeat 1 dosing session X times vs hand configuring the same session over and over again. (Optional)

The DROMatic OS was built to communicate directly with 10 stepper motors or pumps, allowing you to automate / configure up to 10 different hydroponic solutions and save them as "Crops" or individual projects on a SD card. The doser hardware is not something found commercially for this OS and must be built by hand. Here is a basic parts list for the actual doser hardware:

- 16 Channel Relay Switch
- 10 BigEasy step driver chips
- 10 Stepper motors or Dosing pumps
- Breadboards and Jump Cables

Please see https://www.youtube.com/watch?v=vrFCVSegT5M a demonstration on one example of a doser unit with this OS. Please note that in the video, we use syringes with stepper motors setup like an actuator, but you could easily use Peristaltic pump instead, just please be aware Peristaltic pumps can be very expensive and you will need 10 of them.