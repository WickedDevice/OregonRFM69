#ifndef OREGON_SCIENTIFIC_SENSOR_H
#define OREGON_SCIENTIFIC_SENSOR_H

#include<Arduino.h>

// Defines the device ID codes
#define BTHR918 0x050A050D
#define BTHR968 0x050D0600
#define PCR800 0x02090104
#define RGR918 0x020A010D
#define RGR968 0x020D0100
#define STR918 0x030A000D
#define THGN123N 0x010D0200
#define THGN801 0x0F080204
#define THGR122NX 0x010D0200
#define THGR228N 0x010A020D
#define THGR810 0x0F080204
#define THGR8101 0x0F080B04
#define THGR918 0x010A030D
#define THN132N 0x0E0C0400
#define THR238NF 0x0E0C0400
#define THWR288A 0x0E0A040C
#define	THWR800 0x0C080404
#define UVN800 0x0D080704
#define UVR128 0x0E0C0700
#define WGR8002 0x01090904
#define WGR8003 0x01090804
#define WGR918 0x030A000D

// Protocol Version 2.1 Channels
#define V2_CHANNEL_1	0x01
#define V2_CHANNEL_2	0x02
#define V2_CHANNEL_3	0x04

// Protocol Version 3.0 Channels
#define V3_CHANNEL_1	0x01
#define V3_CHANNEL_2	0x02
#define V3_CHANNEL_3	0x03

// id_type union to make it easy to 
// convert between char array and long
union id_type{
	uint8_t array[4];
	uint32_t value;
};

class OregonScientificSensor
{
public:
	OregonScientificSensor(const uint32_t id, const uint8_t dev_channel, const uint8_t size, const uint8_t *msg_format, const String *msg_spec);
	~OregonScientificSensor();
	uint32_t getSensorID();
	uint8_t getSensorChannel();
	void makeJSONMessage(uint8_t *message);
	String getJSONMessage();
	uint8_t getMessageSize();
	static const uint8_t THGR122NX_FORMAT[];
	static const uint8_t THWR800_FORMAT[];
	static const String THGR122NX_TITLES[];
	static const String THWR800_TITLES[];
private:
	uint32_t dev_id;
	uint8_t *format;
	uint8_t channel;
	uint8_t msg_size;
	String *titles;
	String json_msg;

};
#endif //OREGON_SCIENTIFIC_SENSOR_H