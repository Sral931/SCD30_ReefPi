#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <vector>
#include "SCD30.cpp"

/////////////////////////////////////////////////////////////////////////

#define CMD_HELP "-help"

void printHelp(){
    std::cout << "SCD30 control program by Sral" << std::endl;
	std::cout << "\tCommands can be chained together in one line" << std::endl;
	std::cout << "\tExample: ./SCD30Control -addr 98 -meas" << std::endl;
	// Available Commands
    std::cout << "Available Commands:" << std::endl;
    std::cout << "\t-help" << "\t" << "print available commands" << std::endl;
	std::cout << "\t-version" << "\t" << "get software version" << std::endl;
	// System Commands
	std::cout << "  System Commands" << std::endl;
    std::cout << "\t-addr XX" << "\t" << "set current Address to decimal XX" << std::endl;
	std::cout << "\t-addr ?" << "\t" << "get current Address" << std::endl;
	std::cout << "\t-reset" << "\t" << "reset device" << std::endl;
	std::cout << "\t-fwversion" << "\t" << "get firmware version" << std::endl;
	// Setting Commands
	std::cout << "  Settings" << std::endl;
	std::cout << "\t-period XX" << "\t" << "set measurement period to XX (integer [seconds])" << std::endl;
	std::cout << "\t-period ?" << "\t" << "get measurement period" << std::endl;
	std::cout << "\t-autocal XX" << "\t" << "set automatic calibration (0/1)" << std::endl;
	std::cout << "\t-autocal ?" << "\t" << "get automatic calibration" << std::endl;
	std::cout << "\t-forcecal XX" << "\t" << "set forced calibration (integer of 400-2000 [ppm])" << std::endl;
	std::cout << "\t-forcecal ?" << "\t" << "get forced calibration value" << std::endl;
	// calibration notice
	std::cout << "   autocal deactivates forcecal and vice versa" << std::endl;
	std::cout << "\t-tempoff XX" << "\t" << "set temperature offset (integer 100 = 0.01°C)" << std::endl;
	std::cout << "\t-tempoff ?" << "\t" << "get temperature offset" << std::endl;
	std::cout << "\t-altitude XX" << "\t" << "set altitude correction (integer [meter])" << std::endl;
	std::cout << "\t-altitude ?" << "\t" << "get altitude correction" << std::endl;
	// altitude compensation notice
	std::cout << "   pressure compensation will deactivate altitude and vice versa" << std::endl;
	// Measurement Commands
	std::cout << "  Measurement Commands" << std::endl;
	std::cout << "\t-ready" << "\t" << "get Data Ready Status" << std::endl;
	std::cout << "\t-meas" << "\t" << "read measurement values" << std::endl;
	std::cout << "\t-start" << "\t" << "start continuous measurements (integer 0 [off] or 700-1400 [milli bar])" << std::endl;
	// pressure compensation notice
	std::cout << "   pressure compensation will deactivate altitude and vice versa" << std::endl;
	std::cout << "\t-stop" << "\t" << "stop continuous measurements" << std::endl;
}

bool checkInput (std::string arg){
	bool isInt = std::to_string(std::stoul(arg)) == arg;
	if (!isInt){
		std::cout <<"not an unsigned integer " << arg << std::endl;
		std::cout <<"------------------------" << std::endl;
	}
	return isInt;
}

/////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    int counter;
    if(argc==1){
        printHelp();
	}
    else{
        //initiate sensor class
        SCD30 airSensor;
        
        // first argument is the program name
        counter = 1;
		std::vector<std::string> args(argv, argv+argc);
        // cycle all arguments
        while (counter<argc){
			// Help Command
            if (args[counter] == "-help"){
				printHelp();
			}
			// System Commands
            else if (args[counter] == "-addr"){
				if (args[counter+1] != "?"){
					if ( !checkInput(args[counter+1])) break;
					airSensor.setAddress(std::stoul(args[counter+1]));
					counter++;
				}
				std::cout <<"current i2c address     " << std::to_string(airSensor.getAddress()) << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-reset"){
				airSensor.reset();
            }
			else if (args[counter] == "-version"){
				uint16_t version;
				version = airSensor.getSoftwareVersion();
				std::cout <<"software version        " << std::to_string(version >> 8) << "." << std::to_string(version && 0xFF) << std::endl;
				std::cout <<"------------------------" << std::endl;
            }
			else if (args[counter] == "-fwversion"){
				uint16_t version;
				version = airSensor.getFirmwareVersion();
				std::cout <<"firmware version        " << std::to_string(version >> 8) << "." << std::to_string(version && 0xFF) << std::endl;
				std::cout <<"------------------------" << std::endl;
            }
			// Setting Commands
			else if (args[counter] == "-period"){
				if (args[counter+1] != "?"){
					if ( !checkInput(args[counter+1])) break;
					airSensor.setMeasurementInterval(std::stoul(args[counter+1]));
					counter++;
				}
				std::cout <<"current meas. period    " << std::to_string(airSensor.getMeasurementInterval()) << " s" << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-autocal"){
				if (args[counter+1] != "?"){
					if ( !checkInput(args[counter+1])) break;
					airSensor.setAutoCalibration(std::stoul(args[counter+1]));
					counter++;
				}
				std::cout <<"auto calibration        " << std::to_string(airSensor.getAutoCalibration()) << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-forcecal"){
				if (args[counter+1] != "?"){
					if ( !checkInput(args[counter+1])) break;
					airSensor.setForcedCalibration(std::stoul(args[counter+1]));
					counter++;
				}
				std::cout <<"forced calibration to   " << std::to_string(airSensor.getForcedCalibration()) << " ppm" << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-tempoff"){
				if (args[counter+1] != "?"){
					if ( !checkInput(args[counter+1])) break;
					airSensor.setTemperatureOffset(std::stoul(args[counter+1]));
					counter++;
				}
				uint16_t tempoff = airSensor.getTemperatureOffset();
				std::cout <<"temperature offset      " << std::to_string(tempoff*0.01) << " °C" << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-altitude"){
				if (args[counter+1] != "?"){
					if ( !checkInput(args[counter+1])) break;
					airSensor.setAltitudeCorrection(std::stoul(args[counter+1]));
					counter++;
				}
				std::cout <<"altitude correction     " << std::to_string(airSensor.getAltitudeCorrection()) << " m" << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			// Measurement Commands
			else if (args[counter] == "-ready"){
				std::cout <<"data ready              " << std::to_string(airSensor.getDataReady()) << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-meas"){
				float value[3];
				airSensor.readMeasurements(value);
				std::cout <<"co2 concentration       " << std::to_string(value[0]) << " ppm" << std::endl;
				std::cout <<"temperature             " << std::to_string(value[1]) << " °C" << std::endl;
				std::cout <<"humidity                " << std::to_string(value[2]) << " %" << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-start"){
				if ( !checkInput(args[counter+1])) break;
				airSensor.setAltitudeCorrection(std::stoul(args[counter+1]));
				counter++;
				
				std::cout <<"altitude correction     " << std::to_string(airSensor.getAltitudeCorrection()) << " m" << std::endl;
				std::cout <<"------------------------" << std::endl;
			}
			else if (args[counter] == "-stop"){
				airSensor.stopMeasurement();
            }
			counter++;
        }
    }
    return 0;
}
