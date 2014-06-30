#include <WildFire.h>
#include <WordBuffer.h>
#include <ManchesterDecoder.h>
#include <OregonScientific.h>
#include <OregonScientificSensor.h>
#include <WildFire_CC3000.h>
#include <SPI.h>
//#include <dht.h>

#define SERIAL_BAUD 115200
#define DHT22_PIN 5

WildFire wf;
// Manchester Decoder
ManchesterDecoder md;
// Oregon Scientific Version 3.0 parser
OregonScientific oscv3;
// Oregon Scientific Version 2.1 parser
OregonScientific oscv2;

WildFire_CC3000 cc3000;
WildFire_CC3000_Client client;

dht dht22;

void reset(){
  oscv3.reset();
  oscv2.reset();
}

uint8_t* stringToByteArr(String msg){
  uint16_t len = msg.length();
  uint8_t *data =  new uint8_t[len];
  for(int i = 0; i < len; i++){
    data[i] = msg[i];
  }
  return data;
}

void readDHT22(){
      // READ DATA
    Serial.print("DHT22, \t");

    uint32_t start = micros();
    while(dht22.read22(DHT22_PIN) != DHTLIB_OK);
    uint32_t stop = micros();
    // DISPLAY DATA
    Serial.print(dht22.humidity, 1);
    Serial.print(",\t");
    Serial.print(dht22.temperature, 1);
    Serial.print(",\t");
    Serial.println(stop - start);
}

void setup(){
  wf.begin();
  Serial.begin(SERIAL_BAUD);
  oscv2.addSensor(new OregonScientificSensor(THGR122NX, V2_CHANNEL_1, 7, OregonScientificSensor::THGR122NX_FORMAT, OregonScientificSensor::THGR122NX_TITLES));
  oscv2.addSensor(new OregonScientificSensor(THGR122NX, V2_CHANNEL_3, 7, OregonScientificSensor::THGR122NX_FORMAT, OregonScientificSensor::THGR122NX_TITLES));
  oscv3.addSensor(new OregonScientificSensor(THWR800, V2_CHANNEL_1, 6, OregonScientificSensor::THWR800_FORMAT, OregonScientificSensor::THWR800_TITLES));
  Serial.println("Listening on 433.92Mhz");
}

void loop(){
  // Gets the next value from the decoder
  uint8_t data = md.getNextPulse();
  
  // If value indicates timeout then reset
  if(data == RESET){
    reset();
  } // Otherwise put the data in both parsers
  else{
    if(oscv3.parseOregonScientificV3(data)){
      String msg = oscv3.getCurrentSensor()->getJSONMessage();
      Serial.println(msg);
      readDHT22();
      reset();
    }
    if(oscv2.parseOregonScientificV2(data)){
      String msg = oscv2.getCurrentSensor()->getJSONMessage();
      Serial.println(msg);
      reset();
    }
  }
}
    

    
