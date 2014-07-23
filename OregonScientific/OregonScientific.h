// Author: Joel D. Sabol
// Created: June 2014
// File: OregonScientific.cpp
// Description: Implements a parser that will take the output of
// the Manchester decoded signal and based on the protocol selected,
// will parse the relevant information from the sensor and display it
// to the user upon the arrival of a good packet.

#ifndef OREGON_SCIENTIFIC_H
#define OREGON_SCIENTIFIC_H

#include <Arduino.h>
#include <OregonScientificSensor.h>

#define SYNC_NIBBLE 0 ///< Defines the location of the sync nibble in the message.
#define OSCV_3 0x33	 ///< Defines the version 3.0 protocol.
#define OSCV_2_1 0x21 ///< Defines the version 2.1 protocol.
#define DEFAULT_SIZE 32 ///< Defines the size of the message if none is provided.
#define MAX_SENSOR_NUM 10 ///< Defines the maximum number of sensors that the parser will listen for.
#define DEV_ID_BEGIN 0 ///< Defines the location of the start of the device id in the message.
#define DEV_ID_END 3 ///< Defines the location of the end of the device id in the message.
#define CHANNEL_NIBBLE 4 ///< Defines the location of the channel nibble in the message.
#define ROLLING_CODE_BEGIN 5 ///< Defines the beginning of the rolling code in the message.
#define ROLLING_CODE_END 6 ///< Defines the end of the rolling code in the message.
#define FLAGS 7 ///< Defines where the flags are in the message.
#define MESSAGE_BEGIN 8 ///< Defines the location of the data segment in the message.

/** @enum OregonScientific_ParseStates The states that the parser
 * can be in while parsing the message. */
enum OregonScientific_ParseState{
								 SYNCING, ///< The parser is in this state while looking for the sync nibble which will determine where the other elements will be.
								 GET_ID,  ///< The parser is in this state while parsing the device id.
								 GET_MSG,  ///< The parser is in this state while parsing the message.
								 DONE ///< The parser is in this state upon completion of parsing the message.
								};

/** OregonScientific defines a parser capable of parsing
 * both version 2.1 and version 3.0 messages from Oregon
 * Scientific sensors.
 * @class OregonScientific
 * @author Joel D. Sabol
 * @date June 2014
 * @details The parser will take the output of the Manchester decoder
 * and parse that data until it finds the sync nibble. It will then parse
 * the device id and the channel. These two data members will be used to
 * lookup the sensor that sent the message. If the sensor is found it will
 * be placed in the current sensor variable
 */
class OregonScientific
{
public:
	/** The default constructor. */
	OregonScientific();
	/** The constructor which takes the length of the message.
	 * @param msgLen The expected length of the message. */
	OregonScientific(uint8_t msgLen);
	/** The destructor. */
	~OregonScientific();
	/** The member function that parses the version 3.0 protocol.
	 * It parses the message as it receives the data from an outside source.
	 * @param width The value to be shifted into the current nibble. */
	boolean parseOregonScientificV3(uint8_t width);
	/** The member function that parses the version 2.1 protocol.
	 * It parses the message as it receives the data from an outside source.
	 * @param width The value to be shifted into the current nibble. */
	boolean parseOregonScientificV2(uint8_t width);
	/** The member function that "listens" for a message that was
	* sent by its sensor. So whenever the parser parses a device id
	* and channel id it will search for the sensor that matches the
	* device id - channel id combination.
	* @param *sensor The sensor that will be listened for by the parser.*/
	void addSensor(OregonScientificSensor *sensor);
	/** Prints the results of the two sensors that this code has been tested with*/
	virtual void printResults(uint8_t protocol);
	/** Allows the parser to be reset manually. Though it is
	 * used internally by the class as well. */
	virtual void reset();
	/** Returns the sensor that sent the message. */
	OregonScientificSensor* getCurrentSensor();
private:
	/** Validates the message by computing the checksum and
	* checking to see if it matches the checksum that was sent
	* by the sensor.
	* @return True if the checksums matched, false otherwise. */
	boolean validate(uint8_t value);
	/** Finds the sensor that matches the device id and channel number
	 * of the message that is currently being received. If it is found
	 * it will place the sensor in the current sensor variable. In
	 * addition to this it will also get the size of the message that
	 * is being received so that the parser will know when to stop.
	 * This message size is also used to determine where the checksum
	 * is located.
	 * @return True if the sensor was found, false otherwise. */
	boolean findSensor();
	/** The counter that is used to track the number of bits added
	 * to each nibble. */
	uint8_t subNibbleCount;
	/** The current index into the message array. */
	int idx;
	/** Used by the version 2.1 protocol parser to determine which bits
	 * to throw away. */
	uint8_t bitCount;
	/** The variable that stores the message size when find sensor is called. */
	uint8_t messageSize;
	/** Holds the number of sensors that are currently attached to the parser. */
	uint8_t numSensors;
	/** The sensor that sent the current message. */
	OregonScientificSensor *currentSensor;
	/** The array of sensors that are currently being listened for. */
	OregonScientificSensor *sensors[MAX_SENSOR_NUM];
	/** The variable that holds the current state of the parser. */
	OregonScientific_ParseState state;
	/** The array that holds the message. */
	uint8_t *data;	
};

#endif // OREGON_SCIENTIFIC_H