#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include "SCD30.cpp"

void saveToFile(float saveValue, std::string filename){
	// std::cout << filename << std::endl;
	if (isnan(saveValue)){ return; }
	// open file to save value
	std::fstream saveFile;
	saveFile.open(filename,std::ios::out);
	if(saveFile.is_open()){
		saveFile << saveValue;  // save value
		saveFile.close();  // close file
	}
}

//////////
// main //
//////////
int main(int argc, char *argv[])
{
	// init
	SCD30 airSensor;
	float value[3];
	// read
	airSensor.readMeasurements(value);
	// save
	std::string filePath = "/var/lib/reef-pi/SCD30/";
	saveToFile(value[0],filePath+"co2.dat");
	saveToFile(value[1],filePath+"temp.dat");
	saveToFile(value[2],filePath+"hum.dat");
}