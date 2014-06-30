#include <ManchesterDecoder.h>

// Static self pointer definition for the ISR
ManchesterDecoder* ManchesterDecoder::selfPointer;

/*
 * The Constructor of the ManchesterDecoder class
 */
ManchesterDecoder::ManchesterDecoder(){
	// Configures the interrupt pin as  INPUT
	pinMode(3, INPUT);
	// Configures the interrupt pin with internal pullup resistor
  	digitalWrite(3, 1);
  	// Allocates memory for the buffers
	data_buffer = new WordBuffer(150);
	pulse_buffer = new WordBuffer(150);
	// Initializes the member variables
	halfClock = 1;
	start = true;
	state = ZERO;
	selfPointer = this;
	// Attaches the interrupt to the IRS on pin change
	attachInterrupt(1, ManchesterDecoder::isr2, CHANGE);
	// Enable interrupts
	interrupts();
}

/*
 * The destructor of the ManchesterDecoder class
 */
ManchesterDecoder::~ManchesterDecoder(){
	// Frees the memory occupied by the buffers
	delete data_buffer;
	delete pulse_buffer;
}

/*
 * The ISR
 */
void ManchesterDecoder::isr2(){
  	//cli();
  	// Calls the interrupt handler
  	ManchesterDecoder::selfPointer->interruptResponder();
  	//sei();
}

/*
 * The interrupt handeler for called by the ISR
 */
void ManchesterDecoder::interruptResponder(){
	// Static variable records the last time when the function was called
	static word last;
	// Computes the time since the last function call
  	pulse = micros() - last;
  	last += pulse;
  	// Assumes that the buffer will always be empty
  	// Insert the pulse into the buffer
  	pulse_buffer->insert(pulse);
}

/*
 * Resets the member variables.
 */
void ManchesterDecoder::reset(){
	halfClock = 0;
	state = ZERO;
	start = true;
}

uint8_t ManchesterDecoder::getNextPulse(){
	do{
		while(!pulse_buffer->isEmpty()){
			decode(pulse_buffer->remove());
		}
	}while(data_buffer->isEmpty());
	return data_buffer->remove();
	
	return -1;
}

void ManchesterDecoder::toggle(unsigned int *state){
	if(state == ZERO){
		*state = ONE;
	}else{
		*state = ZERO;
	}
}

void ManchesterDecoder::decode(word width){
  	if (50 <= width && width < 1400) {
		boolean w = width >= 750;
		switch(w){
			 // Short pulses increase count by 1
			// There is a boundary condition between the two protocols
			case SHORT_PULSE:
				if(!start){
					halfClock++;
			  		break;
				}
			 // Long pulses increase count by 2
			case LONG_PULSE:
				start = false;
				if(state == ZERO){
					state = ONE;
				}else{
					state = ZERO;
				}
				halfClock+=2;
				break;
		}
		halfClock %= 2;
		if(halfClock == 0){
			data_buffer->insert(state);
		}
  	}
  	else{
		data_buffer->insert(RESET);
		reset();
  	}
}