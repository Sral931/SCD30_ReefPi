#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions

//Constants
#define SCD30_ADDRESS 0x61
#define SCD30_VERSION 0x0100

//Available commands
// system commands
#define CMD_SOFT_RESET 0xD304
#define CMD_GET_FW_LEVEL 0xD100
#define CMD_GET_SERIALNBR 0xD033
#define SCD30_SERIAL_NUM_WORDS 3
// The longer serial number is 16 words / 32 bytes (means 48 bytes with CRC).
// Most I2C buffers are by default 32. Hence the length is kept to the
// 3 words = first 6 (equal to what is printed on the case).
// The additional information is for Sensirion internal only.

// setting commands
#define CMD_SET_MEAS_INTERVAL 0x4600
#define CMD_SET_AUTO_SELF_CAL 0x5306
#define CMD_SET_FORCED_RECAL_FACTOR 0x5204
#define CMD_SET_TEMP_OFFSET 0x5403
#define CMD_SET_ALTITUDE_COMPENSATION 0x5102

// measurement commands
#define CMD_GET_DATA_READY 0x0202
#define CMD_GET_MEAS 0x0300
#define CMD_START_CONT_MEAS  0x0010 //can add pressure in mbar (uint16)
#define CMD_START_SINGLE_MEAS 0x0006 //supposedly not stable ?
#define CMD_STOP_MEAS 0x0104

typedef union {
    uint8_t array[4];
    float value;
} ByteToFl;

typedef union {
    uint8_t array[2];
    uint16_t value;
} ByteToIn;

class SCD30
{
	public:
		bool debug = false;
		
		SCD30(void){
			char *filename = (char*)"/dev/i2c-1";  // Define the i2c file path
			i2cAddress = SCD30_ADDRESS;
			probeI2Cfile = open(filename, O_RDWR);  // open file for read and write
		}
		
		// System commands
		void setAddress (uint8_t address){
			i2cAddress = address;
		}
		
		uint8_t getAddress (void){
			return i2cAddress;
		}
		
		void reset(void){
			if (openConnection() == 0) return;
			// send Stop Command
			sendCommand(CMD_SOFT_RESET);
		}
		
		uint16_t getSoftwareVersion(void){
			return SCD30_VERSION;
		}
		
		uint16_t getFirmwareVersion(void){
			if (openConnection() == 0) return 0;
			// send setting command
			sendCommand(CMD_GET_FW_LEVEL);
			// Read
			return readUInt16Value();
		}
		
		// Setting Commands
		uint16_t getMeasurementInterval(void){
			if (openConnection() == 0) return 0;
			// send setting command
			sendCommand(CMD_SET_MEAS_INTERVAL);
			//read and return
			return readUInt16Value();
		}
		
		void setMeasurementInterval(uint16_t interval){
			if (openConnection() == 0) return;
			// send setting command
			sendCommand(CMD_SET_MEAS_INTERVAL,interval);
		}
		
		uint16_t getAutoCalibration(void){
			if (openConnection() == 0) return 0;
			// send setting command
			sendCommand(CMD_SET_AUTO_SELF_CAL);
			//read and return
			return readUInt16Value();
		}
		
		void setAutoCalibration(uint16_t setting){
			if (openConnection() == 0) return;
			// send setting command
			sendCommand(CMD_SET_AUTO_SELF_CAL,setting);
		}
		
		uint16_t getForcedCalibration(void){
			if (openConnection() == 0) return 0;
			// send setting command
			sendCommand(CMD_SET_FORCED_RECAL_FACTOR);
			//read and return
			return readUInt16Value();
		}
		
		void setForcedCalibration(uint16_t setting){
			if (openConnection() == 0) return;
			// send setting command
			sendCommand(CMD_SET_FORCED_RECAL_FACTOR,setting);
		}
		
		uint16_t getTemperatureOffset(void){
			if (openConnection() == 0) return 0;
			// send setting command
			sendCommand(CMD_SET_TEMP_OFFSET);
			//read and return
			return readUInt16Value();
		}
		
		void setTemperatureOffset(uint16_t setting){
			if (openConnection() == 0) return;
			// send setting command
			sendCommand(CMD_SET_TEMP_OFFSET,setting);
		}
		
		uint16_t getAltitudeCorrection(void){
			if (openConnection() == 0) return 0;
			// send setting command
			sendCommand(CMD_SET_ALTITUDE_COMPENSATION);
			//read and return
			return readUInt16Value();
		}
		
		void setAltitudeCorrection(uint16_t setting){
			if (openConnection() == 0) return;
			// send setting command
			sendCommand(CMD_SET_ALTITUDE_COMPENSATION,setting);
		}
		
		// Measurement commands
		uint16_t getDataReady(void){
			if (openConnection() == 0) return 0;
			// send Read Command
			sendCommand(CMD_GET_DATA_READY);
			//read and return
			return readUInt16Value();
		}
		
		void startMeasurement(void){
			if (openConnection() == 0) return;
			// send start Command
			sendCommand(CMD_START_CONT_MEAS, 0);
		}
		
		void startMeasurement(uint16_t pressure){
			if (openConnection() == 0) return;
			// send start Command with pressure compensation
			sendCommand(CMD_START_CONT_MEAS, pressure);
		}
		
		void stopMeasurement(void){
			if (openConnection() == 0) return;
			// send stop Command
			sendCommand(CMD_STOP_MEAS);
		}
		
		void readMeasurements(float valueArr[3]){
			if (openConnection() == 0) return;
			// send Read Command
			sendCommand(CMD_GET_MEAS);
			// Read
			uint8_t length = 18;  // Number of bytes to read
			if (read(probeI2Cfile, inData, 18) != length) {
				std::cout << "Failed to read from circuit." << std::endl;
			}
			else
			{
				uint8_t crcData[2],crc,i,j,k;
				// print data bytes in decimals
				if (debug){
					for(i=0,j=0; i<18; i++){
						if (j == 2){
							j = 0;
						}
						else{
							std::cout <<"byte = " << std::to_string(inData[i]) << std::endl;
						}
					}
				}
				//extract co2 data
				valueArr[0] = decodeFloatValue(inData, 0);
				//extract temperature data
				valueArr[1] = decodeFloatValue(inData, 6);
				//extract humidity data
				valueArr[2] = decodeFloatValue(inData,12);
			}
			/*returns NaN values for temperature and humidity
			//read co2 data
			valueArr[0] = readFloatValue();
			//read temperature data
			valueArr[1] = readFloatValue();
			//read humidity data
			valueArr[2] = readFloatValue();*/
		}
	
	private:
		uint8_t i2cAddress;
		int probeI2Cfile;  // used when reading and writing to I2C bus
		uint8_t inData[18];  // incoming data
		
		uint8_t openConnection(void){
			// open connection to SCD30 circuit
			if (ioctl(probeI2Cfile, I2C_SLAVE, getAddress()) < 0)
			{
				std::cout << "Failed to acquire bus access" << std::endl;
				return 0;
			}
			return 1;
		}
		
		void sendCommand(uint16_t command, uint16_t argument, bool arg){
			//init
			uint8_t length;
			if (arg) length = 5;
			else length = 2;
			uint8_t commandArr[length], argArr[2], crc;
			
			// parse command
			commandArr[0] = command >> 8;	// MSB of command
			commandArr[1] = command & 0xFF;	// LSB of command
			
			//parse argument
			if (arg){
				argArr[0] = argument >> 8;		// MSB of argument
				argArr[1] = argument & 0xFF;	// LSB of argument
				crc = computeCRC8(argArr,(uint8_t) 2);
				// transfer data to commandArr
				commandArr[2] = argArr[0];
				commandArr[3] = argArr[1];
				commandArr[4] = crc;
			}
			// send
			write(probeI2Cfile, commandArr, length);
			// wait for circuit
			sleep(0.08);
		}
		
		void sendCommand(uint16_t command, uint16_t argument){
			sendCommand(command,argument,true);
		}
		
		void sendCommand(uint16_t command){
			sendCommand(command,0,false);
		}
		
		uint16_t readUInt16Value(){
			uint8_t length = 3;  // Number of bytes to read
			if (read(probeI2Cfile, inData, 3) != length) {
				std::cout << "Failed to read from circuit." << std::endl;
				return 0;
			}
			else{
				uint16_t value;
				value = decodeIntValue(inData, 0);
				return value;
			}
		}
		
		float readFloatValue(){
			uint8_t length = 6;  // Number of bytes to read
			if (read(probeI2Cfile, inData, 6) != length) {
				std::cout << "Failed to read from circuit." << std::endl;
				return 0;
			}
			else{
				float value;
				value = decodeFloatValue(inData, 0);
				return value;
			}
		}
		
		float decodeFloatValue(uint8_t inData[], uint8_t startIndex){
			//init
			uint8_t valData[4],crcData[2],crc,i,j,k;
			float result;
			//main
			for(i=startIndex,j=0,k=0; i < startIndex+6; i++) {
				//check crc
				if (j == 2){
					crc = computeCRC8(crcData,(uint8_t) 2);
					if (inData[i] != crc){
						return 0.0;
						std::cout <<"crc error = " << std::to_string(inData[i]) << " != " << std::to_string(crc) << std::endl;
					}
					j = 0;
				}
				//grab data
				else{
					valData[k++] = inData[i];
					crcData[j++] = inData[i];
				}
			}            
			byte_to_float(&result, &valData[0]);
			//return
			return result;
		}
		
		uint16_t decodeIntValue(uint8_t inData[], uint8_t startIndex){
			//init
			uint8_t valData[2],crcData[2],crc,i,j,k;
			uint16_t result;
			//main
			for(i=startIndex,j=0,k=0; i < startIndex+3; i++) {
				//check crc
				if (j == 2){
					crc = computeCRC8(crcData,(uint8_t) 2);
					if (inData[i] != crc){
						return 0;
						std::cout <<"crc error = " << std::to_string(inData[i]) << " != " << std::to_string(crc) << std::endl;
					}
					j = 0;
				}
				//grab data
				else{
					valData[k++] = inData[i];
					crcData[j++] = inData[i];
				}
			}            
			byte_to_int(&result, &valData[0]);
			//return
			return result;
		}
		
		uint8_t computeCRC8(uint8_t data[], uint8_t len)
		{
			uint8_t crc = 0xFF; //Init with 0xFF

			for (uint8_t x = 0 ; x < len ; x++){
				crc ^= data[x]; // XOR-in the next input byte

				for (uint8_t i = 0 ; i < 8 ; i++)
				{
					if ((crc & 0x80) != 0)
						crc = (uint8_t)((crc << 1) ^ 0x31);
					else
						crc <<= 1;
				}
			}

			return crc; //No output reflection
		}

		void byte_to_float(float *value, uint8_t *p)
		{
			ByteToFl conv;
			for (uint8_t i = 0; i < 4; i++) conv.array[3-i] = *p++ ;
			*value = conv.value;
		}
		
		void byte_to_int(uint16_t *value, uint8_t *p)
		{
			ByteToIn conv;
			for (uint8_t i = 0; i < 2; i++) conv.array[1-i] = *p++ ;
			*value = conv.value;
		}
};