# DRO-Matic

The Arduino based Operating System built for serious hydroponic farmers who want to fully automate both nutrient and PH dosing. 
The DROMatic has been built to allow users to create, configure and share what are called "crops" by building crops onto an external MicroSD card. 
A single DRO-Matic crop disk size (SD card) can very, but in extreme instances large crop configurations were found to be under 3MB.

DROMatic has been built to allow you be flexable in any way which you would like to configure the dosings of your hydroponic solutions. 
Because of the sheer flexability of this dosing OS, there is a lot of a screens to a single crop:

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/1)-Getting-Started">Home Screen</a>
- Displays current date/time, current PPM levels and current PH levels

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/2)-System-Settings">System Settings</a>
- **Date/Time Configuration** = Allows you to set the date and time of the DROMatic OS
- **Number of Channels** = Allows you to set how many total solutions this crop will be dosed with (max is 10)
- **EC/PPM Range Configuration** = Sets the desired range of EC/PPM you would like your DROMatic to maintain for this crop
- **Load Crop** = Load an existing crop project on your SD card
- **New Crop** = Start a brand new crop project on your SD card
- **Delete Crop** = Delete the available listed crops on SD card (except for currently loaded crop)

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/3)-System-Channels">System Channels</a>
- Lists all the available crop's channels (aka dosing solutions)

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/4)-Channel-Settings">Channel Settings</a>
- **Number of Sessions** = Set the number of dosing sessions you would like this crop channel to perform
- **Size of Channel** = Set the total size of this channel (in ml)
- **Channel Calibration** = Calibrate how many rotations a motor or pump takes to reach channels total size

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/5)-Channel-Sessions">Channel Sessions</a>
- Lists all the available channel's dosing sessions configured under channel settings

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/6)-Sessions">Session Settings</a>
- **Session Amount** = How much of hydroponic solution (in ml) should this session dose
- **Session Start Date/Time** = What time/day should this dosing session should start
- **Session Delay Configuration** = How many seconds would you like to delay the dosing start time (optional and 999sec max)
- **Session Repeat Configuration** = If/How/When this dosing session should be repeated. This is good for when you want to repeat 1 dosing session X times vs. hand configuring the same session over and over again. (Optional)
