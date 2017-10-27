#include <Fat16.h>

// Version 1.7 light version of 1.3 with Julian display, and try fat. Modified to run on Arduino Leonardo/Micro. Saving coordinates in txt files as a gpx file (have to then modifies the name)

#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>

#include <Fat16.h>
#include <Fat16util.h> 

const uint8_t CHIP_SELECT = 10;  // SD card chip select pin.
SdCard card;
Fat16 myFile;


#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#include "font.h";

SoftwareSerial mySerial(7, 9);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  true

#define PIN_CS    0
#define PIN_DC    6
#define PIN_RESET 8
#define PIN_SDIN  4
#define PIN_SCLK  5

// Buttons
int ModePin = 2; 
int StartPin = 3;


char tmp[8];

void oledWriteString(char *characters)
{
  while (*characters) oledWriteCharacter(*characters++);
}

void oledWriteCharacter(char character)
{
  for (int i=0; i<5; i++) oledWriteData(pgm_read_byte(&ASCII[character - 0x20][i]));
  oledWriteData(0x00);
}

void oledWriteData(byte data)
{
  digitalWrite(PIN_DC, HIGH);
  digitalWrite(PIN_CS, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_CS, HIGH);
}

void oledWriteCmd(byte command)
{
  digitalWrite(PIN_DC, LOW);
  digitalWrite(PIN_CS, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, command);
  digitalWrite(PIN_CS, HIGH);
}

void setup()   {  


  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
//  digitalWrite(PIN_RESET, HIGH);
  digitalWrite(PIN_RESET, LOW);
  delay(1);
  digitalWrite(PIN_RESET, HIGH);
  delay(1);
  
  oledWriteCmd(0x8d); //enable charge pump
  oledWriteCmd(0x14);
  delay(1);
  oledWriteCmd(0xaf); //set display on

  oledWriteCmd(0xd3); //set display offset
  oledWriteCmd(0x00);
  oledWriteCmd(0x40); //set display start line
  oledWriteCmd(0xa1); //set segment re-map (horizontal flip) - reset value is 0xa0 (or 0xa1)
  oledWriteCmd(0xc8); //set COM output scan direction (vertical flip) - reset value is 0xc0 (or 0xc8)
  oledWriteCmd(0xda); //set COM pins hardware configuration
  oledWriteCmd(0x12); //reset value is 0x12
  oledWriteCmd(0x81); //set contrast (2-byte)
  oledWriteCmd(0xff);



  for (int i=0; i<1024; i++) oledWriteData(0x00); // clear oled screen 
  oledWriteCmd(0x20); //set page addressing mode
  oledWriteCmd(0x02);
  oledWriteCmd(0xb4); //set page start address to page 7
  oledWriteCmd(0x0); //set lower column start address
  oledWriteCmd(0x10); //set upper column start address
  
  oledWriteString("..WAIT..");
  
  card.begin(CHIP_SELECT);
  Fat16::init(&card);
//  Serial.begin(115200);
  delay(5000);
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

}



int n = 0; // Start
int l = 0;
int m = 0;

uint32_t t0;
uint32_t timer = millis();

double lon;
double lat;
double lon2;
double lat2;

float distance = 0;
//float elevationpos = 0;
//float elevation = 0;
//float altitude0;
//int paceint = 0;
//int pacedec = 0;

// define chrono
int sc = 0;
int mc = 0;
int hc = 0;


void loop() {
  
  char c = GPS.read();

  // if you want to debug, this is a good time to do it!
//  if ((c) && (GPSECHO))
//    Serial.write(c); 
  
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
    m=1;
    delay(300);
  }  
  if (timer > millis())  timer = millis();
  
  // approximately every 1 seconds or so, print out the current stats
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the time
    
    //if (GPS.fix){
    // do stuffs
   //}

    // **** MODE MENU **** with display of hour and status of GPS with quality
    //if (n == 2){
      //Pause
    //}

    if (m == 1 && n == 2){
      myFile.println("  </trkseg>");   
      myFile.println(" </trk>");
      myFile.println("</gpx>");

      
      n = 0;
      m = 0;
  
      oledWriteCmd(0xb4); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address
  
      oledWriteString("STOPED");
      delay(2000);
    }

    if (n == 0){

    //display time
    
      if (GPS.fix){
        
        oledWriteCmd(0xb4); //set page start address to page 7
        oledWriteCmd(0x0); //set lower column start address
        oledWriteCmd(0x10); //set upper column start address
  
        oledWriteString("READY!GOGOGO!");
      }
    }
    
    // ***** MODE RUN ***** starts if gps fix
    if (n==1 && GPS.fix){
      // define t0 time when runs for the first time and create the chronometer
      if (l==0) {
        t0 = millis();
        // FIRST TIME measure first lon and lat
        lon = convertDegMinToDecDeg(GPS.longitude);
        if (GPS.lon=='W') lon = (-1)*lon;
        lat = convertDegMinToDecDeg(GPS.latitude);
        if (GPS.lat=='S') lat = (-1)*lat;
    
        myFile.open("test00.txt", O_CREAT | O_EXCL | O_WRITE);
        myFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        myFile.println("<gpx creator=\"strava.com Android\">");
        myFile.println(" <metadata>");
        myFile.print("  <time>20"); myFile.print(GPS.year,DEC); myFile.print('-'); if (GPS.month<10) myFile.print('0'); myFile.print(GPS.month,DEC); myFile.print("-");if (GPS.day<10) myFile.print('0'); myFile.print(GPS.day,DEC); myFile.print('T'); if (GPS.hour<10) myFile.print('0'); myFile.print(GPS.hour,DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
        myFile.println(" </metadata>");
        myFile.println(" <trk>");
        myFile.println("  <name>Run</name>");
        myFile.println("  <trkseg>");    
           
//       for (int i=0; i<1024; i++) oledWriteData(0x00); // clear oled screen 
//        oledWriteCmd(0x20); //set page addressing mode
//        oledWriteCmd(0x02);
        l++;
  //      altitude0 = GPS.altitude;
      }
      sc = (int)((millis() -  t0)/1000 + 0.5)-mc*60-hc*60*60;
      if (sc == 60){
        sc = 0;
        mc++;
        // p++
      }
      if (mc == 60){
        mc = 0;
        hc++;
      }
      // Calculate distance with the previous one and other parameters

      lon2 = convertDegMinToDecDeg(GPS.longitude);
      if (GPS.lon=='W') lon2 = (-1)*lon2;
      lat2 = convertDegMinToDecDeg(GPS.latitude);
      if (GPS.lat=='S') lat2 = (-1)*lat2;
      
      distance = distance + distancebtw(lon,lat,lon2,lat2);
//      paceint = (int)(((int)((millis() -  t0)/1000 + 0.5))/60/distance);
//      pacedec = (int)((((int)((millis() -  t0)/1000 + 0.5))/60/distance-paceint)*6/10*100);

      myFile.print("   <trkpt lat=\""); myFile.print(lat2,7);  myFile.print("\" lon=\""); myFile.print(lon2,7); myFile.println("\">");
      myFile.print("    <ele>"); myFile.print(GPS.altitude/10,1); myFile.println("</ele>");
      myFile.print("    <time>20"); myFile.print(GPS.year,DEC); myFile.print('-'); if (GPS.month<10) myFile.print('0'); myFile.print(GPS.month,DEC); myFile.print("-");if (GPS.day<10) myFile.print('0'); myFile.print(GPS.day,DEC); myFile.print('T'); if (GPS.hour<10) myFile.print('0'); myFile.print(GPS.hour,DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
      myFile.println("   </trkpt>"); 

      // Reinitialize coordinates
      lon = lon2;
      lat = lat2; 

 //     elevation = altitude0 - GPS.altitude;
 //     if (elevation<0) elevation = 0;
//      elevationpos = elevation + elevationpos;

//      altitude0 = GPS.altitude;
/*
      oledWriteCmd(0xb0); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address

      if (hc<10) oledWriteString("0");
      oledWriteString(dtostrf(hc,1,0,tmp));
      oledWriteString(":");
      if (mc<10) oledWriteString("0");
      oledWriteString(dtostrf(mc,1,0,tmp));
      oledWriteString(":");
      if (sc<10) oledWriteString("0");
      oledWriteString(dtostrf(sc,1,0,tmp));


      oledWriteCmd(0xb3); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address
      oledWriteString("Distance:");
      oledWriteString(dtostrf(distance,1,1,tmp));
      oledWriteString("Km");

      oledWriteCmd(0xb4); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address
      oledWriteString("Pace:");
      oledWriteString(dtostrf(paceint,1,0,tmp));
      oledWriteString(":");
      if (pacedec<10) oledWriteString("0");
      oledWriteString(dtostrf(pacedec,1,0,tmp));
      
      oledWriteCmd(0xb5); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address
      oledWriteString("Altitude:");
      oledWriteString(dtostrf(GPS.altitude/10,1,0,tmp));
      oledWriteString("m");

      oledWriteCmd(0xb6); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address
      oledWriteString("Elevation+:");
      oledWriteString(dtostrf(elevationpos/10,1,0,tmp));
      oledWriteString("m");
*/
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
float distancebtw(double lon1, double lat1, double lon2, double lat2) {
  float d = sqrt( sq((lon2-lon1)*1000/57296*cos((lat1+lat2)*1000/57296/2)) + sq((lat2-lat1)*1000/57296) ) * 6371;
  return d;
}
