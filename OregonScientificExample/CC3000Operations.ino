/** CC3000 Operations contains the configuration settings
 * of the CC3000 as well as the methods for using it to
 * connect to the desired network. This includes the
 * ability to connect or reconnect using SmartConfig.
 * @file CC3000Operations.ino */

/** Controls the connection to the network
 * including getting input from the user;
 * whether that is from the Serial port or from
 * a button press. 
 * returns: Whether the connection succeeded.*/
boolean connectToNetwork(){
  checkNPet();
#ifdef CONFIG 
  configure();
#endif
  // Tries to pet the watchdog
  boolean valid = false;
  // Loops while the not connected
  while(!valid){
    checkNPet();
    // Outputs message for user
    lcd_print_top("Press button for");
    lcd_print_bottom("SmartConfig");
#ifdef DEVELOPMENT
    Serial.println(F("Select an option."));
    Serial.println(F("\t(1) SmartConfig Create"));
    Serial.println(F("\t(2) SmartConfig Reconnect"));
#else
    if(getButtonPress(USER_TIMEOUT)){
      valid = smartConfigCreate();
    }
    else{
      valid = smartConfigReconnect();
    }
#endif
    uint8_t numTrys = 0;
    // While the user has not entered input wait until timeout
#ifdef DEVELOPMENT
    while(!Serial.available()){
      if(numTrys > 40){
        Serial.println("Timeout");
        return false;
      }
      // Keeps the watchdog from biting
      checkNPet();
      delay(200);
      numTrys++;
    }

    // Read the user input
    char result = Serial.read();
    // Reacts to user input
    switch(result){
    case '1':
      return smartConfigCreate();
    case '2':
      return smartConfigReconnect();
    default: 
      valid = false;
    }
#else
    return valid;
#endif
  }
}

/** Initializes the address variable by reading
 * the MAC address from the CC3000. One consideration
 * must be made in that this function should not be
 * called before the CC3000 has been initialized by
 * calling the begin() function. */
void initMAC(){
#if defined(DEVELOPMENT) || defined(CONFIG)
  Serial.print(F("Finding mac address ."));
#endif
  uint8_t addr[6];
  if(cc3000.getMacAddress(addr)){
    mactoaddr(addr, address);
  }
  else{
#if defined(DEVELOPMENT) || defined(CONFIG)
    Serial.println("Failed");
#endif
  }
#if defined(DEVELOPMENT) || defined(CONFIG)
  Serial.println(address);
#endif
}

/** Configures the device by setting the encryption key and printing the devices
 * MAC address so that it can be entered into the website for activation.
 * This function should only be called when the config environment
 * is defined. This should only occur when the device is first being
 * programmed by setting the encryption key and accessing the devices
 * MAC address. */
void configure(){
  Serial.println(F("Configuration started.\nPlease make sure you are using a serial mode with newlines."));
  // Checks for a valid previously stored encryption key
  if(validEncryptionKey()){
    // If one is found it will inform the user and verify that they want to keep it
    Serial.println(F("\nOld encryption key found:"));
    char buffer[32] = "";
    getEncryptionKey(buffer);
    Serial.println(buffer);
    Serial.println(F("Overwrite? y/n"));
  } 
  // Otherwise it will check if they want to create a new one
  else {
    Serial.println(F("\nEncryption key not found, make a new one? y/n"));
  }
  checkNPet();
  while(!Serial.available()) { 
    delay(100);
  }

  checkNPet();
  char yesNo = Serial.read();
  Serial.read(); //Get rid of newline
  if(yesNo == 'Y' || yesNo == 'y') {
    checkNPet();
    setEncryptionKeyBySerial();
  }

  Serial.println(F("Configuration finished, restarting."));
  tinyWDT.force_reset();
}

/** Attempts the SmartConfig Create feature of the CC3000. */
boolean smartConfigCreate(){
#ifdef DEVELOPMENT
  Serial.println(F("\nInitializing the CC3000"));
#endif
  lcd_print_top("Enabling WiFi");
  // Initializes the CC3000
  if (!cc3000.begin(false))
  {
#ifdef DEVELOPMENT
    Serial.println("Enable Failed");
#endif
    lcd_print_bottom("Failed");
    return false;
  }

#ifdef DEVELOPMENT
  /* Try to use the smart config app (no AES encryption), saving */
  /* the connection details if we succeed. */
  Serial.println(F("Waiting for a SmartConfig connection (~60s) ..."));
#endif
  lcd_print_top("SmartConfig");
  lcd_print_bottom("Open App (~60s)");
  // Begins the smart config process
  if (!cc3000.startSmartConfig("CC3000"))
  {
#ifdef DEVELOPMENT
    Serial.println(F("SmartConfig failed"));
#endif
    lcd_print_bottom("Failed!");
    return false;
  }
  checkNPet();
  Serial.println(F("SmartConfig Success! AP connection details were saved"));
  lcd_print_bottom("Succeeded!");
#ifdef PRODUCTION
  delay(1000);
#endif
  uint16_t time = millis();

#ifdef DEVELOPMENT
  Serial.println(F("Request DHCP"));
#endif
  lcd_print_top("Requesting DHCP");
  // Requests DHCP
  while (!cc3000.checkDHCP()){
    checkNPet();
    if (millis() - time > DHCP_TIMEOUT) {
      time = 0;
#ifdef DEVELOPMENT
      Serial.println(F("DHCP failed!"));
#endif
      lcd_print_bottom("DHCPFailed!");
      return false;
    }
  }
#ifdef DEVELOPMENT
  Serial.println(F("DHCP Succeeded"));
#endif
  lcd_print_bottom("Succeeded!");
  // Prints out the connection details
  while(!displayConnectionDetails()){
    delay(1000);
  }
  // Initializes the MAC address
  //initMAC();
  return true;
}

/** Displays the connection details
 * when the program is compiled in development mode. */
boolean displayConnectionDetails(void) {
  uint32_t addr, netmask, gateway, dhcpserv, dnsserv;

  if(!cc3000.getIPAddress(&addr, &netmask, &gateway, &dhcpserv, &dnsserv))
    return false;
#if defined(DEVELOPMENT) || defined(CONFIG)
  Serial.print(F("IP Addr: ")); 
  cc3000.printIPdotsRev(addr);
  Serial.print(F("\r\nNetmask: ")); 
  cc3000.printIPdotsRev(netmask);
  Serial.print(F("\r\nGateway: ")); 
  cc3000.printIPdotsRev(gateway);
  Serial.print(F("\r\nDHCPsrv: ")); 
  cc3000.printIPdotsRev(dhcpserv);
  Serial.print(F("\r\nDNSserv: ")); 
  cc3000.printIPdotsRev(dnsserv);
  Serial.println();
#endif
  return true;
}


/** Attempts to reconnect to the previously used network. */
boolean smartConfigReconnect(){
#ifdef DEVELOPMENT
  Serial.println(F("Attempting SmartConfig Reconnect"));
#endif
  lcd_print_top("Reconnecting");
  // Attempts to initialize the CC3000 and reconnect
  if (!cc3000.begin(false, true, "CC3000")){
#ifdef DEVELOPMENT
    Serial.println(F("Unable to re-connect!? Try Running SmartConfig Create"));
#endif
    lcd_print_top("Reconnect Failed");
    lcd_print_bottom("Try SmartConfig");
    return false;
  }
  // Initializes the MAC address

  lcd_print_bottom("Reconnected");
#ifdef PRODUCTION
  delay(1000);
#endif
#ifdef DEVELOPMENT
  Serial.println(F("Reconnected!"));
  // Wait for DHCP to complete
  Serial.println(F("\nRequesting DHCP"));
#endif
  lcd_print_bottom("Requesting DHCP");

  // Requests DHCP
  uint16_t time = millis();
  while (!cc3000.checkDHCP()) {
    if (millis()-time > DHCP_TIMEOUT) {
      lcd_print_bottom("DHCP Failed");
#ifdef DEVELOPMENT
      Serial.println(F("DHCP failed!"));
#endif
      return false;
    }
  }
  lcd_print_bottom("DHCP Succeeded");
#ifdef DEVELOPMENT
  Serial.println(F("DHCP Succeeded"));

  // Displays the connection details
  while(!displayConnectionDetails()){
    delay(1000);
  }
  //initMAC();
#endif
  return true;
}

/***************************Helper Functions***********************************/

/** Converts uint32t MAC address to its ASCII representation.
 * @param ip The numeric value of the MAC address that will be returned by the CC3000.
 * @param *string The buffer that will be used to hold the ASCII representation of the MAC address.
 */
void mactoaddr(uint8_t ip[], char *string) {
  //Mac address to ascii
  uint8_t idx = 0;
  for(uint8_t i = 0; i < 6; i++) {
    // i*2 is used more than once so only compute it once
    idx = i * 2;
    string[idx] = to_hex(ip[i] >> 4);
    string[idx + 1] = to_hex(ip[i]);
  }
  string[12] = '\0';
}

/** Converts uint8_t to hex char representation 
 * @param value The number to be converted to ASCII HEX
 * @return The ASCII representation of the HEX number value
 */
char to_hex(uint8_t value) {
  value &= 0xF;
  // If it is greater than 9 add 55
  // which will make 10 => 'A' ...
  if (value > 9) {
    return (value + 55);
  } 
  // Otherwise 0 => '0'
  else {
    return (value + '0');
  }
}

/** Polls for a button press while updating the LCD.
 * start: The value from which to count down.
 * @param start The value at which to start counting down from.
 * @return Whether the button was pressed at some point during the allotted time.  */
boolean getButtonPress(int start){
  // Configures the pin connected to the button
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, INPUT_PULLUP);
  // Initializes the variables
  int thistime = millis();
  int lasttime = millis();
  // Counts down by one every second
  for(int i = start; i >= 0; i--){
    lcd_print_countdown(i);
    // Stops polling every second to update the LCD
    while(lasttime + 1000 > thistime){
      thistime = millis();
      // Checks to see if the button was pressed
      if(digitalRead(BUTTON_PIN) == 0){
        // Waits to debounce the button
        delay(200);
        if(digitalRead(BUTTON_PIN) == 0){
          return true;
        }
      }
    }
    lasttime = thistime;
  }
  return false;
}

/** Gets a line of input from the serial port stopping at the desired delimiter.
 * @param *data A char buffer that will hold the input.
 * @param len The max length of the buffer.
 * @param delim The desired delimiter.     */
void getSerialInput(char *data, uint16_t len, char delim){
  // Gets rid of junk input etc. the newline char
  while(Serial.available()){
    Serial.read();
  }
  // Waits for valid input
  while(!Serial.available()){
    delay(200);
  }
  char temp = Serial.read();
  uint16_t read_len = 0;
  // Reads in data until it finds a newline
  while(temp != delim && read_len < len){
    data[read_len] = temp;
    temp = Serial.read();
    read_len++;
  }
  return;
}

