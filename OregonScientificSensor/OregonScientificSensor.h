#ifndef OREGON_SCIENTIFIC_SENSOR_H
#define OREGON_SCIENTIFIC_SENSOR_H

#include <Arduino.h>


// Defines the device ID codes
#define BTHR918 0x050A050D 	///< Device ID code for the Oregon Scientific BTHR918
#define BTHR968 0x050D0600 	///< Device ID code for the Oregon Scientific BTHR968
#define PCR800 0x02090104 	///< Device ID code for the Oregon Scientific PCR800
#define RGR918 0x020A010D 	///< Device ID code for the Oregon Scientific RGR918
#define RGR968 0x020D0100 	///< Device ID code for the Oregon Scientific RGR968
#define STR918 0x030A000D 	///< Device ID code for the Oregon Scientific STR918
#define THGN123N 0x010D0200 ///< Device ID code for the Oregon Scientific THGN123N
#define THGN801 0x0F080204  ///< Device ID code for the Oregon Scientific THGN801
#define THGR122NX 0x010D0200///< Device ID code for the Oregon Scientific THGR122NX
#define THGR228N 0x010A020D ///< Device ID code for the Oregon Scientific THGR228N
#define THGR810 0x0F080204  ///< Device ID code for the Oregon Scientific THGR810
#define THGR8101 0x0F080B04 ///< Device ID code for the Oregon Scientific THGR8101
#define THGR918 0x010A030D  ///< Device ID code for the Oregon Scientific THGR918
#define THN132N 0x0E0C0400  ///< Device ID code for the Oregon Scientific THN132N
#define THR238NF 0x0E0C0400 ///< Device ID code for the Oregon Scientific THR238NF
#define THWR288A 0x0E0A040C ///< Device ID code for the Oregon Scientific THWR288A
#define	THWR800 0x0C080404  ///< Device ID code for the Oregon Scientific THWR800
#define UVN800 0x0D080704   ///< Device ID code for the Oregon Scientific UVN800
#define UVR128 0x0E0C0700   ///< Device ID code for the Oregon Scientific UVR128
#define WGR8002 0x01090904  ///< Device ID code for the Oregon Scientific WGR8002
#define WGR8003 0x01090804  ///< Device ID code for the Oregon Scientific WGR8003
#define WGR918 0x030A000D   ///< Device ID code for the Oregon Scientific WGR918

// Protocol Version 2.1 Channels
#define V2_CHANNEL_1	0x01 ///< Protocol version 2.1 Channel 1
#define V2_CHANNEL_2	0x02 ///< Protocol version 2.1 Channel 2
#define V2_CHANNEL_3	0x04 ///< Protocol version 2.1 Channel 3

// Protocol Version 3.0 Channels
#define V3_CHANNEL_1	0x01 ///< Protocol version 3.0 Channel 1
#define V3_CHANNEL_2	0x02 ///< Protocol version 3.0 Channel 2
#define V3_CHANNEL_3	0x03 ///< Protocol version 3.0 Channel 3

// id_type union to make it easy to 
// convert between char array and long Protocol version 3.0 Channel 1
/** A union for ease of converting between the array
 * and integer representations of the device ID. 
 * @union id_type  */
union id_type{
	uint8_t array[4];
	uint32_t value;
};

/** A class that encompasses the necessary information about
 * Oregon Scientific sensors and the methods for accessing
 * that information. 
 * @class OregonScientificSensor
 * @author Joel D. Sabol
 * @todo Add more message formats and titles to the class.
 * @details Contains information pertaining to the sensors
 * such as message format, message length, channel id, and
 * device ID. Additionally it contains helper methods to
 * turn the message that is received into a JSON message
 * that can be sent to a server or application.  */
class OregonScientificSensor
{
public:
	/** The default constructor which requires information about the sensor.
	 * @param id The device id - Best to use the defined device IDs, however you can create one for a sensor.
	 * @param dev_channel The channel on which the device is "broadcasting".
	 * @param size The size of just the message, not including the basic sensor information.
	 * @param *msg_format The array of number pairs comprised of the beginning index and end index of every data member of the message (See examples in .cpp file).
	 * @param *msg_spec The title corresponding to each number pair. 
	 * @see THGR122NX Example of id parameter.
	 * @see V2_CHANNEL_1 Example of dev_channel parameter.
	 * @see THGR122NX_FORMAT[] Example of msg_format parameter.
	 * @see THGR122NX_TITLES[] Example of msg_spec parameter.
	 */
	OregonScientificSensor(const uint32_t id, const uint8_t dev_channel, 
		const uint8_t size, const uint8_t *msg_format, const String *msg_spec);
	/** The destructor */
	~OregonScientificSensor();
	/** Gets the sensor ID.
	 * @return the ID of the sensor as a 32 bit integer.
	 * @details This method is generally used by the Oregon Scientific class when searching for sensors. */	
	uint32_t getSensorID();
	/** Gets the channel that the sensor is on.
	 * @return The channel id as an 8 bit integer.
	 * @details This method is generally used by the Oregon Scientific class when searching for a sensor. */
	uint8_t getSensorChannel();
	/** Creates a JSON message given the data in the standard Oregon Scientific format.
	 * @param *message The standard Oregon Scientific message.
	 * @todo Shore up what the exact format will be for communicating with the server. */
	void makeJSONMessage(uint8_t *message);
	/** Gets the String representation of the JSON formated message. 
	 * @return The string object containing the JSON message.*/
	String getJSONMessage();
	/** Gets the char array representation of the JSON formated message. 
	 * @param The buffer to place the JSON formatted message into. */
	void getCharMessage(char &msg);
	/** Gets the expected size of the message.
	 * @return The integer containing the expected size of the message. 
	 * @details Generally used by the parser to determine when to stop
	 * parsing a message. */
	uint8_t getMessageSize();
	/** The message format of the THGR122NX. */
	static const uint8_t THGR122NX_FORMAT[];
	/** The message format of the THWR800. */
	static const uint8_t THWR800_FORMAT[];
	/** The titles corresponding to the THGR122NX_FORMAT message format.*/
	static const String THGR122NX_TITLES[];
	/** The titles corresponding to the THWR800_FORMAT message format.*/
	static const String THWR800_TITLES[];
private:
	/** The member variable holding the sensors device ID */
	uint32_t dev_id;
	/* The member array containing the message format. */
	uint8_t *format;
	/* The member variable holding the channel id. */
	uint8_t channel;
	/** The member variable holding the expected size of the message.*/
	uint8_t msg_size;
	/** The member array holding the titles corresponding to the format.*/
	String *titles;
	/** The member variable that holds the JSON message after it is created. */
	String json_msg;
};
#endif //OREGON_SCIENTIFIC_SENSOR_H