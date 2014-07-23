/** Contains the definitions that are required for the program.
 * These allow the functionality of the program to be modified
 * in a simple manner. This includes the ability to change pin
 * mappings, environments, and several other useful parameters. 
 * @file header.h 		*/

//#define DEVELOPMENT 	///< Defined for the development environment
#define PRODUCTION 		///< Defined for the production environment
//#define CONFIG			///< Defined for the initial configuration

#define PACKET_SIZE 50 ///< The size of the individual packets                    //Number of datapoints in a packet
// Don't make this too big or the Wildfire won't have space for it
// This shouldn't cause packets to exceed TX_BUFFER_SIZE for the CC3000

#define DATA_MAX_LENGTH (PACKET_SIZE * 35 + 150) ///< The maximum length of the data
#define MAX_PACKET_LENGTH (160 + DATA_MAX_LENGTH + 64) ///< The maximum packet length - used when allocating the packet buffer

#define SERIAL_BAUD 115200 ///< The Baud Rate of the Serial port 
#define LISTEN_PORT 3000  ///< The port on which the server listens
#define IDLE_TIMEOUT_MS  3000 ///< The HTTP timeout (in milliseconds)
#define DHT22_PIN A0	///< The input from the DHT22


#define HOST      "192.168.1.16" ///< The Ruby on Rails host.

#define DHCP_TIMEOUT 10000   ///< DHCP timeout (in milliseconds).

#define USER_TIMEOUT 5 ///< The time to wait for user input during setup. (in seconds)

#define BUTTON_PIN A3	///< The input pin connected to the button.



/****LCD Defines****************/
#define LCD_RS A2 	///< The pin used for the Read Select line for the LCD
#define LCD_E  A1	///< The pin used for the Enable line 
#define LCD_D4  4	///< The pin used for the data bus line 4
#define LCD_D5  5 	///< The pin used for the data bus line 5
#define LCD_D6  6 	///< The pin used for the data bus line 6
#define LCD_D7  8 	///< The pin used for the data bus line 7
/******************************/
//#define CONFIG
