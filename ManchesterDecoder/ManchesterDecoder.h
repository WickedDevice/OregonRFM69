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

#ifndef MANCHESTER_DECODER_H
#define MANCHESTER_DECODER_H

#include <Arduino.h>
#include <WordBuffer.h>

#define LONG_PULSE 1
#define SHORT_PULSE 0

#define DEFAULT_SIZE 128u
#define RESET 0xFFu
#define ONE 0x08u
#define ZERO 0x00u

class ManchesterDecoder{
public:
	ManchesterDecoder();
	~ManchesterDecoder();
	uint8_t getNextPulse();
	void reset();
private:
	static void isr2();
	void virtual interruptResponder();
	void decode(word width);
	void toggle(unsigned int *state);
	
	static ManchesterDecoder* selfPointer;
	unsigned int state;
	uint8_t halfClock;
	boolean start;
	volatile word pulse;
	// The input buffer
	WordBuffer *pulse_buffer;
	// The output buffer
	WordBuffer *data_buffer;
};

#endif // MANCHESTER_DECODER_H