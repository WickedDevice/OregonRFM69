// Author: Joel D. Sabol
// Created: June 2014
// File: ManchesterDecoder.cpp
// Description: Implements an interrupt driven manchester decoder.
// The input and output are fully buffered so that multiple signals
// may be decoded when the arrive close together. The class implementation
// has one limitation in that it was designed to be used for communication
// with the Oregon Scientific sensors using version 3 and version 2.1
// communication protocols. Therefore in the decode method there is a 
// hack which will direct the state machine to use the correct edge 
// based on whether the first pulse was long or short.

/**
 * This Manchester Decoder class is spcifically 
 * designed to decode messages from Oregon 
 * Scientific Sensors, and has been tested on 
 * both version 2.1 and version 3.0 protocols.
 * It has not been tested with anything other
 * than those devices. However it should work, 
 * or only need minor modifications to work.
 * One consideration that must be made is that
 * the code was designed to be used with a device
 * that supports interrupts on the specified pin.
 * @author Joel D. Sabol
 * @date June 2014
 * @file ManchesterDecoder.h */

#ifndef MANCHESTER_DECODER_H
#define MANCHESTER_DECODER_H

#include <Arduino.h>
#include <WordBuffer.h>

#define LONG_PULSE 1  ///< Defines a long pulse as 1.
#define SHORT_PULSE 0 ///< Defines a shor pulse as 0.

#define DEFAULT_SIZE 1024u ///< Defines the default size of the input and output buffers.

#define RESET 0xFFu ///< Defines reset as 0xFF so the parser will know that the decoder timed out.
#define ONE 0x08u ///< Defines One as 0x80 so it can be shifted into the variable.
#define ZERO 0x00u ///< Defines Zero as 0x00 for obvious reasons.

class ManchesterDecoder{
public:
	/** The Default Constructor */
	ManchesterDecoder();
	/** The Destructor */
	~ManchesterDecoder();
	/** Gets the next result from the decoder (ZERO, ONE, RESET). 
	 * @return The next result from the data buffer. */
	uint8_t getNextPulse();
	/** Checks if the data buffer is empty.
	 * @return True if the buffer is not empty; false otherwise. */
	boolean hasNextPulse();
	/** Resets the decoder by clearing the input and output buffers and re-initializing the state machine. */
	void reset();
private:
	/** The private static interrupt service routine.
	 * Responds to the interrupts by calling the interrupt handler. */
	static void isr2();
	/** The private virtual iterrupt handler which is called by the isr. */
	void virtual interruptResponder();
	/** Decodes the pulse width and updates the state machine, which could in turn add data to the data buffer. */
	void decode(word width);
	/** A helper function used to toggle the state of the state machine. */
	void toggle(unsigned int *state);
	/** The static self pointer which is necessary in order for the interrupt
	 * handler to be able to add data to the input buffer. */
	static ManchesterDecoder* selfPointer;
	/** The state variable used by the state machine. */
	unsigned int state;

	/** A variable used by the state machine to determine what state to go to next.
	 * It also determines whether the given pulse was valid or if it produced output. */
	uint8_t halfClock;
	/** A boolean variable which is used to ensure that special
	 * considerations are met when decoding the manchester encoded
	 * data fro the Oregon Scientific Sensors. This is because the
	 * version 3.0 and 2.1 protocols differ in the way in which
	 * they start their messages. In version 3.0 messages you do
	 * not consider the first transition to be decoded as a logical
	 * 1, whereas in the version 2.1 protocol you do in order to
	 * prodce the correct output. */
	boolean start;
	/** The volatile variable pulse is used to record the time
	 * between transition on the data line. It must volatile,
	 * because it appears to the compiler that the value should
	 * never change as it is never called. However since it is
	 * inside an isr it does change thereby requiring the volatile
	 * keyword.*/
	volatile word pulse;
	/** The input buffer in which the pulse values are stored. */
	WordBuffer *pulse_buffer;
	/** The data buffer in which the decoded data is placed. */
	WordBuffer *data_buffer;
};

#endif // MANCHESTER_DECODER_H