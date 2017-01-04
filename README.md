# DRO<img src="https://raw.githubusercontent.com/devinrayolsen/DRO-Matic/master/Docs/images/hydro-hyphen.png" width="64">Matic OS

This is an Arduino (MicroController) based hydroponics Operating System built for any scale hydroponic farm that wants to fully automate nutrient and PH dosing. 
The DRO-Matic (automatic hydroponics) has been built to allow users to create, configure and share projects called "crops" by interfacing with an external MicroSD card. 
A single DRO-Matic crop disk size (SD card) can very, but in very extreme crop configurations (10 channels, 100 weeks of dosing sessions per channel) were found to be just under 5MB.

##Why?
DROMatic has been built to allow you be flexible in any way which you would like to configure the dosings of your hydroponic solutions. 
However, before we get into the settings and configuration of the OS, its important to ask our selfs why this even exists? 
It's undoubtedly true across almost all cultures that agriculture and food growth is not only a necessity but also expensive. 
Agriculture is expensive in the amount of land used, energy spent to get to harvest, and even time spent consistency with successful crops year after year.

In steps hydroponics, and vertical farming. We now have the technology to begin moving farms into a hydroponic setup that stacks vertically. 
This means no more time spent in large scale soil maintenance, crops can be summed up into much smaller spaces and thus operations can get bigger (concentrated spaces if you will), and crop consistency is much easier to achieve.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=BwgXb9h-Qgs
" target="_blank"><img src="http://img.youtube.com/vi/BwgXb9h-Qgs/0.jpg" 
alt="" width="900" height="620" border="10" /></a>

However, the big down side to moving to vertical farming or hydroponics in general means you do have to learn a lot about nutrients, dosing regiments, regiment drifting, EC/PPM and PH testing etc etc.

In steps the DRO-Matic OS to take care of all the heavy lifting in hydroponic farming for you. Our goal is curve the time spent learning and maintaining hydroponic farms in hopes to better bring this much more efficient way of food production to a lot more farmers.

##OS Overview
The DROMatic OS has the ability to be a basic scheduled doser or even allow it to maintain pH and PPM levels autonomously and responsively. DROMatic OS comes with a entire configurable but optional water drainage/top off system allowing you to schedule plant water draining / refilling or topping off of dosed reservoir water.
Because of the sheer flexibility of the DRO-Matic OS, there is a lot of a screens to configure a single crop. Below is a quick overview of the available OS features, but for a more in-depth look please <a href="https://github.com/devinrayolsen/DRO-Matic/wiki/1)-Getting-Started">visit the wiki page</a>.

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/1)-Getting-Started">Home Screen</a>
- Displays current date / time, current PPM levels and current PH levels

### <a href="https://github.com/devinrayolsen/DROmatic/wiki/2)-System-Settings">System Settings</a>
- **Date/Time Configuration** = Allows you to set the date and time of the DROMatic OS
- **Number of Channels** = Allows you to set how many total solutions this crop will be dosed with (max is 10)
- **EC/PPM Range Configuration** = Sets the desired range of EC/PPM you would like your DROMatic to maintain for this crop
- **EC/PPM Respond or Fixed** - Set system to responsively adjust dosing amounts or not, based on recored EC/PPM levels of dosed / run off water. 
-- **EC/PPM Range** = If EC/PPM is set to responsive, system wants to know desired EC/PPM ranges it may respond under.
- **pH Configuration** = Sets the desired range of pH you would like your DROMatic to maintain for this crop and what channels are pH up and down
-- **pH Auto or Scheduled** - Set system to reserve two channels for real-time pH reading / dosing, or allow channels to be scheduled with fixed dose sizes / times. 
-- **pH Range** = If system is set to use Autonomous real-time dosing, system will want to know desired pH ranges. 
-- **pH Channels** = If system is set to Autonomous real-time pH dosing, system will want to know what channels are pH up and pH down. You are not allowed to set 1 channel to be both up and down pH as system required separate up / down solutions.
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
- **Session Start Date/Time** = What time / day should this dosing session should start
- **Session Repeat Configuration** = If/How/When this dosing session should be repeated. This is good for when you want to repeat 1 dosing session X times vs. hand configuring the same session over and over again. (Optional)
