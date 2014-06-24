#include <ManchesterDecoder.h>

ManchesterDecoder* ManchesterDecoder::selfPointer;

ManchesterDecoder::ManchesterDecoder(){
	pinMode(3, INPUT);
  	digitalWrite(3, 1);
	data_buffer = new WordBuffer(150);
	pulse_buffer = new WordBuffer(150);
	halfClock = 1;
	start = true;
	state = ZERO;
	selfPointer = this;
	attachInterrupt(1, ManchesterDecoder::isr2, CHANGE);
	interrupts();
}

ManchesterDecoder::~ManchesterDecoder(){
	delete data_buffer;
	delete pulse_buffer;
	//detachInterrupt(1);
}

void ManchesterDecoder::isr2(){
  	//cli();
  	ManchesterDecoder::selfPointer->interruptResponder();
  	//sei();
}

void ManchesterDecoder::interruptResponder(){
	//Serial.print("*");
	static word last;
  	pulse = micros() - last;
  	last += pulse;
  	//while(pulse_buffer->isFull());
  	pulse_buffer->insert(pulse);
  	//Serial.println(pulse);
}

void ManchesterDecoder::reset(){
	halfClock = 0;
	state = ZERO;
	start = true;
}

uint8_t ManchesterDecoder::getNextData(){
	//Serial.println(pulse_buffer->getNumElements());
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
	//Serial.println(width);
  	if (50 <= width && width < 1400) {
		boolean w = width >= 750;
	//Serial.print(w);
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
	//Serial.print(" : ");
	//Serial.println(halfClock);
	halfClock %= 2;
	if(halfClock == 0){
		//Serial.print(state, HEX);
		data_buffer->insert(state);
	}
  }
  else{
	//Serial.println("Reset: ");
	//Serial.println(width);
	data_buffer->insert(RESET);
	reset();
  }
}