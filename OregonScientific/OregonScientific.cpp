#include <OregonScientific.h>

OregonScientific::OregonScientific(){
	data = new uint8_t[DEFAULT_SIZE];
	numSensors = 0;
	messageSize = DEFAULT_SIZE;
	reset();
}

OregonScientific::OregonScientific(uint8_t messageSize = DEFAULT_SIZE){
	data = new uint8_t[messageSize];
	OregonScientific::messageSize = messageSize;
	reset();
}

OregonScientific::~OregonScientific(){
	delete[] data;
}

void OregonScientific::reset(){
  	subNibbleCount = 0;
  	idx = 0;
  	state = SYNCING;
  	bitCount = 0;
  	for (int i = 0; i < messageSize; ++i)
  	{
  		data[i] = 0;
  	}
}

OregonScientificSensor* OregonScientific::getCurrentSensor(){
	return currentSensor;
}

void OregonScientific::printResults(uint8_t protocol){
  Serial.println();
  switch(protocol){
  case OSCV_3:
	Serial.print(F("OSCV_3:\t"));
	break;
  case OSCV_2_1:
	Serial.print(F("OSCV_2_1:\t"));
	break;
  }
  for(int i = DEV_ID_BEGIN; i < idx; i++){
	Serial.print(data[i], HEX);
  }
  Serial.println();
  Serial.print(F("Dev ID:\t\t"));
  for(int i = DEV_ID_BEGIN; i <= DEV_ID_END; i++){
	Serial.print(data[i], HEX);
  }
  Serial.print(F("\nBattery:\t"));
  if(data[FLAGS] >> 2){
  	Serial.println(F("Low"));
  }else{
  	Serial.println(F("Ok"));
  }
  Serial.print(F("Channel:\t"));
  if(protocol == OSCV_3){
  	Serial.println(data[CHANNEL_NIBBLE]);
  }else{
  	switch(data[CHANNEL_NIBBLE]){
  		case 0x01: Serial.println(F("1"));
  		break;
  		case 0x02: Serial.println(F("2"));
  		break;
  		case 0x04: Serial.println(F("3"));
  		break;
  		default: Serial.println(F("Channel Error"));
  	}
  }
  Serial.print(F("Temp:\t\t"));
  if(!(data[13] & 0x08) >> 3){
	Serial.print(F("-"));
  }
  Serial.print(data[10], HEX);
  Serial.print(data[9], HEX);
  Serial.print(F("."));
  Serial.print(data[8], HEX);
  Serial.println(F("C"));
  if(protocol == OSCV_2_1){
	Serial.print(F("Humidity:\t"));
	Serial.print(data[13], HEX);
	Serial.print(data[12], HEX);
	Serial.println(F("%\n"));
  }
}

boolean OregonScientific::parseOregonScientificV2(uint8_t value){
	if(idx > messageSize){
		//reset();
		return false;
	}
	bitCount++;
	if(bitCount & 0x01){
		data[idx] = data[idx] >> 1;
		data[idx] = data[idx] | value;

		switch(state){
			case SYNCING:
				if(data[idx] == 0x0A){
					state = GET_ID;
					subNibbleCount = 0;
				}
				break;
			case GET_ID:
				if ((subNibbleCount & 0x03) == 3){
					idx++;
				}
				subNibbleCount++;
				if (idx > CHANNEL_NIBBLE){
					if(findSensor()){
						state = GET_MSG;
						//messageSize = currentSensor->getMessageSize();
					}else{
						state = SYNCING;
					}

				}
				break;
			case GET_MSG:
				if ((subNibbleCount & 0x03) == 3){
					idx++;
				}
				subNibbleCount++;
				if(idx >= messageSize){
					state = DONE;
				}
				break;
			case DONE:
				currentSensor->makeJSONMessage(data);
				return validate(messageSize-3);
		}
	}
	return false;
}

boolean OregonScientific::parseOregonScientificV3(uint8_t value){
	if(idx > messageSize){
		return false;
	}
	data[idx] = data[idx] >> 1;
	data[idx] = data[idx] | value;

	switch(state){
			case SYNCING:
				if(data[idx] == 0x0A){
					state = GET_ID;
					//idx++;
					subNibbleCount = 0;
				}
				break;
			case GET_ID:
				if ((subNibbleCount & 0x03) == 3){
					idx++;
				}
				subNibbleCount++;
				if (idx > CHANNEL_NIBBLE){
					if(findSensor()){
						state = GET_MSG;
						//messageSize = currentSensor->getMessageSize();
					}else{
						state = SYNCING;
					}
				}
				break;
			case GET_MSG:
				if ((subNibbleCount & 0x03) == 3){
					idx++;
				}
				subNibbleCount++;
				if(idx >= messageSize){
					state = DONE;
				}
				break;
			case DONE:
				currentSensor->makeJSONMessage(data);
				return validate(messageSize-3);
		}
	return false;
}

void OregonScientific::addSensor(OregonScientificSensor *sen1){
	sensors[numSensors] = sen1;
	numSensors++;
}

boolean OregonScientific::findSensor(){
	id_type temp;
	// Reverse copy the data into the union to convert
	// the dev_id to an integer representation.
	for(uint8_t i = 0; i < 4; i++){
		temp.array[i] = data[DEV_ID_END-i];
	}
	// Check all the sensors to find the sensor that matches the dev ID and channel
	for (uint8_t i = 0; i < numSensors; i++){
		if(sensors[i]->getSensorID() == temp.value && 
			sensors[i]->getSensorChannel() == data[CHANNEL_NIBBLE]){
			currentSensor = sensors[i];
			messageSize = currentSensor->getMessageSize();
			return true;
		}
	}
	return false;
}

boolean OregonScientific::validate(uint8_t value){
	// Converts the checksum to a single integer value for comparison
	uint8_t chksum_dev = (data[value+1] << 4) | data[value];
	uint8_t chksum_computed = 0;
	// Computes the checksum of the message
	for(uint8_t i = DEV_ID_BEGIN; i < value; i++){
		chksum_computed += data[i];
	}

	return (chksum_computed == chksum_dev);
}