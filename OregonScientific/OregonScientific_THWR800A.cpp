#include <OregonScientific_THWR800A.h>

OregonScientific_THWR800A::OregonScientific_THWR800A(): OregonScientific(DEFAULT_SIZE){
	reset();
}

OregonScientific_THWR800A::OregonScientific_THWR800A(uint8_t mSize):OregonScientific::OregonScientific(mSize){
	
	reset();
}

void OregonScientific_THWR800A::reset(){
	//TODO Reset all member variables
}

void OregonScientific_THWR800A::parseSensorData(){

}