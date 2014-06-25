#include <WildFire.h>
#include <WordBuffer.h>
#include <ManchesterDecoder.h>
#include <OregonScientific.h>

#define SERIAL_BAUD 115200

WildFire wf;
// Manchester Decoder
ManchesterDecoder md;
// Oregon Scientific Version 3 parser
OregonScientific oscv3(15);
// Oregon Scientific Version 2.1 parser
OregonScientific oscv2(18);


void setup(){
  wf.begin();
  Serial.begin(SERIAL_BAUD);
  Serial.println("Listening on 433.92Mhz");
}

void loop(){
  // Gets the next value from the decoder
  uint8_t data = md.getNextData();
  
  // If value indicates timeout then reset
  if(data == RESET){
    oscv3.reset();
    oscv2.reset();
  } // Otherwise put the data in both parsers
  else{
    if(oscv3.parseOregonScientificV3(data)){
      oscv3.printResults(OSCV_3);
      oscv3.reset();
      oscv2.reset();
    }
    if(oscv2.parseOregonScientificV2(data)){
      oscv2.printResults(OSCV_2_1);
      oscv2.reset();
      oscv3.reset();
    }
  }
}
    

    
