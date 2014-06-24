#include <OregonScientific.h>

OregonScientific::OregonScientific(){
	data = new uint8_t[DEFAULT_SIZE];
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


void OregonScientific::printResults(uint8_t protocol){
  Serial.println();
  switch(protocol){
  case OSCV_3:
	Serial.print("OSCV_3:\t");
	break;
  case OSCV_2_1:
	Serial.print("OSCV_2_1:\t");
	break;
  }
  for(int i = DEV_ID_BEGIN; i < idx; i++){
	Serial.print(data[i], HEX);
  }
  Serial.println();
  Serial.print("Dev ID:\t\t");
  for(int i = DEV_ID_BEGIN; i <= DEV_ID_END; i++){
	Serial.print(data[i], HEX);
  }
  Serial.print("\nChannel:\t");
  if(protocol == OSCV_3){
  	Serial.println(data[CHANNEL_NIBBLE]);
  }else{
  	switch(data[CHANNEL_NIBBLE]){
  		case 0x01: Serial.println("1");
  		break;
  		case 0x02: Serial.println("2");
  		break;
  		case 0x04: Serial.println("3");
  		break;
  		default: Serial.println("Channel Error");
  	}
  }
  Serial.print("Temp:\t\t");
  if(!(data[13] & 0x08) >> 3){
	Serial.print('-');
  }
  Serial.print(data[10], HEX);
  Serial.print(data[9], HEX);
  Serial.print(".");
  Serial.print(data[8], HEX);
  Serial.println("C");
  if(protocol == OSCV_2_1){
	Serial.print("Humidity:\t");
	Serial.print(data[13], HEX);
	Serial.print(data[12], HEX);
	Serial.println("%\n");
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
					state = IN_SYNC;
					//idx++;
					subNibbleCount = 0;
				}
				break;
			case IN_SYNC:
				if ((subNibbleCount & 0x03) == 3)
				{
					//Serial.println(data[idx], HEX);
					idx++;
				}
				subNibbleCount++;
				if(idx >= 18){
					state = DONE;
				}
				break;
			case DONE:
				return validate(15);
		}
	}
	return false;
}

boolean OregonScientific::parseOregonScientificV3(uint8_t value){
	//subNibbleCount++;
	if(idx > messageSize){
		//reset();
		return false;
	}
	data[idx] = data[idx] >> 1;
	data[idx] = data[idx] | value;

	switch(state){
			case SYNCING:
				if(data[idx] == 0x0A){
					state = IN_SYNC;
					//idx++;
					subNibbleCount = 0;
				}
				break;
			case IN_SYNC:
				if ((subNibbleCount & 0x03) == 3)
				{
					idx++;
				}
				subNibbleCount++;
				if(idx >= 15 && data[DEV_ID_BEGIN] == 0x0C){
					state = DONE;
				}
				break;
			case DONE:
				return validate(12);
		}
	return false;
}

boolean OregonScientific::validate(uint8_t value){
	uint8_t chksum_dev = (data[value+1] << 4) | data[value];
	uint8_t chksum_cmp = 0;
	for(uint8_t i = DEV_ID_BEGIN; i < value; i++){
		chksum_cmp += data[i];
	}
	Serial.println(chksum_cmp);
	Serial.println(chksum_dev);
	if(chksum_cmp == chksum_dev){
		return true;
	}
	return false;
}
