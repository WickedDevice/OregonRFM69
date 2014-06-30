#include <OregonScientificSensor.h>

const uint8_t OregonScientificSensor::THGR122NX_FORMAT[]= { 0,18,        0,3,     4,4,       7,7,       8,11,   12,13,      15,16}; 
const String OregonScientificSensor::THGR122NX_TITLES[] = {"THGR122NX", "DevID", "Channel", "Battery", "Temp", "Humidity", "Checksum"}; 
const String OregonScientificSensor::THWR800_TITLES[] = {"THWR800","DevID", "Channel", "Battery", "Temp", "Checksum"};
const uint8_t OregonScientificSensor::THWR800_FORMAT[] ={ 0,15,     0,3,     4,4,       7,7,       8,11,   12,13};

OregonScientificSensor::OregonScientificSensor(const uint32_t id, const uint8_t dev_channel,
					const uint8_t size, const uint8_t *msg_format, const String *msg_titles){
	dev_id = (uint32_t) id;
	format = (uint8_t*) msg_format;
	titles = (String*) msg_titles;
	channel = (uint8_t) dev_channel;
	msg_size = (uint8_t) size;
}
OregonScientificSensor::~OregonScientificSensor(){}
uint32_t OregonScientificSensor::getSensorID(){
	return dev_id;
}

uint8_t OregonScientificSensor::getSensorChannel(){
	return channel;
}

uint8_t OregonScientificSensor::getMessageSize(){
	return format[1];
}

String OregonScientificSensor::getJSONMessage(){
	return json_msg;
}

void OregonScientificSensor::makeJSONMessage(uint8_t *message){
	const char hexToChar[] = {'0','1','2','3','4','5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	//String hexToChar(dt);
	json_msg = "";
	json_msg += "{[\n{\"title\":\"";
	json_msg += titles[0];
	json_msg += "\",\"data\":\"";
	for(uint8_t i = format[0]; i <= format[1]; i++){
		json_msg+= hexToChar[message[i]];
	}
	json_msg += "\"}";
	uint8_t formatCounter = 2;
	for (int8_t i = 1; i < msg_size; i++)
	{
		json_msg += ",\n{\"title\":\"";
		json_msg += titles[i];
		json_msg += "\",\"data\":\"";
		for(int8_t j = format[formatCounter+1]; j >= format[formatCounter]; j--){
			json_msg +=hexToChar[message[j]];
		}
		json_msg += "\"}";
		formatCounter+=2;
	}
	json_msg += "\n]}";
}
