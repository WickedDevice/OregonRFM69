#ifndef OREGONSCIENTIFIC_THWR800A
#define OREGONSCIENTIFIC_THWR800A

#include <OregonScientific.h>

#define TEMP_SIGN 0
#define TEMP_TEN 1
#define TEMP_ONE 2
#define TEMP_DEC 3
#define MESSAGE_SIZE 16
#define MESSAGE

class OregonScientific_THWR800A: public OregonScientific{
public:
	OregonScientific_THWR800A();
	OregonScientific_THWR800A(uint8_t messageSize);
	void reset();
	void parseSensorData();
	void printResults();
private:
	char temp[4];
	uint8_t checksum;

};

#endif // OREGONSCIENTIFIC_THWR800A