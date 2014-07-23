/** Contains the methods that interface with the server.
 * This includes assembling the HTTP packet that is sent
 * to the server, establishing the connection to the server,
 * and sending the packet with the CC3000. 
 * @file ServerOperations.ino     */

#include <string.h>
#include <stdlib.h>

/** Assembles the HTTP packet that will be sent to the server.
 * @param *data The sensor data that will form the payload of the packet.*/
void assemblePacket(char *data){
  lcd_print_top("Assembling Packet");
  // Clears the packet buffer to make sure that it is not dirty
  clearPacketBuffer();

  // Gets the encryption key 
  char vignere_key[32] = ""; 
  getEncryptionKey(vignere_key);

  // Encrypts the data using the encryption key
  encrypt(data, vignere_key, data);

  // Begins creating parts of the header
  char putstr_buffer[64] = "POST /sensor_data/batch_create/";
  strcat(putstr_buffer,address);
  strcat_P(putstr_buffer, PSTR(".json HTTP/1.1"));

  int additionalCharacters = 17; // the brackets, :, and "s
  //Account for characters that will be escaped
  uint8_t len = strlen(data);
  for(int i=0; i<len; i++) {
    if(data[i] == '\\' || data[i]=='"')
      additionalCharacters++;
  }

  // Completes the header
  makePacketHeader(putstr_buffer, "application/json", len + additionalCharacters);
  strcat_P(packet_buffer, PSTR("\n{\"encrypted\":\""));

  //Copy encrypted text and escape " and \s
  int packetSize = strlen(packet_buffer);
  for(int i=0; i<len; i++) {
    if(data[i] == '"' || data[i] == '\\') {
      packet_buffer[packetSize] = '\\';
      packetSize++;
    }
    packet_buffer[packetSize] = data[i];
    packetSize++;
  }
  packet_buffer[packetSize] = '\0';

  strcat_P(packet_buffer, PSTR("\"}"));

  Serial.println(packet_buffer);
  sendPacket();
}

/** Generates the packet header given the necessary information.
 * @param *request_type_and_location Specifies the HTTP method as well as the URI.
 * @param *mime_type Specifies the type of data that the  packet will be carrying (application-json) etc.
 * @param datalength The length of the data that will be encapsulated in the content section of the packet.
 * @return The packet buffer containing the header.*/
char *makePacketHeader(char *request_type_and_location, char *mime_type, int datalength) {
  char len_buffer[32] = "";
  itoa(datalength, len_buffer, 10);
  //packet_buffer[0] = '\0';
  strcat(packet_buffer, request_type_and_location);
  strcat_P(packet_buffer, PSTR("\nHost: " HOST "\nContent-Type: "));
  strcat(packet_buffer, mime_type);  
  strcat_P(packet_buffer, PSTR("; charset=UTF-8\nContent-Length: "));  
  strcat(packet_buffer, len_buffer);
  strcat_P(packet_buffer, PSTR("\nConnection: close\n")); 

  return packet_buffer;

}

/** Establishes the TCP connection between the device and the server then sends the packet and reads the servers response.
 * @return The whether or not the packet was successfully sent. */
boolean sendPacket() {
  //Creates and sends a packet of data to the server containing CO2 results and timestamps
  Serial.println(F("Sending data..."));
  lcd_print_top("Sending Data");
  WildFire_CC3000_Client client = cc3000.connectTCP(ip, LISTEN_PORT);
  Serial.println("Established TCP Connection");
  Serial.println(F("Connected"));
  lcd_print_bottom("Connected");
#ifdef DEVELOPMENT

#endif

  if (client.connected()) {
    //Send packet
    checkNPet();      
    while(client.available()) { //flushing input buffer, just in case
      Serial.println(client.read());
    }

    client.fastrprintln(packet_buffer);
    Serial.println(F("Printed"));
    //#ifdef DEVELOPMENT

    Serial.println("Outgoing request: ");
    Serial.println(packet_buffer);
    Serial.println();
  }
  //#endif
  Serial.println(F("Packet sent.\nWaiting for response."));
  checkNPet();

  int timeLeft = 6000;
  char headerBuffer[7] = {
    0,0,0,0,0,0,0                  };
  while(timeLeft) {
    if(!strcmp("start\n", headerBuffer) && client.available()) {
      //When the header is over, and there is one character from the actual body
      break;
    }

    if(client.available()) {
      //Add the new character to the end of headerBuffer
      for(int i=0; i<5; i++) {
        headerBuffer[i] = headerBuffer[i+1];
      }
      headerBuffer[5] = client.read();
      Serial.print(headerBuffer[5]);
    } 
    else {
      delay(50);
      timeLeft -= 50;
    }
  } //End ignoring header
  lcd_print_top("Listening 492Mhz");
  checkNPet();
  if(client.read() != 'S') {
    Serial.println(F("Upload failed"));
    //if uploading succeeded, the server will display a page that says "Success uploading data".
    // otherwise, it will show "Failed to upload"
    //On a timeout, client.read() gives -1.
    return false;
  } 
  else {
    Serial.println(F("Upload succeeded"));
  }


  client.close();
  Serial.println("client closed");
  return false;
}

/** Checks whether the device is activated inside a building and if so what building and what sensors does it have. 
 * @return The building id if the device is currently active in a building otherwise it will return -1.*/
int getBuilding() {
  clearPacketBuffer();
  Serial.println(F("Connecting to server...\nIf this is the first time, it may take a while"));
  checkNPet();
  Serial.println("Radio Connected");
  WildFire_CC3000_Client client = cc3000.connectTCP(ip, LISTEN_PORT);
  Serial.println(F("Established TCP Connection"));
  int datalength = 0;
  char data[1] = "";


  //Sending request
  char putstr_buffer[128] = "GET /first_contact/";
  strcat(putstr_buffer, address);
  Serial.print("Address is:");
  Serial.println(address);

  strcat_P(putstr_buffer, PSTR(".html HTTP/1.1"));
  makePacketHeader(putstr_buffer, "application/json", datalength);

  strcat(packet_buffer, data);
  Serial.println(F("Sending request"));
  checkNPet();
  Serial.println(packet_buffer);
  if(client.connected()){
    client.fastrprintln(packet_buffer);
  }
  else{
    Serial.println("Error");
  }
  Serial.print("Address is: ");
  Serial.println(address);
  ///Receiving reply
  checkNPet();

  char serverReply[512] = "";

  Serial.println(F("Getting Server reply"));

  //Ignoring the header:
  int i = 0;
  while(client.connected() && i < 511) {
    while(client.available()) {
      Serial.print('*');
      checkNPet();
      serverReply[i] = (char)client.read();
      i++;
      serverReply[i] = '\0';
      if(i >= 5 && !strcmp("start", serverReply+i-5)) {
        break;
      }
    }


    if(i >= 5 && !strcmp("start", serverReply+i-5)) {
      break;
    }
  }


  //Reading the body
  i=0;
  while(client.connected() && i < 511){
    Serial.print('.');
    if(client.available()) {
      checkNPet();
      serverReply[i] = (char)client.read();
      i = (i == 0 && ( serverReply[0] == ' ' || serverReply[0] == '\n') ) ?  i : i+1;
    } 
    else {
      //delay(50);
    }

    if(i >= 3 && !strcmp("end", serverReply+i-3)) {
      i -= 3;
      break;
    }
  }
  serverReply[i] = '\0';

#ifdef DEVELOPMENT
  /* Serial.println("\nPacket to server:");
   Serial.println(packet_buffer);
   Serial.println("ServerReply:");
   Serial.println(serverReply);*/
#endif

  //Decoding server reply
  char vignere_key[32] = ""; 
  getEncryptionKey(vignere_key);
  decrypt(serverReply, vignere_key, serverReply);
  Serial.println();
  Serial.println(serverReply);

  long int time;
  int experiment_id_tmp, CO2_cutoff_tmp;
  int varsRead = sscanf(serverReply, "%ld %*s %d %*s %d", &time, &experiment_id_tmp, &CO2_cutoff_tmp);

  client.close();

  switch(varsRead){
  case 1:
    return -1;
    break;
  case 2:
  case 3:
    return experiment_id_tmp;
    break;
  default:
    return -1;
  } 
}

/** The helper function used to clear the packet buffer to ensure that there is no chance of buffer overflow.*/
void clearPacketBuffer(){
  for(int i = 0; i < strlen(packet_buffer); i++){
    packet_buffer[i] = '\0';
  }
}
