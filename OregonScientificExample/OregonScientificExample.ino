/** Oregon Scientific Example is the main program
 * that handles the configuration and the main loop
 * of the program.
 * @todo Add the ability to dynamicall add and remove sensors.
 * @file OregonScientificExample.ino */

#include <WildFire.h>
#include <WildFire_CC3000.h>
#include <WordBuffer.h>
#include <ManchesterDecoder.h>
#include <OregonScientific.h>
#include <OregonScientificSensor.h>
#include <LiquidCrystal.h>
#include <dht.h>
#include <SPI.h>
#include <TinyWatchdog.h>
#include "header.h"


WildFire wf; ///< The instantiation of the WildFire
WildFire_CC3000 cc3000; ///< The instantiation of the CC3000 radio
TinyWatchdog tinyWDT; ///< The Watchdog Timer
ManchesterDecoder md; ///< The Manchester Decoder
OregonScientific oscv3; ///< The Oregon Scientific Version 3.0 Parser
OregonScientific oscv2; ///< Oregon Scientific Version 2.1 parser

char packet_buffer[MAX_PACKET_LENGTH]; ///< The packet buffer used to hold the packet

char address[13] = {
  '0','8','0','0','2','8','5','7','5','A','0','E'}; ///< The hard-coded device address: This really should not be this way, however there is an issue with reading the MAC address from the CC3000 which when run keeps it from connecting to the server via TCP. 

int building_id = -1; ///< The variable holding the id of the building that the device is currently in

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7); ///< The instantiation of the LCD

dht dht22; ///<  The instantiation of the DHT22 object

uint32_t ip; ///< The variable holding the IP address of the server

uint16_t current_time; ///< Stores the time that the checkNPet function is called at so a comparison can be made.
uint16_t time_last_pet; ///< Stores the time at which the watchdog timer was last petted.

/** @enum The enumeration of the states that the device can be in. */
enum device_states{
  PING_SERVER, ///< In this state the device will check
  ///< the server for information regarding what 
  ///< sensors it has as well as where to send the sensor datum to.
  ACTIVATED,   ///< In this state the device will send the data
  ///< from the sensors to the URI specified by the
  ///< immediately after receiving data from the sensors.
  GEN_SENSOR   ///< In this state the device will generate the
  ///< sensors that the sever told it to listen for.
};


/** Resets the both version protocol parsers. */
void resetParser(){
  oscv3.reset();
  oscv2.reset();
}

/** Converts the string to its byte array representation.
 * @param msg The string to be converted.
 * @param *data The buffer to place the bytes of the string into. */
void stringToByteArr(String msg, char *data){
  uint16_t len = msg.length();
  for(int i = 0; i < len; i++){
    data[i] = msg.charAt(i);
  }
  data[len] = '\0';
}


/** Reads the DHT22 until it returns a valid reading
 * then prints out that reading.                  */
void readDHT22(){
#ifdef DEVELOPMENT
  Serial.print("DHT22, \t");
#endif
  while(dht22.read22(DHT22_PIN) != DHTLIB_OK);
#ifdef DEVELOPMENT
  Serial.print(dht22.humidity, 1);
  Serial.print(",\t");
  Serial.println(dht22.temperature, 1);
#endif
  lcd_print_dht22(dht22.temperature, dht22.humidity);
}

/** Assembles the necessary information to create the JSON string that contains the data from the DHT22. */
String assembleDHT22JSON(){
  char msg[DATA_MAX_LENGTH] = {
    '\0'      };
  String temp = "\"sensor_datum\":{\"Temp\":\"";
  temp += (int)dht22.temperature * 10;
  temp += "\", \"Channel\":\"22\", \"DevID\":\"DHT\",";
  temp += "\"humidity\":\"";
  temp += (int)dht22.humidity;
  temp += "\"}";
  return temp;
}

/** Checks to see if the watchdog timer needs to be petted
 * and pets it if enough time has elapsed.    */
void checkNPet(){
  current_time = millis();
  if(current_time - time_last_pet >= 2000){
#ifdef DEVELOPMENT 
    Serial.print(".");
#endif
    tinyWDT.pet();
    time_last_pet = current_time;
  }
}
/** Generates the JSON object that contains the data as well as the building id and the device address. */

void generateDeviceJSON(String sensor_msg, char *msg){
  String js = "{";
  js += sensor_msg;
  js += ",\"building_id\":\"";
  js += building_id;
  js += "\",\"device_address\":\"";
  js += address;
  js += "\"}";
  lcd_print_bottom("Got Message");
  js.toCharArray(msg, js.length()+1);
}

/** Processes the data as it comes from the Manchester Decoder
 * and passes it to the parser to be interpreted. */
void processMessages(){
  while(md.hasNextPulse()){
    uint8_t data = md.getNextPulse();
    //Serial.print(data, HEX);
    // If value indicates timeout then resetParser
    if(data == RESET){
      resetParser();
    } // Otherwise put the data in both parsers
    else{
      char payload[DATA_MAX_LENGTH] = {
        '\0'            };
      if(oscv3.parseOregonScientificV3(data)){
        // Gets the sensor that broad-casted the message and print it.
        lcd_print_top("Got Message");
        generateDeviceJSON(oscv3.getCurrentSensor()->getJSONMessage(), payload);
        assemblePacket(payload);
        lcd_print_top("Sent Message");
        resetParser();
        //readDHT22();
        //generateDeviceJSON(assembleDHT22JSON(), payload);
        //assemblePacket(payload);
      }
      else if(oscv2.parseOregonScientificV2(data)){
        generateDeviceJSON(oscv2.getCurrentSensor()->getJSONMessage(), payload);
        assemblePacket(payload);
        resetParser();
      }
    }
  }
  //Serial.println();
}

/** Performs all of the initializations along with all of the necessary configurations. */
void setup(){
  // Initializes the WildFire
  wf.begin();
  // Configures the WDT and check method
  time_last_pet = 0;
  tinyWDT.begin(1000, 60000);
  lcd.begin(16,2);
  lcd.clear();
  lcd_print_top("Welcome to");
  lcd_print_bottom("Home Monitor");
  delay(1000);
  lcd_print_top("Performing Setup");
#if defined(DEVELOPMENT) || defined(CONFIG)
  Serial.begin(SERIAL_BAUD);
  // Output compile information and server information
  Serial.println(F("Compiled on " __DATE__ ", " __TIME__));
  Serial.println(F("Server is " HOST));
#endif
  for(uint16_t i = 0; i < MAX_PACKET_LENGTH; i++){
    packet_buffer[i] = '\0';
  }
  // IF the connection attempts to the network fail sleep
  if(!connectToNetwork()){
    // TODO put the wildfire to sleep
    while(1);
  }

  checkNPet();

  // Resolve the IP address of the server
  ip = 0;
  while (ip == 0) {

    if (! cc3000.getHostByName(HOST, &ip)) {
#ifdef DEVELOPMENT
      Serial.println(F("Couldn't resolve!"));
#endif
    }
    delay(500);
  }
  checkNPet();
#ifdef DEVELOPMENT
  Serial.println("Resolved the server");
#endif
  // Adds sensors with the appropriate message formats
  oscv2.addSensor(new OregonScientificSensor(THGR122NX, V2_CHANNEL_1, 7, OregonScientificSensor::THGR122NX_FORMAT, OregonScientificSensor::THGR122NX_TITLES));
  oscv2.addSensor(new OregonScientificSensor(THGR122NX, V2_CHANNEL_3, 7, OregonScientificSensor::THGR122NX_FORMAT, OregonScientificSensor::THGR122NX_TITLES));
  oscv3.addSensor(new OregonScientificSensor(THWR800, V2_CHANNEL_1, 6, OregonScientificSensor::THWR800_FORMAT, OregonScientificSensor::THWR800_TITLES));

  lcd_print_top("Listening 492Mhz");
}

/** The main loop that controls the operation of the device by using a state machine. */
void loop(){
  device_states state = PING_SERVER;
#ifdef DEVELOPMENT
  Serial.println("Listening on 433.92Mhz");
#endif
  while(1){
    checkNPet();
    switch(state){
    case PING_SERVER:
      lcd_print_top("Querying Server");
      building_id = getBuilding();
#ifdef DEVELOPMENT
      Serial.print("B_ID");
      Serial.println(building_id);
#endif
      if(building_id > 0){
#ifdef DEVELOPMENT
        Serial.println("Activated");
#endif
        lcd_print_top("Activated");
        state = ACTIVATED;
      }
      else{
        lcd_print_top("Not In Building");
        lcd_print_bottom("Add to Building");
        delay(10000);
      }
      break;
    case ACTIVATED:
      building_id = getBuilding();
      if(building_id < 0){
        state = PING_SERVER;
        lcd_print_top("Deactivated");
      }
      else{
        processMessages();
        delay(10000);
      }
      break;
    case GEN_SENSOR:
    break;
    }
  }
}


