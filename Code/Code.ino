// ###########################################################################################################################################
// #
// # Code for "7 segment LED clock" project: https://www.printables.com/de/model/325415-7-segment-led-clock
// #
// # Code by https://github.com/AWSW-de
// #
// # Released under license: GNU General Public License v3.0: https://github.com/AWSW-de/7-segment-LED-clock/blob/main/LICENSE
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
// # Includes:
// #
// # You will need to add the following libraries to your Arduino IDE to use the project:
// # - Adafruit NeoPixel              // by Adafruit:                     https://github.com/adafruit/Adafruit_NeoPixel
// # - WiFiManager                    // by tablatronix / tzapu:          https://github.com/tzapu/WiFiManager
// #
// ###########################################################################################################################################
#include <ESP8266WiFi.h>              // Used to connect the ESP8266 NODE MCU to your WiFi
#include <DNSServer.h>                // Used for name resolution for the internal webserver
#include <ESP8266WebServer.h>         // Used for the internal webserver
#include <WiFiManager.h>              // Used for the WiFi Manager option to be able to connect the Clock to your WiFi without code changes
#include <EEPROM.h>                   // Used to store the in the internal configuration page set configuration on the ESP internal storage
#include <Adafruit_NeoPixel.h>        // Used to drive the NeoPixel LEDs
#include <time.h>                     // Used to get the time from the internet
#include <Wire.h>                     // Used to connect the RTC board
#include <ESP8266mDNS.h>              // Used for the internal update function
#include <ESP8266httpUpdate.h>        // Used for the http update function
#include <ESP8266HTTPUpdateServer.h>  // Used for the internal update function
#include "settings.h"                 // Settings are stored in a seperate file to make to code better readable and to be able to switch to other settings faster
#include "languages.h"                // Translation for texts for the HTML page


// ###########################################################################################################################################
// # Version number of the code:
// ###########################################################################################################################################
const char* CLOCK_VERSION = "V1.0.0";
// NOTE: Very 1st version... Needs a lot of cleanup and integration of some funcions... Basic function seems to be OK.


// ###########################################################################################################################################
// # Declartions and variables used in the functions:
// ###########################################################################################################################################
String header;                                                                       // Variable to store the HTTP request
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  // LED strip settings
int delayval = 250;                                                                  // delay in milliseconds
int iYear, iMonth, iDay, iHour, iMinute, iSecond, iWeekDay;                          // variables for RTC-module read time:
String timeZone = DEFAULT_TIMEZONE;                                                  // Time server settings
String ntpServer = DEFAULT_NTP_SERVER;                                               // Time server settings
String UpdatePath = "-";                                                             // Update via Hostname
String UpdatePathIP = "-";                                                           // Update via IP-address
ESP8266HTTPUpdateServer httpUpdater;                                                 // Update server
std::unique_ptr<ESP8266WebServer> server1;                                           // REST function web server
bool RESTmanLEDsON = true;                                                           // Global flag to turn LEDs manually on or off - Used for the REST function


// ###########################################################################################################################################
// # Parameter record to store to the EEPROM of the ESP:
// ###########################################################################################################################################
struct parmRec {
  int pRed;
  int pGreen;
  int pBlue;
  int pIntensity;
  int pIntensityNight;
  int pShowDate;
  char pdisplayonmaxMO;
  char pdisplayonminMO;
  char pdisplayonmaxTU;
  char pdisplayonminTU;
  char pdisplayonmaxWE;
  char pdisplayonminWE;
  char pdisplayonmaxTH;
  char pdisplayonminTH;
  char pdisplayonmaxFR;
  char pdisplayonminFR;
  char pdisplayonmaxSA;
  char pdisplayonminSA;
  char pdisplayonmaxSU;
  char pdisplayonminSU;
  int pdisplayoff;
  int puseNightLEDs;
  int puseupdate;
  int puseledtest;
  int pswitchLangWeb;
  int pwchostnamenum;
  char pTimeZone[50];
  char pNTPServer[50];
  int pCheckSum;  // This checkSum is used to find out whether we have valid parameters
} parameter;


// ###########################################################################################################################################
// # Setup function that runs once at startup of the ESP:
// ###########################################################################################################################################
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("######################################################################");
  Serial.print("# Clock startup of version: ");
  Serial.println(CLOCK_VERSION);
  Serial.println("######################################################################");
  dunkel();                         // Switch display black
  pixels.begin();                   // Init the NeoPixel library
  readEEPROM();                     // get persistent data from EEPROM
  pixels.setBrightness(intensity);  // Set LED brightness
  DisplayTest();                    // Perform the LED test
  WIFI_login();                     // WiFiManager
  showIP();                         // Display the current IP-address

  MDNS.begin(wchostname + wchostnamenum);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 2022);
  MDNS.addService("http", "tcp", 80);
  UpdatePath = "http://" + String(wchostname + wchostnamenum) + ".local:2022/update";
  UpdatePathIP = "http://" + WiFi.localIP().toString() + ":2022/update";
  server.begin();

  // REST function web server:
  server1.reset(new ESP8266WebServer(WiFi.localIP(), server1port));
  server1->on("/", handleRoot);
  //server1->on("/ledson", ledsON);
  //server1->on("/ledsoff", ledsOFF);
  //server1->on("/ledstatus", LedStatus);
  server1->on("/clockrestart", ClockRestart);
  server1->on("/clockwifireset", ClockWifiReset);
  server1->begin();

  setLanguage(switchLangWeb);  // Load set language

  Serial.println("######################################################################");
  Serial.println("# Clock startup finished...");
  Serial.println("######################################################################");
  Serial.print("# Configuration page now available: ");
  Serial.println("http://" + WiFi.localIP().toString());
  Serial.println("######################################################################");
}


// ###########################################################################################################################################
// # Loop function which runs all the time after the startup was done:
// ###########################################################################################################################################
void loop() {
  //ESP.wdtFeed();  // Reset watchdog timer

  // Check WiFi connection and reconnect if needed:
  // if (WiFi.status() != WL_CONNECTED) {
  //   WIFI_login();  // WiFi inactive --> Reconnect to it...
  // } else {         // Wifi active --> Sort some atoms in the univers and show the time...
  // Check, whether something has been entered on Config Page
  checkClient();
  //ESP.wdtFeed();  // Reset watchdog timer
  handleTime();  // handle NTP / RTC time
  delay(750);

  redVal = redVal;
  greenVal = greenVal;
  blueVal = blueVal;

  /* // Show the display only during the set Min/Max time if option is set
    if (displayoff) {
      switch (iWeekDay) {
        case 0:  // Sunday
          DayNightMode(displayonminSU, displayonmaxSU);
          break;
        case 1:  // Monday
          DayNightMode(displayonminMO, displayonmaxMO);
          break;
        case 2:  // Tuesday
          DayNightMode(displayonminTU, displayonmaxTU);
          break;
        case 3:  // Wednesday
          DayNightMode(displayonminWE, displayonmaxWE);
          break;
        case 4:  // Thursday
          DayNightMode(displayonminTH, displayonmaxTH);
          break;
        case 5:  // Friday
          DayNightMode(displayonminFR, displayonmaxFR);
          break;
        case 6:  // Saturday
          DayNightMode(displayonminSA, displayonmaxSA);
          break;
      }
    } else {*/
  pixels.setBrightness(intensity);  // DAY brightness
  //}

  // Web update start:
  httpServer.handleClient();
  //MDNS.update();

  //if (LEDsON == true && RESTmanLEDsON == true) pixels.show();  // This sends the updated pixel color to the hardware.
  // }

  // REST function web server:
  server1->handleClient();
}


// ###########################################################################################################################################
// # Try to read settings from FLASH - initialize if WLAN ID read from flash is invalid:
// ###########################################################################################################################################
void readEEPROM() {
  //Serial.print("Copy ");
  //Serial.print(sizeof(parameter));
  //Serial.println(" bytes from flash memory to EPROM buffer: ");

  // initialize space to read parameter
  EEPROM.begin(sizeof(parameter));
  delay(10);

  // Copy bytes from EEPROM
  byte* p = (byte*)(void*)&parameter;

  int check = 0;
  for (int L = 0; L < sizeof(parameter); ++L) {
    byte b = EEPROM.read(L);

    // Don't take the checkSum itself into account
    if (L < sizeof(parameter) - sizeof(parameter.pCheckSum))
      check = check + b;

    *p++ = b;
    /*  Serial.print("Read FLASH Byte ");
      Serial.print(L);
      Serial.print(" = ");
      Serial.println(b);  */
  }

  // Check checksum
  //Serial.print("Compare checksums: ");
  //Serial.print(check);
  //Serial.print("/");
  //Serial.println(parameter.pCheckSum);

  if (check == parameter.pCheckSum) {
    // Serial.println("Checksum does match. Get parameter values from EEPROM");
    redVal = parameter.pRed;
    greenVal = parameter.pGreen;
    blueVal = parameter.pBlue;
    showDate = parameter.pShowDate;
    displayoff = parameter.pdisplayoff;
    useNightLEDs = parameter.puseNightLEDs;
    displayonmaxMO = parameter.pdisplayonmaxMO;
    displayonminMO = parameter.pdisplayonminMO;
    displayonmaxTU = parameter.pdisplayonmaxTU;
    displayonminTU = parameter.pdisplayonminTU;
    displayonmaxWE = parameter.pdisplayonmaxWE;
    displayonminWE = parameter.pdisplayonminWE;
    displayonmaxTH = parameter.pdisplayonmaxTH;
    displayonminTH = parameter.pdisplayonminTH;
    displayonmaxFR = parameter.pdisplayonmaxFR;
    displayonminFR = parameter.pdisplayonminFR;
    displayonmaxSA = parameter.pdisplayonmaxSA;
    displayonminSA = parameter.pdisplayonminSA;
    displayonmaxSU = parameter.pdisplayonmaxSU;
    displayonminSU = parameter.pdisplayonminSU;
    wchostnamenum = parameter.pwchostnamenum;
    useledtest = parameter.puseledtest;
    switchLangWeb = parameter.pswitchLangWeb;
    intensity = parameter.pIntensity;
    intensityNight = parameter.pIntensityNight;
    String ntp(parameter.pNTPServer);
    ntpServer = ntp;
    String tz(parameter.pTimeZone);
    timeZone = tz;
  } else {
    Serial.println("Checksum does not match. New program version or new installed ESP detected...");
  }
}


// ###########################################################################################################################################
// # Write current parameter settings to flash:
// ###########################################################################################################################################
void writeEEPROM() {
  //Serial.println("Write parameter into EEPROM");
  parameter.pRed = redVal;
  parameter.pGreen = greenVal;
  parameter.pBlue = blueVal;
  parameter.pIntensity = intensity;
  parameter.pIntensityNight = intensityNight;
  ntpServer.toCharArray(parameter.pNTPServer, sizeof(parameter.pNTPServer));
  timeZone.toCharArray(parameter.pTimeZone, sizeof(parameter.pTimeZone));
  parameter.pShowDate = showDate;
  parameter.pdisplayoff = displayoff;
  parameter.puseNightLEDs = useNightLEDs;
  parameter.pdisplayonmaxMO = displayonmaxMO;
  parameter.pdisplayonminMO = displayonminMO;
  parameter.pdisplayonmaxTU = displayonmaxTU;
  parameter.pdisplayonminTU = displayonminTU;
  parameter.pdisplayonmaxWE = displayonmaxWE;
  parameter.pdisplayonminWE = displayonminWE;
  parameter.pdisplayonmaxTH = displayonmaxTH;
  parameter.pdisplayonminTH = displayonminTH;
  parameter.pdisplayonmaxFR = displayonmaxFR;
  parameter.pdisplayonminFR = displayonminFR;
  parameter.pdisplayonmaxSA = displayonmaxSA;
  parameter.pdisplayonminSA = displayonminSA;
  parameter.pdisplayonmaxSU = displayonmaxSU;
  parameter.pdisplayonminSU = displayonminSU;
  parameter.pwchostnamenum = wchostnamenum;
  parameter.puseledtest = useledtest;
  parameter.pswitchLangWeb = switchLangWeb;
  // calculate checksum
  byte* p = (byte*)(void*)&parameter;
  parameter.pCheckSum = 0;
  for (int L = 0; L < sizeof(parameter) - sizeof(parameter.pCheckSum); ++L) {
    byte b = *p++;
    parameter.pCheckSum = parameter.pCheckSum + b;
  }
  // Write data to EEPROM
  p = (byte*)(void*)&parameter;
  for (int L = 0; L < sizeof(parameter); ++L) {
    byte b = *p++;
    EEPROM.write(L, b);
    // Serial.print("Write FLASH Byte ");
    // Serial.print(L);
    // Serial.print(" = ");
    // Serial.println(b);
  }
  EEPROM.commit();
}


// ###########################################################################################################################################
// # Check for HTML respond from Client logged on to Web page:
// ###########################################################################################################################################
void checkClient() {
  //Serial.println("check for client");
  WiFiClient client = server.available();  // Listen for incoming clients // @suppress("Abstract class cannot be instantiated")

  if (client) {  // If a new client connects,
    //Serial.println("New web client...");          // print a message out in the serial port
    String currentLine = "";      // make a String to hold incoming data from the client
    while (client.connected()) {  // loop while the client's connected
      if (client.available()) {   // if there's bytes to read from the client,
        char c = client.read();   // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            // Display the HTML web page:
            // ##########################
            client.println("<!DOCTYPE html><html>");
            client.println("<head><title>" + ClockName + " - " + wchostname + wchostnamenum + "</title><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");


            // Web Page Heading:
            // #################
            String title = "<body><h1>" + ClockName + " '" + wchostname + wchostnamenum + "' " + txtSettings + ": ";
            title = title + CLOCK_VERSION;
            title = title + "</h1>";
            client.println(title);
            client.println("<form action=\"/setWC.php\">");


            // Convert color into hex settings:
            // ################################
            if (parameter.pRed >= 0) {  // Load defined color from EEPROM - important after Rainbow random function usage
              redVal = parameter.pRed;
              greenVal = parameter.pGreen;
              blueVal = parameter.pBlue;
            }
            char hex_main[7] = { 0 };
            sprintf(hex_main, "#%02X%02X%02X", redVal, greenVal, blueVal);
            client.println("<hr><h2>" + txtLEDsettings + ":</h2><br>");
            client.println("<label for=\"favcolor\">" + txtLEDcolor + ": </label>");
            client.print("<input type=\"color\" id=\"favcolor\" name=\"favcolor\" value=\"");
            client.print(hex_main);
            client.print("\"><br><br>");


            // Intensity Day:
            // ##############
            client.print("<label for=\"intensity\">" + txtIntensityDay + ": </label>");
            client.print("<input type='range' id='intensity' name='intensity' min='1' max='255' value=");
            client.print(intensity);
            client.println(" style='height:30px; width:200px' oninput='showValue(this.value)'>");
            client.print("<span id='valrange'>");
            client.print(intensity);
            client.println("</span>");
            client.println("<script type='text/javascript'>\r\nfunction showValue(newValue)\r\n{\r\ndocument.getElementById('valrange').innerHTML=newValue;\r\n}\r\n</script>\r\n");
            client.println("</label><br><br><hr>");

            /*
            // Intensity Night:
            // ################
            client.print("<label for=\"intensity\">" + txtIntensityNight + ": </label>");
            client.print("<input type='range' id='intensityNight' name='intensityNight' min='1' max='255' value=");
            client.print(intensityNight);
            client.println(" style='height:30px; width:200px' oninput='showValueNight(this.value)'>");
            client.print("<span id='valrangeNight'>");
            client.print(intensityNight);
            client.println("</span>");
            client.println("<script type='text/javascript'>\r\nfunction showValueNight(newValue)\r\n{\r\ndocument.getElementById('valrangeNight').innerHTML=newValue;\r\n}\r\n</script>\r\n");
            client.println("</label>");
            client.print("<br><hr>");

            
            // Show date value as scrolling text every minute after 30s:
            // #########################################################
            client.println("<h2>" + txtShowDate1 + ":</h2><br>");
            client.println("<label for=\"showdate\">" + txtShowDate2 + ": </label>");
            client.print("<input type=\"checkbox\" id=\"showdate\" name=\"showdate\"");
            if (showDate)
              client.print(" checked");
            client.print(">");
            client.print("<br><hr>");


            // Night mode - display off:
            // ###########
            client.println("<h2>" + txtNightMode1 + ":</h2><br>");
            client.println("<label for=\"displayoff\">" + txtNightMode2 + " </label>");
            client.print("<input type=\"checkbox\" id=\"displayoff\" name=\"displayoff\"");
            if (displayoff) {
              client.print(" checked");
            }
            client.print("><br><br>");


            // Night mode - display darker:
            // ############################
            client.println("<label for=\"useNightLEDs\">" + txtNightMode3 + " </label>");
            client.print("<input type=\"checkbox\" id=\"useNightLEDs\" name=\"useNightLEDs\"");
            if (useNightLEDs) {
              client.print(" checked");
            }
            client.print("><br><br>");

            // Monday:
            // #######
            client.println("<label for=\"displayonmaxMO\">" + txtMO + " - " + txtNightModeOff + ": </label><select id=\"displayonmaxMO\" name=\"displayonmaxMO\" ><option selected=\"selected\">");
            client.println(displayonmaxMO);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminMO\" name=\"displayonminMO\" ><option selected=\"selected\">");
            client.println(displayonminMO);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br>");

            // Tuesday:
            // ########
            client.println("<label for=\"displayonmaxTU\">" + txtTU + " - " + txtNightModeOff + ": </label><select id=\"displayonmaxTU\" name=\"displayonmaxTU\" ><option selected=\"selected\">");
            client.println(displayonmaxTU);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminTU\" name=\"displayonminTU\" ><option selected=\"selected\">");
            client.println(displayonminTU);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br>");
            ;

            // Wednesday:
            // ##########
            client.println("<label for=\"displayonmaxWE\">" + txtWE + " - " + txtNightModeOff + ": </label><select id=\"displayonmaxWE\" name=\"displayonmaxWE\" ><option selected=\"selected\">");
            client.println(displayonmaxWE);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminWE\" name=\"displayonminWE\" ><option selected=\"selected\">");
            client.println(displayonminWE);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br>");

            // Thursday:
            // #########
            client.println("<label for=\"displayonmaxTH\">" + txtTH + " - " + txtNightModeOff + ":  </label><select id=\"displayonmaxTH\" name=\"displayonmaxTH\" ><option selected=\"selected\">");
            client.println(displayonmaxTH);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminTH\" name=\"displayonminTH\" ><option selected=\"selected\">");
            client.println(displayonminTH);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br>");

            // Friday:
            // #######
            client.println("<label for=\"displayonmaxFR\">" + txtFR + " - " + txtNightModeOff + ": </label><select id=\"displayonmaxFR\" name=\"displayonmaxFR\" ><option selected=\"selected\">");
            client.println(displayonmaxFR);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminFR\" name=\"displayonminFR\" ><option selected=\"selected\">");
            client.println(displayonminFR);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br>");

            // Saturday:
            // #########
            client.println("<label for=\"displayonmaxSA\">" + txtSA + " - " + txtNightModeOff + ": </label><select id=\"displayonmaxSA\" name=\"displayonmaxSA\" ><option selected=\"selected\">");
            client.println(displayonmaxSA);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminSA\" name=\"displayonminSA\" ><option selected=\"selected\">");
            client.println(displayonminSA);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br>");

            // Sunday:
            // #######
            client.println("<label for=\"displayonmaxSU\">" + txtSU + " - " + txtNightModeOff + ": </label><select id=\"displayonmaxSU\" name=\"displayonmaxSU\" ><option selected=\"selected\">");
            client.println(displayonmaxSU);
            client.println("</option><option>15</option><option>16</option><option>17</option><option>18</option><option>19</option><option>20</option><option>21</option><option>22</option><option>23</option>");
            client.println("</select>:00 " + txtNightModeTo + " <select id=\"displayonminSU\" name=\"displayonminSU\" ><option selected=\"selected\">");
            client.println(displayonminSU);
            client.println("</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select>:59 " + txtNightModeClock + " <br><br><hr>");
            */

            // LED display and startup:
            // ########################

            // LED test:
            client.println("<h2>" + txtContentStartup + ":</h2>");
            client.println("<label for=\"useledtest\">" + txtUseLEDtest + " </label>");
            client.print("<input type=\"checkbox\" id=\"useledtest\" name=\"useledtest\"");
            if (useledtest) {
              client.print(" checked");
              client.print("><br><br>");
            } else {
              client.print("><br><br>");
            }
            client.println("<hr>");


            // Language selection:
            // ###################
            client.println("<br><label for=\"switchLangWeb\"><h2>" + languageSelect + ":</h2></label>");
            client.println("<fieldset>");
            client.println("<div>");
            client.println("<input type='radio' id='idlang0' name='switchLangWeb' value='0'");
            if (switchLangWeb == 0) {
              client.print(" checked");
              client.print(">");
            } else {
              client.print(">");
            }
            client.println("<label for='idlang0'>" + languageInt0 + "</label>");
            client.println("</div>");
            client.println("<div>");
            client.println("<input type='radio' id='idlang1' name='switchLangWeb' value='1'");
            if (switchLangWeb == 1) {
              client.print(" checked");
              client.print(">");
            } else {
              client.print(">");
            }
            client.println("<label for='idlang1'>" + languageInt1 + "</label>");
            client.println("</div>");
            client.println("</fieldset>");
            client.println("<br><br><hr>");

            /*
            // Hostname:
            // #########
            client.println("<h2>" + txtHostName1 + ":</h2><br>");
            client.println("<label for=\"wchostnamenum\">" + txtHostName2 + ": " + wchostname + "</label>");
            client.println("<select id=\"wchostnamenum\" name=\"wchostnamenum\" >");
            client.println("<option selected=\"selected\">");
            client.println(wchostnamenum);
            client.println("</option><option>0</option><option>1</option><option>2</option><option>3</option><option>4</option><option>5</option><option>6</option><option>7</option><option>8</option><option>9</option></select><br><br><hr>");

         
            // REST functions:
            // ###############
            client.println("<h2>" + txtREST0 + ":</h2>");
            client.println("<label>" + txtREST1 + "</label><br><br>");
            client.println("<label for=\"useresturl\">" + txtREST2 + " </label>");
            client.print("<input type=\"checkbox\" id=\"useresturl\" name=\"useresturl\"");
            client.print(" checked");
            client.print("><br><br>");
            client.println("<label>" + txtREST3 + ": </label>");
            client.println("<br>");
            client.println("<label>" + txtREST4 + ": </label>");
            client.println("<a href=http://" + WiFi.localIP().toString() + ":" + server1port + "/ledsoff target='_blank'>http://" + WiFi.localIP().toString() + ":" + server1port + "/ledsoff</a><br>");
            client.println("<label>" + txtREST5 + ": </label>");
            client.println("<a href=http://" + WiFi.localIP().toString() + ":" + server1port + "/ledson target='_blank'>http://" + WiFi.localIP().toString() + ":" + server1port + "/ledson</a><br>");
            client.println("<label>" + txtREST6 + ": </label>");
            client.println("<a href=http://" + WiFi.localIP().toString() + ":" + server1port + "/ledstatus target='_blank'>http://" + WiFi.localIP().toString() + ":" + server1port + "/ledstatus</a><br>");
            client.print("<br><hr>");


            // Update function:
            // ################
             client.println("<h2>" + txtUpdate0 + ":</h2>");
            client.println("<fieldset>");
            client.println("<div>");
            client.println("<input type='radio' id='idupdate0' name='useupdate' value='0'");
            if (useupdate == 0) {
              client.print(" checked");
              client.print(">");
            } else {
              client.print(">");
            }
            client.println("<label for='idupdate0'>" + txtUpdateE1 + "</label>");
            client.println("</div>");
            client.println("<div>");
            client.println("<input type='radio' id='idupdate1' name='useupdate' value='1'");
            if (useupdate == 1) {
              client.print(" checked");
              client.print(">");
            } else {
              client.print(">");
            }
            client.println("<label for='idupdate1'>" + txtUpdateE2 + "</label>");
            client.println("</div>");
            client.println("<div>");
            client.println("<input type='radio' id='idupdate2' name='useupdate' value='2'");
            if (useupdate == 2) {
              client.print(" checked");
              client.print(">");
            } else {
              client.print(">");
            }
            client.println("<label for='idupdate2'>" + txtUpdateE3 + "</label>");
            client.println("</div>");
            client.println("</fieldset>");

            // Update details section:
            if (useupdate == -1) useupdate = 1;  // Fix for setting from older versions
            if (useupdate == 0) {
              client.println("<br><br><label>" + txtUpdateX + "</label>");
            }
            if (useupdate == 1) {
              client.println("<br><br><label>" + txtUpdate2 + ":</label><br><br>");
              client.println("<a href=" + UpdatePath + " target='_blank'>" + UpdatePath + "</a><br><br>");
              client.println("<a href=" + UpdatePathIP + " target='_blank'>" + UpdatePathIP + "</a><br><br>");
              client.println("<label>" + txtUpdate3 + "<br>" + txtUpdate4 + "</label>");
              client.println("<br><br><label>" + txtUpdate5 + ":</label>");
              client.println("<a href='https://github.com/N1cls/Clock' target='_blank'>" + txtUpdate6 + "</a>");
            }
            if (useupdate == 2) {
              if (AvailableVersion == "-") readhttpfile();  // Read version from internet if function used for the first time...
              if (UpdateAvailable == 0) {
                client.println("<br><br><label>" + txtUpdate7 + ": " + AvailableVersion + "</label>");
              }
              if (UpdateAvailable == 1) {
                client.println("<br><br><label><b>" + txtUpdate8 + ": " + AvailableVersion + "</b></label><br><br>");
                if (useresturl == -1) {
                  client.println("<label>" + txtUpdate9 + ": <a href= http://" + WiFi.localIP().toString() + ":" + server1port + "/esphttpupdate target='_blank'>" + txtUpdate0 + "</a></label>");
                } else {
                  // client.println("<label>" + txtRESTX + " - " + txtUpdateX + "</label>");
                }
              }
            }
            client.println("<br><br><hr>"); */


            // Reset WiFi configuration:
            // #########################
            client.println("<h2>" + txtWiFi0 + ":</h2><br>");
            client.println("<label>" + txtWiFi1 + "</label><br>");
            client.println("<br><a href= http://" + WiFi.localIP().toString() + ":" + server1port + "/clockwifireset target='_blank'>" + txtWiFi0 + "</a><br>");
            client.println("<br>! " + txtWiFi2 + " !<br><br><hr>");


            // Clock restart:
            // ##################
            client.println("<h2>" + txtRestart0 + ":</h2><br>");
            client.println("<label>" + txtRestart1 + "</label><br>");
            client.println("<br><a href= http://" + WiFi.localIP().toString() + ":" + server1port + "/clockrestart target='_blank'>" + txtRestart0 + "</a><br>");
            client.println("<br>! " + txtRestart2 + " !<br><br><hr>");


            // Timezone and NTP:
            // #################
            client.println("<h2>" + txtTZNTP0 + ":</h2><br>");
            client.println("<label for=\"ntpserver\"></label>");
            client.print("<input type=\"text\" id=\"ntpserver\" name=\"ntpserver\" size=\"45\" value=\"");
            client.print(ntpServer);
            client.println("\"><br><br>");
            client.print("<input type=\"text\" id=\"timezone\" name=\"timezone\" size=\"45\" value=\"");
            client.print(timeZone);
            client.println("\"><br><br>");
            client.print("<br>" + txtTZNTP1 + ":<br><br>");
            client.print(DEFAULT_NTP_SERVER);
            client.println("<br><br>");
            client.print(DEFAULT_TIMEZONE);
            client.print("<br><br><br><a href=\"");
            client.println(TZ_WEB_SITE);
            client.println("\" target=\"_blank\">" + txtTZNTP2 + "</a><br><br><hr><br>");


            // Save settings button:
            // #####################
            client.println("<br><br><input type='submit' value='" + txtSaveSettings + "'>");
            client.print("<br><br><br><hr><br>");
            client.println("</form>");
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            if (currentLine.startsWith("GET /setWC.php?")) {
              // Serial.print("Current request:  ");
              // Serial.println(currentLine);


              // Check for color settings:
              // #########################
              int pos = currentLine.indexOf("favcolor=%23");
              if (pos >= 0) {
                char* succ;
                String newColStr = currentLine.substring(pos + 12, pos + 18);
                String newRed = newColStr.substring(0, 2);
                redVal = strtol(newRed.begin(), &succ, 16);
                String newGreen = newColStr.substring(2, 4);
                greenVal = strtol(newGreen.begin(), &succ, 16);
                String newBlue = newColStr.substring(4, 6);
                blueVal = strtol(newBlue.begin(), &succ, 16);
              }

              /*
              // Check for date display:
              // #######################
              if (currentLine.indexOf("&showdate=on&") >= 0) {
                showDate = -1;
              } else {
                showDate = 0;
              } 


              // Check for DisplayOff switch:
              // ############################
              if (currentLine.indexOf("&displayoff=on&") >= 0) {
                displayoff = -1;
              } else {
                displayoff = 0;
              }


              // Check for useNightLEDs switch:
              // ##############################
              if (currentLine.indexOf("&useNightLEDs=on&") >= 0) {
                useNightLEDs = -1;
              } else {
                useNightLEDs = 0;
              }


              // Pick up Display OFF - Monday:
              // ############################
              pos = currentLine.indexOf("&displayonmaxMO=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxMO = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminMO=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminMO = minStr.toInt();
              }


              // Pick up Display OFF Max - Tuesday:
              // #################################
              pos = currentLine.indexOf("&displayonmaxTU=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxTU = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminTU=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminTU = minStr.toInt();
              }


              // Pick up Display OFF - Wednesday:
              // ################################
              pos = currentLine.indexOf("&displayonmaxWE=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxWE = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminWE=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminWE = minStr.toInt();
              }


              // Pick up Display OFF - Thursday:
              // ###############################
              pos = currentLine.indexOf("&displayonmaxTH=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxTH = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminTH=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminTH = minStr.toInt();
              }


              // Pick up Display OFF - Friday:
              // #############################
              pos = currentLine.indexOf("&displayonmaxFR=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxFR = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminFR=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminFR = minStr.toInt();
              }


              // Pick up Display OFF - Saturday:
              // ###############################
              pos = currentLine.indexOf("&displayonmaxSA=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxSA = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminSA=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminSA = minStr.toInt();
              }


              // Pick up Display OFF - Sunday:
              // #############################
              pos = currentLine.indexOf("&displayonmaxSU=");
              if (pos >= 0) {
                String maxStr = currentLine.substring(pos + 16);
                pos = maxStr.indexOf("&");
                if (pos > 0)
                  maxStr = maxStr.substring(0, pos);
                displayonmaxSU = maxStr.toInt();
              }
              pos = currentLine.indexOf("&displayonminSU=");
              if (pos >= 0) {
                String minStr = currentLine.substring(pos + 16);
                pos = minStr.indexOf("&");
                if (pos > 0)
                  minStr = minStr.substring(0, pos);
                displayonminSU = minStr.toInt();
              }


              // Pick up Clock HostName:
              // ###########################
              pos = currentLine.indexOf("&wchostnamenum=");
              if (pos >= 0) {
                String hostStr = currentLine.substring(pos + 15);
                pos = hostStr.indexOf("&");
                if (pos > 0)
                  hostStr = hostStr.substring(0, pos);
                wchostnamenum = hostStr.toInt();
              }


              // Check for UseUpdate switch:
              // ###########################
              pos = currentLine.indexOf("&useupdate=");
              if (pos >= 0) {
                String updateStr = currentLine.substring(pos + 11);
                pos = updateStr.indexOf("&");
                if (pos > 0)
                  updateStr = updateStr.substring(0, pos);
                useupdate = updateStr.toInt();
              }
              if (useupdate == 1) {
                MDNS.begin(wchostname + wchostnamenum);
                httpUpdater.setup(&httpServer);
                httpServer.begin();
                MDNS.addService("http", "tcp", 2022);
                MDNS.addService("http", "tcp", 80);
                UpdatePath = "http://" + String(wchostname + wchostnamenum) + ".local:2022/update";
                UpdatePathIP = "http://" + WiFi.localIP().toString() + ":2022/update";
              } else {
                httpUpdater.setup(&httpServer);
                httpServer.stop();
              } */


              // Check for Use LED test switch:
              // ##############################
              if (currentLine.indexOf("&useledtest=on&") >= 0) {
                useledtest = -1;
              } else {
                useledtest = 0;
              }


              // Check for language setting:
              // ###########################
              pos = currentLine.indexOf("&switchLangWeb=");
              if (pos >= 0) {
                String LangWebStr = currentLine.substring(pos + 15);
                pos = LangWebStr.indexOf("&");
                if (pos > 0)
                  LangWebStr = LangWebStr.substring(0, pos);
                switchLangWeb = LangWebStr.toInt();
                setLanguage(switchLangWeb);
              }


              // Get intensity DAY:
              // ##################
              pos = currentLine.indexOf("&intensity=");
              if (pos >= 0) {
                String intStr = currentLine.substring(pos + 11, pos + 14);
                pos = intStr.indexOf("&");
                if (pos > 0)
                  intStr = intStr.substring(0, pos);
                intensity = intStr.toInt();
              }


              // Get intensity NIGHT:
              // ####################
              /*
              pos = currentLine.indexOf("&intensityNight=");
              if (pos >= 0) {
                String intStr = currentLine.substring(pos + 16, pos + 19);
                pos = intStr.indexOf("&");
                if (pos > 0)
                  intStr = intStr.substring(0, pos);
                intensityNight = intStr.toInt();
              }
              */


              // Get NTP server:
              // ###############
              pos = currentLine.indexOf("&ntpserver=");
              if (pos >= 0) {
                String ntpStr = currentLine.substring(pos + 11);
                pos = ntpStr.indexOf("&");  // "&" !!!
                if (pos > 0)
                  ntpStr = ntpStr.substring(0, pos);
                ntpServer = ntpStr;
              }


              // Pick up TimeZone definition:
              // ############################
              pos = currentLine.indexOf("timezone=");
              if (pos >= 0) {
                String tz = currentLine.substring(pos + 9);
                // check for unwanted suffix
                pos = tz.indexOf(" ");  // " " !!!
                if (pos > 0) {
                  tz = tz.substring(0, pos);
                }
                timeZone = urldecode(tz);
              }


              // Save data to EEPROM:
              // ####################
              writeEEPROM();  // save DATA to EEPROM
              //configNTPTime();  // Reset NTP
            }
            currentLine = "";  // Clear the current command line
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    header = "";    // Clear the header variable
    client.stop();  // Close the connection
    //Serial.println("Web client disconnected.");
    //Serial.println("######################################################################################################");
  }
}


// ###########################################################################################################################################
// # Get a digit from a number at position pos: (Split IP-address octets in single digits)
// ###########################################################################################################################################
int getDigit(int number, int pos) {
  return (pos == 0) ? number % 10 : getDigit(number / 10, --pos);
}


// ###########################################################################################################################################
// # Show the IP-address on the display:
// ###########################################################################################################################################
void showIP() {
  int delayip = 2000;
  dunkel();
  delay(1000);
  IPAddress ip1 = WiFi.localIP();
  Serial.print("Displaying IP address ");
  Serial.println(ip1);

  // IP:
  setLED(24, 24, 1);
  setLED(26, 26, 1);
  setLED(15, 19, 1);
  setLED(28, 29, 1);
  pixels.show();
  delay(delayip);

  // Octet 1:
  dunkel();
  numbers(getDigit(int(WiFi.localIP()[0]), 2), 3, 0);
  numbers(getDigit(int(WiFi.localIP()[0]), 1), 2, 0);
  numbers(getDigit(int(WiFi.localIP()[0]), 0), 1, 0);
  setLED(0, 0, 1);
  pixels.show();
  delay(delayip);

  // Octet 2:
  numbers(getDigit(int(WiFi.localIP()[1]), 2), 3, 0);
  numbers(getDigit(int(WiFi.localIP()[1]), 1), 2, 0);
  numbers(getDigit(int(WiFi.localIP()[1]), 0), 1, 0);
  setLED(0, 0, 1);
  pixels.show();
  delay(delayip);

  // Octet 3:
  numbers(getDigit(int(WiFi.localIP()[2]), 2), 3, 0);
  numbers(getDigit(int(WiFi.localIP()[2]), 1), 2, 0);
  numbers(getDigit(int(WiFi.localIP()[2]), 0), 1, 0);
  setLED(0, 0, 1);
  pixels.show();
  delay(delayip);

  // Octet 4:
  dunkel();
  numbers(getDigit(int(WiFi.localIP()[3]), 2), 3, 0);
  numbers(getDigit(int(WiFi.localIP()[3]), 1), 2, 0);
  numbers(getDigit(int(WiFi.localIP()[3]), 0), 1, 0);
  pixels.show();
  delay(delayip);

  // http:
  dunkel();
  setLED(21, 21, 1);
  setLED(24, 26, 1);
  setLED(17, 20, 1);
  setLED(10, 13, 1);
  setLED(1, 5, 1);
  pixels.show();
  delay(delayip);

  // on:
  dunkel();
  setLED(18, 20, 1);
  setLED(14, 14, 1);
  setLED(7, 7, 1);
  setLED(11, 12, 1);
  pixels.show();
  delay(delayip);
  dunkel();
}


// ###########################################################################################################################################
// #  Switch off all LEDs:
// ###########################################################################################################################################
void dunkel() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));  // Switch off all LEDs
  }
}


// ###########################################################################################################################################
// # Convert x/y coordinates into LED number return -1 for invalid coordinate:
// ###########################################################################################################################################
/*int ledXY(int x, int y) {
  // Test for valid coordinates
  // If outside panel return -1
  if ((x < 0) || (x > 10) || (y < 0) || (y > 9))
    return -1;
  int ledNr = (9 - y) * 11;
  if ((y % 2) == 0)
    ledNr = ledNr + x;
  else
    ledNr = ledNr + 10 - x;
  return ledNr;
}*/


// ###########################################################################################################################################
// # Sets, where the numbers from 1 to 9 are printed:
// ###########################################################################################################################################
/*void printAt(int ziffer, int x, int y) {
  switch (ziffer) {
    case 0:                                 //number 0
      setLEDLine(x + 1, x + 3, y + 6, -1);  //-1 is true, so switchOn
      for (int yd = 1; yd < 6; yd++) {
        setLED(ledXY(x, y + yd), ledXY(x, y + yd), -1);
        setLED(ledXY(x + 4, y + yd), ledXY(x + 4, y + yd), -1);
      }
      setLEDLine(x + 1, x + 3, y, -1);
      break;

    case 1:  //number 1
      setLED(ledXY(x + 3, y + 5), ledXY(x + 3, y + 5), -1);
      setLED(ledXY(x + 2, y + 4), ledXY(x + 2, y + 4), -1);
      for (int yd = 0; yd <= 6; yd++) {
        setLED(ledXY(x + 4, y + yd), ledXY(x + 4, y + yd), -1);
      }
      break;

    case 2:  //number 2
      for (int d = 1; d <= 4; d++) {
        setLED(ledXY(x + d, y + d), ledXY(x + d, y + d), -1);
      }
      setLEDLine(x, x + 4, y, -1);
      setLED(ledXY(x, y + 5), ledXY(x, y + 5), -1);
      setLED(ledXY(x + 4, y + 5), ledXY(x + 4, y + 5), -1);
      setLEDLine(x + 1, x + 3, y + 6, -1);
      break;

    case 3:  //number 3
      for (int yd = 1; yd <= 2; yd++) {
        setLED(ledXY(x + 4, y + yd + 3), ledXY(x + 4, y + yd + 3), -1);
        setLED(ledXY(x + 4, y + yd), ledXY(x + 4, y + yd), -1);
      }
      for (int yd = 0; yd < 7; yd = yd + 3) {
        setLEDLine(x + 1, x + 3, y + yd, -1);
      }
      setLED(ledXY(x, y + 1), ledXY(x, y + 1), -1);
      setLED(ledXY(x, y + 5), ledXY(x, y + 5), -1);
      break;

    case 4:  //number 4
      for (int d = 0; d <= 3; d++) {
        setLED(ledXY(x + d, y + d + 3), ledXY(x + d, y + d + 3), -1);
      }
      for (int yd = 0; yd <= 3; yd++) {
        setLED(ledXY(x + 3, y + yd), ledXY(x + 3, y + yd), -1);
      }
      setLEDLine(x, x + 4, y + 2, -1);
      break;

    case 5:  //number 5
      setLEDLine(x, x + 4, y + 6, -1);
      setLED(ledXY(x, y + 5), ledXY(x, y + 5), -1);
      setLED(ledXY(x, y + 4), ledXY(x, y + 4), -1);
      setLEDLine(x, x + 3, y + 3, -1);
      setLED(ledXY(x + 4, y + 2), ledXY(x + 4, y + 2), -1);
      setLED(ledXY(x + 4, y + 1), ledXY(x + 4, y + 1), -1);
      setLEDLine(x, x + 3, y, -1);
      break;

    case 6:  //number 6
      for (int d = 0; d <= 3; d++) {
        setLED(ledXY(x + d, y + d + 3), ledXY(x + d, y + d + 3), -1);
      }
      for (int yd = 0; yd < 4; yd = yd + 3) {
        setLEDLine(x + 1, x + 3, y + yd, -1);
      }
      setLED(ledXY(x, y + 1), ledXY(x, y + 1), -1);
      setLED(ledXY(x, y + 2), ledXY(x, y + 2), -1);
      setLED(ledXY(x + 4, y + 1), ledXY(x + 4, y + 1), -1);
      setLED(ledXY(x + 4, y + 2), ledXY(x + 4, y + 2), -1);
      break;

    case 7:  //number 7
      for (int yd = 0; yd <= 6; yd++) {
        setLED(ledXY(x + 3, y + yd), ledXY(x + 3, y + yd), -1);
      }
      setLEDLine(x + 1, x + 4, y + 3, -1);
      setLEDLine(x, x + 3, y + 6, -1);
      break;

    case 8:  //number 8
      for (int yd = 1; yd <= 2; yd++) {
        setLED(ledXY(x + 4, y + yd + 3), ledXY(x + 4, y + yd + 3), -1);
        setLED(ledXY(x + 4, y + yd), ledXY(x + 4, y + yd), -1);
        setLED(ledXY(x, y + yd + 3), ledXY(x, y + yd + 3), -1);
        setLED(ledXY(x, y + yd), ledXY(x, y + yd), -1);
      }
      for (int yd = 0; yd < 7; yd = yd + 3) {
        setLEDLine(x + 1, x + 3, y + yd, -1);
      }
      break;

    case 9:  //number 9
      for (int d = 0; d <= 3; d++) {
        setLED(ledXY(x + d + 1, y + d), ledXY(x + d + 1, y + d), -1);
      }
      for (int yd = 4; yd <= 5; yd++) {
        setLED(ledXY(x, y + yd), ledXY(x, y + yd), -1);
        setLED(ledXY(x + 4, y + yd), ledXY(x + 4, y + yd), -1);
      }
      setLEDLine(x + 1, x + 3, y + 6, -1);
      setLEDLine(x + 1, x + 4, y + 3, -1);
      break;
  }
}*/


// ###########################################################################################################################################
// # Turns on the outer four LEDs (one per minute):
// ###########################################################################################################################################
/*void showMinutes(int minutes) {
  int minMod = (minutes % 5);
  for (int i = 1; i < 5; i++) {
    int ledNr = 0;

    if (minMod < i)
      pixels.setPixelColor(ledNr, pixels.Color(0, 0, 0));
    else
      pixels.setPixelColor(ledNr, pixels.Color(redVal, greenVal, blueVal));
  } 
}*/


// ###########################################################################################################################################
// # Show current date on clock with moving digits:
// ###########################################################################################################################################
/*void showCurrentDate() {
}*/
  

// ###########################################################################################################################################
// # Startup LED test function:
// ###########################################################################################################################################
int dots = 0;
void DisplayTest() {
  if (useledtest) {
    Serial.println("Display Test...");
    for (int y = 0; y < 4; y++) {
      for (int i = 0; i < 10; i++) {
        if (dots == 1) {
          dots = 0;
        } else {
          dots = 1;
        }
        numbers(i, y, dots);
        pixels.show();
        delay(500);
      }
    }
  }
}


// ###########################################################################################################################################
// # Set the numbers on the display in each single row:
// ###########################################################################################################################################
void numbers(int wert, int stelle, int dots) {
  int row = 0;


  switch (stelle) {
    case 0:
      {
        row = 0;
        break;
      }
    case 1:
      {
        row = 7;
        break;
      }
    case 2:
      {
        row = 14;
        break;
      }
    case 3:
      {
        row = 21;
        break;
      }
  }


  for (int i = row; i < row + 7; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));   // Switch off all LEDs in that row
    pixels.setPixelColor(28, pixels.Color(0, 0, 0));  // Switch off dot 1
    pixels.setPixelColor(29, pixels.Color(0, 0, 0));  // Switch off dot 2
  }


  if (dots == 1) {
    setLED(28, 29, 1);
  } else {
    setLED(28, 29, 0);
  }


  switch (wert) {
    case 0:
      {
        setLED(0 + row, 3 + row, 1);
        setLED(5 + row, 6 + row, 1);
        break;
      }
    case 1:
      {
        setLED(0 + row, 1 + row, 1);
        break;
      }
    case 2:
      {
        setLED(1 + row, 2 + row, 1);
        setLED(4 + row, 6 + row, 1);
        break;
      }
    case 3:
      {
        setLED(0 + row, 2 + row, 1);
        setLED(4 + row, 4 + row, 1);
        setLED(6 + row, 6 + row, 1);
        break;
      }
    case 4:
      {
        setLED(0 + row, 1 + row, 1);
        setLED(3 + row, 4 + row, 1);
        break;
      }
    case 5:
      {
        setLED(0 + row, 0 + row, 1);
        setLED(2 + row, 4 + row, 1);
        setLED(6 + row, 6 + row, 1);
        break;
      }
    case 6:
      {
        setLED(0 + row, 0 + row, 1);
        setLED(2 + row, 6 + row, 1);
        break;
      }
    case 7:
      {
        setLED(0 + row, 2 + row, 1);
        break;
      }
    case 8:
      {
        setLED(0 + row, 6 + row, 1);
        break;
      }
    case 9:
      {
        setLED(0 + row, 4 + row, 1);
        setLED(6 + row, 6 + row, 1);
        break;
      }
  }
}



// ###########################################################################################################################################
// # Handle the time from the NTP server and write it to the RTC:
// ###########################################################################################################################################
void handleTime() {
  int timedebug = 0;
  // Check, whether we are connected to WLAN
  if ((WiFi.status() == WL_CONNECTED)) {
    configNTPTime();
    time_t now;
    time(&now);  // read the current time
    struct tm ti;
    localtime_r(&now, &ti);
    uint16_t ye = ti.tm_year + 1900;
    uint8_t mo = ti.tm_mon + 1;
    uint8_t da = ti.tm_mday;
    int ho = ti.tm_hour;
    int mi = ti.tm_min;
    int sec = ti.tm_sec;

    // NTP TIME WITHOUT RTC:
    if (timedebug == 1) Serial.print("Current time: ");
    iYear = ye;
    iMonth = mo;
    iDay = da;
    iHour = ho;
    iMinute = mi;
    iSecond = sec;

    if (timedebug == 1) {
      Serial.print(ho);
      Serial.print(':');
      Serial.print(mi);
      Serial.print(':');
      Serial.print(sec);
      Serial.print(" ");
      Serial.print(ye);
      Serial.print('-');
      Serial.print(mo);
      Serial.print('-');
      Serial.println(da);
    }

    if (dots == 1) {
      dots = 0;
    } else {
      dots = 1;
    }

    // Add leading zero:
    int hour = ho;
    int mini = mi;
    String h = String(hour);
    if (hour < 10) { h = "0" + h; }
    String m = String(mini);
    if (mini < 10) { m = "0" + m; }
    String s = h + ":" + m;
    if (timedebug == 1) Serial.println(s);

    // Display the time:
    numbers(h.substring(0, 1).toInt(), 3, dots);
    numbers(h.substring(1).toInt(), 2, dots);
    numbers(m.substring(0, 1).toInt(), 1, dots);
    numbers(m.substring(1).toInt(), 0, dots);
    pixels.show();
  }
}


// ###########################################################################################################################################
// # Handle day and night time mode:
// ###########################################################################################################################################
/*void DayNightMode(int displayonMin, int displayonMax) {
  if (iHour > displayonMin && iHour < displayonMax) {
    ShowTheTime();
  } else {
    if (useNightLEDs == -1) {
      pixels.setBrightness(intensityNight);  // Night brightness
      ShowTheTime();
    } else {
      dunkel();
    }
  }
}*/


// ###########################################################################################################################################
// # NTP time function:
// ###########################################################################################################################################
void configNTPTime() {
  //Serial.print("Set time zone to ");
  //Serial.println(timeZone);
  //Serial.print("Set time server to ");
  //Serial.println(ntpServer);
  configTime(timeZone.begin(), ntpServer.begin());
}


// ###########################################################################################################################################
// # Convert hex digit to int value:
// ###########################################################################################################################################
unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}


// ###########################################################################################################################################
// # Decode %xx values in String - comming from URL / HTTP:
// ###########################################################################################################################################
String urldecode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {
      encodedString += c;
    }
    yield();
  }
  return encodedString;
}


// ###########################################################################################################################################
// # REST command function: ROOT
// ###########################################################################################################################################
void handleRoot() {
  WiFiClient client = server.available();
  server1->send(200, "text/plain", "Clock REST web server active");
  Serial.print("Clock REST web server active on port: ");
  Serial.println(server1port);
  client.stop();
}


// ###########################################################################################################################################
// # REST command function: LED set to ON
// ###########################################################################################################################################
/*void ledsON() {
  WiFiClient client = server.available();
  server1->send(200, "text/plain", "Clock LEDs set to ON");
  Serial.println("Clock LEDs set to ON");
  pixels.setBrightness(intensity);
  RESTmanLEDsON = true;
  LEDsON = true;
  pixels.show();
  client.stop();
}*/


// ###########################################################################################################################################
// # REST command function: LED set to OFF
// ###########################################################################################################################################
/*void ledsOFF() {
  WiFiClient client = server.available();
  server1->send(200, "text/plain", "Clock LEDs set to OFF");
  Serial.println("Clock LEDs set to OFF");
  pixels.setBrightness(0);
  pixels.show();
  RESTmanLEDsON = false;
  LEDsON = false;
  pixels.show();
  client.stop();
}*/


// ###########################################################################################################################################
// # REST command function: LED status
// ###########################################################################################################################################
/*void LedStatus() {
  WiFiClient client = server.available();
  server1->send(200, "text/plain", String(LEDsON));
  Serial.print("LED status: ");
  Serial.println(LEDsON);
  client.stop();
}*/


// ###########################################################################################################################################
// # Restart the clock:
// ###########################################################################################################################################
void ClockRestart() {
  WiFiClient client = server.available();
  server1->send(200, "text/plain", "Clock WILL RESTART IN 3 SECONDS...");
  delay(1000);
  client.stop();
  dunkel();
  Serial.println("Show RESET before board reset...");
  Serial.println("##########################################");
  Serial.println("# Clock WILL RESTART IN 3 SECONDS... #");
  Serial.println("##########################################");
  delay(3000);
  ESP.restart();
}


// ###########################################################################################################################################
// # Reset the WiFi configuration:
// ###########################################################################################################################################
void ClockWifiReset() {
  WiFiClient client = server.available();
  server1->send(200, "text/plain", "WIFI SETTING WERE SET TO DEFAULT... Clock WILL NOW RESTART... PLEASE CONFIGURE WIFI AGAIN...");
  delay(1000);
  client.stop();
  dunkel();
  WiFi.disconnect(true);
  delay(1500);
  WiFiManager wifiManager;
  delay(1500);
  wifiManager.resetSettings();
  delay(1500);
  Serial.println("####################################################################################################");
  Serial.println("# WIFI SETTING WERE SET TO DEFAULT... Clock WILL NOW RESTART... PLEASE CONFIGURE WIFI AGAIN... #");
  Serial.println("####################################################################################################");
  delay(3000);
  ESP.restart();
}


// ###########################################################################################################################################
// # Actual function, which controls 1/0 of the LED:
// ###########################################################################################################################################
void setLED(int ledNrFrom, int ledNrTo, int switchOn) {
  if (switchOn) {
    if (ledNrFrom > ledNrTo) {
      setLED(ledNrTo, ledNrFrom, switchOn);  //sets LED numbers in correct order (because of the date programming below)
    } else {
      for (int i = ledNrFrom; i <= ledNrTo; i++) {
        if ((i >= 0) && (i < NUMPIXELS))
          pixels.setPixelColor(i, pixels.Color(redVal, greenVal, blueVal));
      }
    }
  }
}


// ###########################################################################################################################################
// # Wifi Manager setup and reconnect function that runs once at startup and during the loop function of the ESP:
// ###########################################################################################################################################
void WIFI_login() {
  //Serial.print("Try to connect to WiFi: ");
  //Serial.println(WiFi.SSID());
  bool WiFires;
  WiFiManager wifiManager;
  // configNTPTime();                                 // Set timezone
  wifiManager.setConfigPortalTimeout(AP_TIMEOUT);  // Max wait for 3 minutes
  WiFires = wifiManager.autoConnect(DEFAULT_AP_NAME);
  if (!WiFires) {
    Serial.print("Failed to connect to WiFi: ");
    Serial.println(WiFi.SSID());
  } else {
    Serial.print("Connected to WiFi: ");
    Serial.println(WiFi.SSID());
  }
}


// ###########################################################################################################################################
// # EOF - You have successfully reached the end of the code - well done ;-)
// ###########################################################################################################################################