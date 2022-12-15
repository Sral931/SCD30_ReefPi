# SCD30_ReefPi
Full Reef-Pi interface to Sensirion SCD30 air quality sensor. Provides SCD30 base class for communication, command line tool for settings and control, a service for regular data acquisition and an install script.

## Installation

 - in SSH command console type:
 - sudo wget -c https://github.com/Sral931/SCD30_ReefPi/releases/download/1.1/SCD30_ReefPi_1.1.zip
 - sudo unzip SCD30_ReefPi_1.1.zip
 - sudo bash install_SCD30.sh
 
 - the install script will copy all files, start the necessary service and clean up the install files afterwards
 
 - In Reef-Pi go to Configuration->Drivers
 - create 3 file drivers in Reef-Pi:
	- Name appropriately (e.g. "SCD30_CO2", "SCD30_temp" and "SCD30_hum")
	- Type: "file-analog"
	- Paths (for each filedriver respectively):
		- "/var/lib/reef-pi/SCD30/co2.dat"
		- "/var/lib/reef-pi/SCD30/temp.dat"
		- "/var/lib/reef-pi/SCD30/hum.dat"
		
 - go to Configuration->Admin and reload
 - go to Configuration->Connectors
	- create 3 Analog-Inputs:
		- Name appropriately (e.g. "SCD30_CO2", "SCD30_temp" and "SCD30_hum")
		- Driver: the 3 file-drivers you have created before
		- Pin: 0
 - go to Configuration->Admin and reload
 - go to pH
 - add 3 "pH" values using the analog-inputs you have just created
 
 ## Operation and Settings
 
  - to access the command line tool type into an SSH command line tool (I use PuTTY):
  - cd /var/lib/reef-pi/SCD30
  - ./SCD30Control -help
  - you will see a listing of all available commands
  - Examples:
  	- print current measurements:
  		- ./SCD30Control -meas
	- set altitude to 75m (will turn off exact pressure offset using ./SCD30Control -start 1000)
  		- ./SCD30Control -altitude 75
	- set temperature offset to 2.8 °C
		- ./SCD30Control -tempoff 280

 ## Notes
 
 - temperature offset:
 	- as you will likely put the board into an enclosure this enclosure will warm up and have a higher temperature than the surrounding air
 	- to compensate, wait half an hour to let the temperature settle on equilibrium
	- then lok at the measurement value (either in Reef-Pi or using ./SCD30Control -meas)
	- calculate how much the temperature is above the actual room temperature (in my case 2.8°C)
	- set this by using: ./SCD30Control 280 (single digits represent 0.01°C, e.g. 100 is 1.0°C)
 - future development:
 	- create a similar interface for a BMP280 air pressure sensor and feed this information to the SCD30 sensor as well
