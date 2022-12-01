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
// # Default texts in german language:
// ###########################################################################################################################################
// General texts:
String ClockName, languageSelect, languageInt0, languageInt1, txtSaveSettings;
// LED settings:
String txtSettings, txtLEDsettings, txtLEDcolor, txtIntensityDay, txtIntensityNight, txtPowerSupplyNote1, txtPowerSupplyNote2, txtPowerSupplyNote3, txtPowerSupplyNote4;
String txtFlashFullHour1, txtFlashFullHour2, txtShowDate1, txtShowDate2, txtNightMode1, txtNightMode2, txtNightMode3, txtNightModeOff, txtNightModeTo, txtNightModeClock;
String txtMO, txtTU, txtWE, txtTH, txtFR, txtSA, txtSU;
// Content and startup:
String txtContentStartup, txtUseLEDtest, txtUSEsetWLAN, txtShowIP, txtRainbow1, txtRainbow2, txtRainbow3, txtRainbow4, txtMinDir1, txtMinDir2, txtMinDir3;
// Hostname:
String txtHostName1, txtHostName2;
// REST functions:
String txtREST0, txtREST1, txtREST2, txtREST3, txtREST4, txtREST5, txtREST6, txtREST7;
// Update function:
String txtUpdate0, txtUpdateE1, txtUpdateE2, txtUpdateE3, txtUpdate2, txtUpdate3, txtUpdate4, txtUpdate5, txtUpdate6, txtUpdate7, txtUpdate8, txtUpdate9;
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
  if (lang == 0) {         // DEUTSCH
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
    txtPowerSupplyNote1 = "Wichtig: Beide Werte begrenzt auf 128 von maximal 255. Achte darauf ein geeignetes Netzteil zu verwenden!";
    txtPowerSupplyNote2 = "Je nach LED Anzahl, selektierter Farbe und Helligkeit wird mindestens ein 5V/3A Netzteil empfohlen!";
    txtPowerSupplyNote3 = "Den Hinweis zum Netzteil akzeptiere und beachte ich. Ich möchte die Werte wieder auf maximal 255 einstellen können";
    txtPowerSupplyNote4 = "Den wichtigen Hinweis zum Netzteil nicht mehr anzeigen";
    txtFlashFullHour1 = "Volle Stunde blinken";
    txtFlashFullHour2 = "Stundenangabe soll zur vollen Stunde blinken?";
    txtShowDate1 = "Datumsanzeige als Lauftext";
    txtShowDate2 = "Alle 30 Sekunden anzeigen?";
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
    txtUSEsetWLAN = "SET WLAN beim Start anzeigen?";
    txtShowIP = "IP-Addresse beim Start anzeigen?";
    txtRainbow1 = "Wähle den Regenbogen Farbeffekt Modus";
    txtRainbow2 = "Aus";
    txtRainbow3 = "Variante 1 (Worte verschieden bunt)";
    txtRainbow4 = "Variante 2 (Alle Worte zufällig bunt)";
    txtMinDir1 = "Minuten LEDs Ecken Reihenfolge im Uhrzeigersinn?";
    txtMinDir2 = "Wenn diese Option gesetzt wird, werden die Minuten-LEDs in den 4 Ecken";
    txtMinDir3 = "im Uhrzeigersinn angezeigt, ansonsten entgegen dem Uhrzeigersinn.";

    // Hostname:
    txtHostName1 = "Uhr Hostname anpassen";
    txtHostName2 = "Hostname";

    // REST Funktionen:
    txtREST0 = "REST Funktionen";
    txtREST1 = "Über die folgenden Links können Funktionen der Uhr von Außen gesteuert werden.";
    txtREST2 = "REST Funktion verwenden?";
    txtREST3 = "Über einen der folgenden Links kann die Uhr manuell über den Browser ab und an geschaltet werden";
    txtREST4 = "LEDs ausschalten";
    txtREST5 = "LEDs einschalten";
    txtREST6 = "LED Status";

    // Update Funktion:
    txtUpdate0 = "Update";
    txtUpdateE1 = "Update Funktion nicht verwenden";
    txtUpdateE2 = "Lokale Update Funktion verwenden";
    txtUpdateE3 = "Automatische Update Funktion via Internet verwenden";
    txtUpdate2 = "Über einen der folgenden Links kann die Uhr über den Browser ohne Arduino IDE aktualisiert werden";
    txtUpdate3 = "Hinweis: Es wird eine in der Arduino IDE mit Strg+Alt+S zuvor erstellte .BIN Datei des Sketches benötigt,";
    txtUpdate4 = "die über die Option 'Update Firmware' hochgeladen werden kann.";
    txtUpdate5 = "Die notwendige Update-Datei kann hier heruntergeladen werden";
    txtUpdate6 = "Uhr Repository auf GitHub";
    txtUpdate7 = "Die installierte Version entspricht der aktuell verfügbaren Version";
    txtUpdate8 = "Ein Update ist verfügbar auf Version";
    txtUpdate9 = "Verwende den folgenden Link um das Update zu starten";

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
  if (lang == 1) {         // ENGLISH
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
    txtPowerSupplyNote1 = "Important: Both values limited to 128 of maximum 255. Take care to use a suitable power supply!";
    txtPowerSupplyNote2 = "Depending on the amount of LEDs, selected color and intensity a 5V/3A power supply is recommended!";
    txtPowerSupplyNote3 = "I accept and observe the note on the power supply unit. I would like to be able to set the values ​​back to a maximum of 255";
    txtPowerSupplyNote4 = "Do not show the important note about the power supply again";
    txtFlashFullHour1 = "Flash full hour";
    txtFlashFullHour2 = "Flash the hour value every new hour?";
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
    txtUSEsetWLAN = "Show WIFI text on startup?";
    txtShowIP = "Show IP-address on startup?";
    txtRainbow1 = "Choose the rainbow color effect mode";
    txtRainbow2 = "Off";
    txtRainbow3 = "Variant 1 (words in different colours)";
    txtRainbow4 = "Variant 2 (all words randomly colored)";
    txtMinDir1 = "Minutes LEDs corners order clockwise?";
    txtMinDir2 = "If this option is set, the minute leds will be in the 4 corners";
    txtMinDir3 = "displayed clockwise, otherwise counterclockwise.";

    // Hostname:
    txtHostName1 = "Customize clock hostname";
    txtHostName2 = "Hostname";

    // REST functions:
    txtREST0 = "REST functions";
    txtREST1 = "clock functions can be controlled externally via the following links.";
    txtREST2 = "Use REST function?";
    txtREST3 = "The clock can be switched on and off manually via the browser via one of the following links";
    txtREST4 = "Turn off LEDs";
    txtREST5 = "Turn on LEDs";
    txtREST6 = "LED state";

    // Update function:
    txtUpdate0 = "Update";
    txtUpdateE1 = "Do not use the update function";
    txtUpdateE2 = "Use local update function";
    txtUpdateE3 = "Use automatic update function via internet";
    txtUpdate2 = "Using one of the links below, the clock can be updated via the browser without the Arduino IDE";
    txtUpdate3 = "Note: A .BIN file of the sketch previously created in the Arduino IDE with Ctrl+Alt+S is required,";
    txtUpdate4 = "which can be uploaded via the 'Update Firmware' option.";
    txtUpdate5 = "The necessary update file can be downloaded here";
    txtUpdate6 = "clock repository on GitHub";
    txtUpdate7 = "The installed version is the same as the available version";
    txtUpdate8 = "Update available to version";
    txtUpdate9 = "Use the following link to start the update";

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
