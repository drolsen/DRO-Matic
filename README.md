# DRO<img src="https://raw.githubusercontent.com/devinrayolsen/DRO-Matic/master/Docs/images/hydro-hyphen.png" width="64">Matic OS

DRO-Matic OS automates nutrient dosing, pH & EC drift fixing, feedings, drainings, topoffs and timers for hydroponics. The OS has been built to scale of nearly any size hydroponics farm.
The OS allow users to create, configure and share what are called "crops" by interfacing with a LCD screen and external MicroSD card. 
A single DRO-Matic crop disk size (SD card) can very, but in very extreme crop configurations (10 channels, 100 weeks of dosing sessions per channel) were found to be just under 5MB.

## Why?
With a DRO-Matic you are allowed to be flexible in any which way you would like to configure your hydroponic solutions. 
However, before we get into the configuration of the OS, it's important to ask ourselves why this even exists? 
It's undoubtedly true across all cultures that agriculture and food growth is not only a necessity but also very expensive. 
Agriculture is expensive in the amount of land used, time used to harvest, and energy spent to obtaining consistant crops year after year.

We now have the technology to move large soil based farms into a vertical hydroponic setup to save land, time and money in mass food production. This means no more time spent at large scale soil maintenance, farms can summed up into much smaller but scaleable spaces, and crop consistency is much easier to achieve all year.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=BwgXb9h-Qgs
" target="_blank"><img src="http://img.youtube.com/vi/BwgXb9h-Qgs/0.jpg" 
alt="" width="900" height="620" border="10" /></a>

However the big down side to moving to vertical farming (or hydroponics in general) means you do have to learn a lot about nutrients, dosing regimens, regimen drifting, EC/PPM and PH testing etc etc. Because of this rather large learning curve, we don't see a lot of traditional farmer make the jump to this more cost effective solution because of this sheer learning curve. We need a way to simply the entire process of hydropinic farming for everyone and still be scaleable...

In steps the DRO-Matic OS to take care of all the heavy lifting our of your hydroponic farming needs. The goal is curve all time spent learning and maintaining hydroponic farms by automating nearly every aspect and ultimatly bring a much more efficient way of food production option to the world.

Please note that this OS is only the OS part of this solution; without the actual DRO-Matic dosing cabinet (not commercially available, must DIY build), this OS makes no sense for you yet. For detailed instructions on how to DIY build your own DRO-Matic cabinet, please visit our <a href="https://github.com/drolsen/DRO-Matic/wiki/6)-Hardware-Requirements">hardware requirements page videos.</a>

## OS Overview
Th DRO-Matic OS has the ability to fully automate the follow process for you:
- Regimen dosing (full feedings and top off feedings with drifting amounts)
- pH adjusting (both plant and reservoir water sources)
- eC adjusting (plant water source topoffs)
- Irrigation (feeding, draining and topping off)
- Timers (4 120v/10amp power recepticals that can be timed)

Because of the sheer flexibility of the DRO-Matic OS, there is a lot of a screens to configure a single crop. 
Below is a overview of the available OS features.

### <a href="https://github.com/drolsen/DRO-Matic/wiki">Home Screen</a>
- Displays current date / time, and plant water's current PPM/EC levels and current pH level

### <a href="https://github.com/drolsen/DRO-Matic/wiki/1)-System-Settings">System Settings</a>
- **Date/Time** = Allows you to set the date and time of the DROMatic OS
- **EC Calibration** = 3 point calibration (Dry, Low, High) for both EC probes.
- **pH Calibration** = 3 point calibration (Low, Mid, High) for both pH probes.
- **Pump Calibration** = Calibrate how many milliliters per minute your perstaltics can pumps.
- **Pump Delay** = Calibrate how long of a delay each dosing pump should take between dosing each solution (allows time to fully mix each solution).

### <a href="https://github.com/drolsen/DRO-Matic/wiki/2)-Crop-Settings">Crop Settings</a>
- **New Crop** = Start a brand new crop project on your SD card
- **Load Crop** = Load an existing crop project on your SD card
- **Delete Crop** = Delete the available listed crops on SD card (except for currently loaded crop)
- **Reset Crop** = ResetS crop back to first configured regimen and timer day/week. Does NOT reset crop back to factory settings!!
- **EC Range** = Sets the desired range of EC you would like your DRO-Matic to maintain for this crop for a given regimen. 
- **pH Range** = Sets the desired range of pH you would like your DRO-Matic to maintain for this crop for a given regimen.
- **Number of Regimens** = Sets desired number of regimen doses you exspect your plant's life cycle to take.

### <a href="https://github.com/drolsen/DRO-Matic/wiki/3)-Pump-Settings">Pump Settings (after choosing a pump)</a>
- **Prime** = Primes a pump line to remove any excess air during first setup.
- **Amount** = Configure how many milliliters a pump will dose for a given regimen. No need to calculate reservoir volumne, OS will do this already.
- **Delay** = Configure how long of a delay between each pump to prevent nutrient lockout.

### <a href="https://github.com/drolsen/DRO-Matic/wiki/4)-Irrigation-Settings">Irrigation Settings</a>
- **Reservoir Volume** = Configure the volume of water your reseroivr / plants will be using throughout the life of plants.
- **TopOff Concentrate** = Configure the fraction amount of dosing amount topoff water gets.
- **Drain Time** = Configure how long of a plant water drain time you will need. You can may want to over estimate this.
- **Flow Calibration** = Configure pulse count per second IN and OUT flow meters are manufactured to send. These numbers are different for per size/manufacturers of flow meters.
- **Manual Flush** = Utility that allows you to flow either IN and OUT irrigation valvels to do any manual flushing during setup / testing.


### <a href="https://github.com/drolsen/DRO-Matic/wiki/5)-Timer-Settings">Timer Settings (after choosing a timer)</a>
- **Start End Times** = Configure start and end times over a given day of the week, for an infinate amonut of configurable weeks.
