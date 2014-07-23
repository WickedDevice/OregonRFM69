/** Contains the encryption methods that are used to secure the
 * data as it is transmitted over the network. It also contains
 * a function to set the encryption key via the serial port. 
 * @file encryption.ino     */

/** 
 * The encryption method implementing a Vignere cipher
 * @param *plaintext The plaintext data that will be encrypted.
 * @param *key The key that will be used by the cipher to encrypt the data.
 * @param *encrypted The buffer in which the resulting encrypted data will be placed. */
void encrypt(char * plaintext, char * key, char *encrypted) {
  int textLength = strlen(plaintext);
  int keyLength =  strlen(key);
  for(int i=0; i < textLength; i++) {
    encrypted[i] = plaintext[i] + key[i % keyLength] - 32;
    if((unsigned) encrypted[i] >= 127) {
      encrypted[i] -= (unsigned) (127-32);
    }
  }
}

/**
 * The decryption method for returning the data back to plaintext.
 * @param *encrypted The encrypted data.
 * @param *key The encryption key that will be used to decrypt the cipher text.
 * @param *plaintext The buffer in which the resulting decrypted data will be placed. */
void decrypt(char *encrypted, char * key, char *plaintext) {
  int textLength = strlen(encrypted); 
  int keyLength =  strlen(key);
  for(int i=0; i < textLength; i++) {
    plaintext[i] = encrypted[i] - (key[i % keyLength] - 32);
    if(plaintext[i] < 32 || plaintext[i] >= 127) {
      plaintext[i] += (127-32);
    }
  }
}

/** Sets the encryption key that will be used by the encryption and decryption methods. */
void setEncryptionKeyBySerial() {
  Serial.println(F("\nPlease type in new encryption key. (<32 characters)"));
  
  boolean done = false;
  char buffer[32] = "";
  int index = 0;
  char c = ' ';  //' ' is an arbitrary value
   
  while(c != '\n' && c != '\0') { //Until newline
    checkNPet();
    while(Serial.available()) {
      c = Serial.read();
      if(c == '\n') {
        buffer[index] = '\0';
        break;
      }
      buffer[index] = c;
      index++;
    }
  }
  
  Serial.println(F("Your new encryption key is:"));
  Serial.println(buffer);
  Serial.println(F("Is this OK? y/n"));
  while(!Serial.available()){}
  c = Serial.read();
  if(c == 'Y' || c == 'y') {
    setEncryptionKey(buffer);
    Serial.println(F("Key saved."));
  } else {
    while(Serial.available())
    { Serial.read(); }
    setEncryptionKeyBySerial();
  }
}

/*
//Function that tests encryption
int main() {
  char input[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
  
  char key[2] = "";
  int len = strlen(input);
  char encrypted[len];
  char output[len];
  
  bool success = true;
  //Use ever character as the key
  for(char c = ' '; c <= '~'; c++) {
    key[0] = c;
    encrypt(input,key,encrypted);
    decrypt(encrypted,key,output);
  
    //printf("%s\n%s\n", input, output);
    //printf("%s\n", encrypted);
  
    //Check for successful decryption
    for(int i=0; i<len; i++) {
      if(input[i] != output[i]) {
        success = false;
        printf("Key:%c %d:%d ", key[0], input[i], output[i]);
      }
    }
  }
  if(success) {
    printf("Encryption worked properly");
  } 
}
*/

