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