// ###########################################################################################################################################
// #
// # Code for "7 segment LED clock" project: https://www.printables.com/de/model/325415-7-segment-led-clock
// #
// # Code by https://github.com/AWSW-de
// #
// # Released under license: GNU General Public License v3.0: https://github.com/AWSW-de/7-segment-LED-clock/blob/main/LICENSE
// #
// # Compatible with version: V1.0.1
// # 
// ###########################################################################################################################################
/*
 ________                                                        __             
/\_____  \                                                      /\ \__          
\/___//'/'         ____     __     __     ___ ___      __    ___\ \ ,_\         
    /' /'         /',__\  /'__`\ /'_ `\ /' __` __`\  /'__`\/' _ `\ \ \/         
  /' /'          /\__, `\/\  __//\ \L\ \/\ \/\ \/\ \/\  __//\ \/\ \ \ \_        
 /\_/            \/\____/\ \____\ \____ \ \_\ \_\ \_\ \____\ \_\ \_\ \__\       
 \//              \/___/  \/____/\/___L\ \/_/\/_/\/_/\/____/\/_/\/_/\/__/       
                                   /\____/                                      
                                   \_/__/                                       
 __       ____    ____                ___                   __                  
/\ \     /\  _`\ /\  _`\             /\_ \                 /\ \                 
\ \ \    \ \ \L\_\ \ \/\ \        ___\//\ \     ___     ___\ \ \/'\             
 \ \ \  __\ \  _\L\ \ \ \ \      /'___\\ \ \   / __`\  /'___\ \ , <             
  \ \ \L\ \\ \ \L\ \ \ \_\ \    /\ \__/ \_\ \_/\ \L\ \/\ \__/\ \ \\`\           
   \ \____/ \ \____/\ \____/    \ \____\/\____\ \____/\ \____\\ \_\ \_\         
    \/___/   \/___/  \/___/      \/____/\/____/\/___/  \/____/ \/_/\/_/         

*/


// ###########################################################################################################################################
// # Hardware settings:
// ###########################################################################################################################################
#define AP_TIMEOUT 240                      // Timeout in seconds for AP / WLAN config
#define PIN D3                              // Arduino-Pin connected to the NeoPixels
#define NUMPIXELS 30                        // How many NeoPixels are attached to the Arduino?


// ###########################################################################################################################################
// # LED defaults:
// ###########################################################################################################################################
int redVal   = 0;                            // Default color RED
int greenVal = 255;                          // Default color GREEN
int blueVal  = 128;                          // Default color BLUE
int intensity = 64;                          // LED intensity (0..255) in day mode   - Important note: Check power consumption and used power supply capabilities!
int intensityNight = 21;                     // LED intensity (0..255) in night mode - Important note: Check power consumption and used power supply capabilities!


// ###########################################################################################################################################
// # Various default settings:
// ###########################################################################################################################################
int switchLangWeb = 0;                      // Language default setting (0 = german, 1 = english, 2...x = for future use)
int wifireset = 0;                          // WiFi Reset switch (not stored in EEPROM)
int clockreset = 0;                         // WordClock Reset switch (not stored in EEPROM)
int useledtest = 1;                         // Show start animation and display test at boot
int showDate = 0;                           // Show date when seconds = 30 ? - Note: Any value <> 0 shows the date when second = 30
int server1port = 55555;                    // REST function web server port
int useupdate = 1;                          // Use the software update function of the device


// ###########################################################################################################################################
// # Night mode option defaults: 
// ###########################################################################################################################################
int displayoff = 0;                         // Use the night mode function to set the LEDs to off
int useNightLEDs = 0;                       // Show LEDs in night mode with reduced intensity
int displayonmaxMO = 22;                    // Display off from 22:00 o'clock Monday
int displayonminMO = 6;                     // Display on after 06:59 o'clock Monday
int displayonmaxTU = 22;                    // Display off from 22:00 o'clock Tuesday
int displayonminTU = 6;                     // Display on after 06:59 o'clock Tuesday
int displayonmaxWE = 22;                    // Display off from 22:00 o'clock Wednesday
int displayonminWE = 6;                     // Display on after 06:59 o'clock Wednesday
int displayonmaxTH = 22;                    // Display off from 22:00 o'clock Thursday
int displayonminTH = 6;                     // Display on after 06:59 o'clock Thursday
int displayonmaxFR = 23;                    // Display off from 23:00 o'clock Friday
int displayonminFR = 6;                     // Display on after 06:59 o'clock Friday
int displayonmaxSA = 23;                    // Display off from 23:00 o'clock Saturday
int displayonminSA = 7;                     // Display on after 06:59 o'clock Saturday
int displayonmaxSU = 22;                    // Display off from 22:00 o'clock Sunday
int displayonminSU = 6;                     // Display on after 06:59 o'clock Sunday


// ###########################################################################################################################################
// # Variables declaration:
// ###########################################################################################################################################
#define DEFAULT_AP_NAME "7segment"               // WiFi access point name of the ESP8266
String wchostname = "7segment";                  // Hostname to be set in your router
int wchostnamenum = 0;                           // Hostname + Number used as suffix to the host name
ESP8266WebServer httpServer(2022);               // Update server port


// ###########################################################################################################################################
// # NTP time server settings:
// ###########################################################################################################################################
#define DEFAULT_NTP_SERVER "de.pool.ntp.org"
#define DEFAULT_TIMEZONE   "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define TZ_WEB_SITE        "http://www.hs-help.net/hshelp/gira/v4_7/de/proj_tz.html"


// ###########################################################################################################################################
// # Internal web server settings:
// ###########################################################################################################################################
WiFiServer server(80);                      // Set web server port number to 80


// ###########################################################################################################################################
// # EOF - You have successfully reached the end of the code - well done ;-)
// ###########################################################################################################################################
