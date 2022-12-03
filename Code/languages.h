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
// # Default texts in german language:
// ###########################################################################################################################################
// General texts:
String ClockName, languageSelect, languageInt0, languageInt1, txtSaveSettings;
// LED settings:
String txtSettings, txtLEDsettings, txtLEDcolor, txtIntensityDay, txtIntensityNight, txtShowDate1, txtShowDate2;
String txtNightMode1, txtNightMode2, txtNightMode3, txtNightModeOff, txtNightModeTo, txtNightModeClock;
String txtMO, txtTU, txtWE, txtTH, txtFR, txtSA, txtSU;
// Content and startup:
String txtContentStartup, txtUseLEDtest;
// Hostname:
String txtHostName1, txtHostName2;
// REST functions:
String txtREST0, txtREST1, txtREST3, txtREST4, txtREST5, txtREST6, txtREST7;
// Update function:
String txtUpdate0, txtUpdate1, txtUpdate2, txtUpdate3, txtUpdate4, txtUpdate5, txtUpdate6;
// WiFi:
String txtWiFi0, txtWiFi1, txtWiFi2;
// Restart
String txtRestart0, txtRestart1, txtRestart2;
// Time zone and NTP server:
String txtTZNTP0, txtTZNTP1, txtTZNTP2;


void setLanguage(int lang) {
  // ###########################################################################################################################################
  // # Translations for: DE
  // ###########################################################################################################################################
  if (lang == 0) {  // DEUTSCH
    // Allgemeine Texte:
    ClockName = "7 Segment Clock";
    languageSelect = "Sprache der Web Konfiguration";
    languageInt0 = "Deutsch";
    languageInt1 = "Englisch";
    txtSaveSettings = "Einstellungen speichern";

    // LED Einstellungen:
    txtSettings = "Einstellungen";
    txtLEDsettings = "LED Einstellungen";
    txtLEDcolor = "Farbe";
    txtIntensityDay = "Helligkeit am Tag";
    txtIntensityNight = "Helligkeit bei Nacht";
    txtShowDate1 = "Datum anzeigen";
    txtShowDate2 = "Datum alle 30 Sekuden anzeigen?";
    txtNightMode1 = "Display abschalten oder dunkler schalten?";
    txtNightMode2 = "Display komplett abschalten ...";
    txtNightMode3 = "... oder nur dunkler schalten auf Wert der Helligkeit bei Nacht?";
    txtNightModeOff = "Display aus ab";
    txtNightModeTo = "bis";
    txtNightModeClock = "Uhr";
    txtMO = "Montag";
    txtTU = "Dienstag";
    txtWE = "Mittwoch";
    txtTH = "Donnerstag";
    txtFR = "Freitag";
    txtSA = "Samstag";
    txtSU = "Sonntag";

    // Anzeigen und Startverhalten:
    txtContentStartup = "Anzeigen und Startverhalten";
    txtUseLEDtest = "LED Start Test anzeigen?";

    // Hostname:
    txtHostName1 = "Uhr Hostname anpassen";
    txtHostName2 = "Hostname";

    // REST Funktionen:
    txtREST0 = "REST Funktionen";
    txtREST1 = "Über die folgenden Links können Funktionen der Uhr von Außen gesteuert werden";
    txtREST3 = "Über einen der folgenden Links kann die Uhr manuell ab und an geschaltet werden";
    txtREST4 = "LEDs ausschalten";
    txtREST5 = "LEDs einschalten";
    txtREST6 = "LED Status";

    // Update Funktion:
    txtUpdate0 = "Update";
    txtUpdate1 = "Update Funktion nicht verwenden";
    txtUpdate2 = "Lokale Update Funktion verwenden";
    txtUpdate2 = "Über einen der folgenden Links kann die Uhr über den Browser ohne Arduino IDE aktualisiert werden";
    txtUpdate3 = "Hinweis: Es wird eine in der Arduino IDE mit Strg+Alt+S zuvor erstellte .BIN Datei des Sketches benötigt,";
    txtUpdate4 = "die über die Option 'Update Firmware' hochgeladen werden kann.";
    txtUpdate5 = "Die notwendige Update-Datei kann hier heruntergeladen werden";
    txtUpdate6 = "Uhr Repository auf GitHub";

    // WLAN:
    txtWiFi0 = "WLAN Einstellungen zurücksetzen";
    txtWiFi1 = "WLAN Einstellungen zurücksetzen und Uhr neu starten?";
    txtWiFi2 = "Wenn diese Option verwendet wird, werden die WLAN Einstellungen gelöscht";

    // Neustart:
    txtRestart0 = "Uhr neustarten";
    txtRestart1 = "Uhr neustarten?";
    txtRestart2 = "Wenn diese Option verwendet wird, wird die Uhr neu gestartet";

    // Zeitzone and NTP-Server:
    txtTZNTP0 = "Zeitzone & NTP-Server";
    txtTZNTP1 = "Standardwerte";
    txtTZNTP2 = "Erklärung zur Einstellung der Zeitzone";
  }


  // ###########################################################################################################################################
  // # Translations for: EN
  // ###########################################################################################################################################
  if (lang == 1) {  // ENGLISH
    // General texts:
    ClockName = "7 Segment Clock";
    languageSelect = "Language for the web configuration";
    languageInt0 = "German";
    languageInt1 = "English";
    txtSaveSettings = "Save settings";

    // LED settings:
    txtSettings = "settings";
    txtLEDsettings = "LED settings";
    txtLEDcolor = "Color";
    txtIntensityDay = "Intensity in day mode";
    txtIntensityNight = "Intensity in night mode";
    txtShowDate1 = "Show date as scolling text";
    txtShowDate2 = "Display the date every 30 seconds?";
    txtNightMode1 = "Switch off or darken the display?";
    txtNightMode2 = "Switch off the display completely ...";
    txtNightMode3 = "... or only switch it darker to the value of the intensity in night mode?";
    txtNightModeOff = "Turn display off from";
    txtNightModeTo = "to";
    txtNightModeClock = "o'clock";
    txtMO = "Monday";
    txtTU = "Tuesday";
    txtWE = "Wednesday";
    txtTH = "Thursday";
    txtFR = "Friday";
    txtSA = "Saturday";
    txtSU = "Sunday";

    // Content and startup:
    txtContentStartup = "Content and startup";
    txtUseLEDtest = "Run LED test on startup?";

    // Hostname:
    txtHostName1 = "Customize clock hostname";
    txtHostName2 = "Hostname";

    // REST functions:
    txtREST0 = "REST functions";
    txtREST1 = "clock functions can be controlled externally via the following links";
    txtREST3 = "The clock can be switched on and off manually with one of the following links";
    txtREST4 = "Turn off LEDs";
    txtREST5 = "Turn on LEDs";
    txtREST6 = "LED state";

    // Update function:
    txtUpdate0 = "Update";
    txtUpdate1 = "Do not use the update function";
    txtUpdate2 = "Use local update function";
    txtUpdate3 = "Use automatic update function via internet";
    txtUpdate2 = "Using one of the links below, the clock can be updated via the browser without the Arduino IDE";
    txtUpdate3 = "Note: A .BIN file of the sketch previously created in the Arduino IDE with Ctrl+Alt+S is required,";
    txtUpdate4 = "which can be uploaded via the 'Update Firmware' option.";
    txtUpdate5 = "The necessary update file can be downloaded here";
    txtUpdate6 = "clock repository on GitHub";

    // WiFi:
    txtWiFi0 = "Reset WiFi settings";
    txtWiFi1 = "Reset wifi settings and restart watch?";
    txtWiFi2 = "If this option is used, the WiFi settings will be deleted";

    // Restart:
    txtRestart0 = "Restart clock";
    txtRestart1 = "Restart clock?";
    txtRestart2 = "If this option is used, the clock will be restarted";

    // Time zone and NTP server:
    txtTZNTP0 = "Time zone & NTP server";
    txtTZNTP1 = "Default values";
    txtTZNTP2 = "Explanation of setting the time zone";
  }
}
// ###########################################################################################################################################
// # EOF - You have successfully reached the end of the code - well done ;-)
// ###########################################################################################################################################
