// Version 1.4 light version of 1.3 includes SD card libraries. Modified to run on Arduino Leonardo/Micro. Saving coordinates in txt files as a gpx file (have to then modifies the name)

#include <SPI.h>
#include <Wire.h>
#include <SD.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 9);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  true

// If using software SPI (the default case):
#define OLED_MOSI   4
#define OLED_CLK   5
#define OLED_DC    6
#define OLED_CS    0
#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Buttons
int ModePin = 2; 
int StartPin = 3;

File myFile;

void setup()   {        

  Serial.begin(115200);
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  pinMode(ModePin, INPUT);  
  pinMode(StartPin, INPUT);  
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  myFile = SD.open("test.txt", FILE_WRITE);
//  myFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
//  myFile.println("<gpx creator=\"strava.com Android\">");
}

int n = 0; // Start
int l = 0;
double lon2;
double lat2;
uint32_t t0;
uint32_t timer = millis();

void loop() {
  
  char c = GPS.read();
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  if (digitalRead(StartPin) == HIGH && GPS.fix) {   
    n++; //Pause
    if (n==3) //Resume
      n=1;  
    delay(300);
  }
  if (digitalRead(ModePin) == HIGH && n == 2) {
 //   myFile.print("  </trkseg>");   
 //   myFile.print(" </trk>");
 //   myFile.print("</gpx>");
    myFile.close();
    n = 0;
  }  
  if (timer > millis())  timer = millis();
  
  // approximately every 1 seconds or so, print out the current stats
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the time
    
    // add definition of new time with the zones and convert coordinates 
    if (GPS.fix){
    // do stuffs
    }

    // **** MODE MENU **** with display of hour and status of GPS with quality
    if (n == 0 || n == 2){
      display.setTextSize(1);
      display.setTextColor(WHITE);
      if (n==0) display.println("Mode Run");
      if (n==2) display.println("PAUSE");
      display.clearDisplay();
    }
    
    // ***** MODE RUN ***** starts if gps fix
    if (n==1 && GPS.fix){
      // define t0 time when runs for the first time and create the chronometer
/*      if (l==0) {
        myFile.println(" <metadata>");
        myFile.print("  <time>"); myFile.print(GPS.year,DEC); myFile.print('-'); if (GPS.month<10) myFile.print('0'); myFile.print(GPS.month,DEC); myFile.print("-");if (GPS.day<10) myFile.print('0'); myFile.print(GPS.day,DEC); myFile.print('T'); if (GPS.hour<10) myFile.print('0'); myFile.print(GPS.hour,DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
        myFile.println(" </metadata>");
        myFile.println(" <trk>");
        myFile.println("  <name>Run</name>");
        myFile.println("  <trkseg>");        
        l++;
      }
*/      
      // Calculate distance with the previous one and other parameters

      lon2 = convertDegMinToDecDeg(GPS.longitude);
      if (GPS.lon=='W') lon2 = (-1)*lon2;
      lat2 = convertDegMinToDecDeg(GPS.latitude);
      if (GPS.lat=='S') lat2 = (-1)*lat2;

      myFile.print(F("   <trkpt lat=\"")); myFile.print(lat2,7);  myFile.print(F("\" lon=\"")); myFile.print(lon2,7); myFile.println(F("\">"));
      myFile.print(F("    <ele>")); myFile.print(GPS.altitude/100,1); myFile.println(F("</ele>"));
      myFile.print("  <time>"); myFile.print(GPS.year,DEC); myFile.print('-'); if (GPS.month<10) myFile.print('0'); myFile.print(GPS.month,DEC); myFile.print("-");if (GPS.day<10) myFile.print('0'); myFile.print(GPS.day,DEC); myFile.print('T'); if (GPS.hour<10) myFile.print('0'); myFile.print(GPS.hour,DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
      myFile.println(F("   </trkpt>"));    
    }
  }
}
double convertDegMinToDecDeg (float degMin) {
  double min = 0.0;
  double decDeg = 0.0;
  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60 );
  return decDeg;
}
