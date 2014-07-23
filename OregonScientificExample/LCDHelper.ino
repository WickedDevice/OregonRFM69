/** Contains the helper functions for the LCD.
 * @file LCDHelper.ino */


/** Clears the entire screen then prints the specified string on the top line.
 * @param message The message to be displayed on the top line of the screen. */
void lcd_print_top(char* message) {
  lcd.clear();
  lcd.print(message);
}

/** Clears the bottom line of the LCD screen before printing the desired message there.
 * @param message The message to be printed on the bottom line of the LCD. */
void lcd_print_bottom(char* message) {
  // Clears the bottom row of the lcd
  lcd.setCursor(0,1);
  lcd.print("                ");
  // Prints the message
  lcd.setCursor(0,1);
  lcd.print(message);
}

/** Prints a countdown in the bottom left corner of the LCD display.
 * @param val The value to be displayed in the bottom left corner. */
void lcd_print_countdown(uint16_t val){
  uint8_t cursor_pos;
  if(val > 9){
    cursor_pos = 14;
  }
  else if(val > 100){
    cursor_pos = 13;
  }
  else{
    cursor_pos = 15;
  }
  lcd.setCursor(12, 1);
  lcd.print("    ");
  lcd.setCursor(cursor_pos, 1);
  lcd.print(val);
}

/** A helper function that will print the DHT22 temperature and humidity to the LCD.
 * @param temp The temperature provided by the DHT22.
 * @param humid The humidity provided by the DHT22.*/
void lcd_print_dht22(double temp, double humid){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print("Humid: ");
  lcd.print(humid);
}

