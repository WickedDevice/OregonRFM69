// Author: Joel D. Sabol
// Created: June 2014
// File: OregonScientific.cpp
// Description: Implements a parser that will take the output of
// the manchester decoded signal and based on the protocol selected,
// will parse the relavent information from the sensor and display it
// to the user upon the arrival of a good packet.

#ifndef OREGON_SCIENTIFIC_H
#define OREGON_SCIENTIFIC_H

#include <Arduino.h>

#define SYNC_NIBBLE 0
#define OSCV_3 0x33
#define OSCV_2_1 0x21
#define DEFAULT_SIZE 32
#define POOL_TEMP_THWR800A_ID 0XC844
#define TEMP_HUMID_THGR122NR_ID 0X1D20
#define DEV_ID_BEGIN 0
#define DEV_ID_END 3
#define CHANNEL_NIBBLE 4
#define ROLLING_CODE_BEGIN 5
#define ROLLING_CODE_END 6
#define FLAGS 7
#define MESSAGE_BEGIN 8

enum OregonScientific_ParseState{SYNCING, IN_SYNC, DONE};

class OregonScientific
{
public:
	OregonScientific();
	OregonScientific(uint8_t msgLen);
	~OregonScientific();
	boolean parseOregonScientificV3(uint8_t width);
	boolean parseOregonScientificV2(uint8_t width);
	virtual void printResults(uint8_t protocol);
	virtual void reset();
private:
	boolean validate(uint8_t value);

	uint8_t subNibbleCount;
	int idx;
	uint8_t bitCount;
	uint8_t messageSize;
	OregonScientific_ParseState state;
protected:
	uint8_t *data;	
};

#endif // OREGON_SCIENTIFIC_H