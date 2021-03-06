// Version 2.0



// ****************************************************************************************************************************
// ********************************************** INCLUDE LIBRARIES AND DEFINE PINS *******************************************
// ****************************************************************************************************************************

#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>
//#include <SD.h>
#include "SdFat.h"
SdFat SD;

// GPS
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 9);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  true



// font.h defines characters for display
#include "font.h";

// Display
#define PIN_CS    0
#define PIN_DC    6
#define PIN_RESET 8
#define PIN_SDIN  4
#define PIN_SCLK  5

// Buttons
int ModePin = 2; 
int StartPin = 3;

File myFile;


// ***************************************************************************************************************
// *******************************************   SETUP DISPLAY ***************************************************
// ***************************************************************************************************************

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

void lave(){
  for (int i=0; i<1024; i++) oledWriteData(0x00); // clear oled screen 
}
void setline(byte data)
{
      oledWriteCmd(data); //set page start address to page 7
      oledWriteCmd(0x0); //set lower column start address
      oledWriteCmd(0x10); //set upper column start address
}


// *************************************************************************************************************************************************
// ********************************************************** VOID SETUP ***************************************************************************
// *************************************************************************************************************************************************

void setup()   {  

// ************** DISPLAY ****************************
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

  setline(0xb4);
  oledWriteString("   ..WAIT..   ");


// ***************************** BEGIN SD AND GPS AND BUTTON *****************************  
  SD.begin(10);
  //Serial.begin(115200);
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

  //EEPROM.write(0, 20);

}

// **************************************************************************************************
// ************************************** MAIN VARIABLES ********************************************
// **************************************************************************************************

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
float elevationpos = 0;
float elevation = 0;
float altitude0;
int paceint = 0;
int pacedec = 0;

// define chrono
int sc = 0;
int mc = 0;
int hc = 0;

int filein; 

// ****************************************************************************************************************
// ****************************************************************************************************************
// ***************************************************** LOOP START ***********************************************
// ****************************************************************************************************************
// ****************************************************************************************************************

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
    n++; // Start if n = 1, Pause if n = 2
    if (n==3) //Resume
      n=1;  
    delay(300);
  }
  if (digitalRead(ModePin) == HIGH) {
    if (n == 2) m=1; // if paused it stops
    delay(50);                                                                                                                      //??
    if (n == 1){
      setline(0xb0);
      if (hc<10) oledWriteString("0");
      oledWriteString(dtostrf(hc,1,0,tmp));
      oledWriteString(":");
      if (mc<10) oledWriteString("0");
      oledWriteString(dtostrf(mc,1,0,tmp));
      oledWriteString(":");
      if (sc<10) oledWriteString("0");
      oledWriteString(dtostrf(sc,1,0,tmp));

      setline(0xb2);
      oledWriteString("Distance:");
      oledWriteString(dtostrf(distance,1,1,tmp));
      oledWriteString("Km");

      setline(0xb4);
      oledWriteString("Pace:");
      oledWriteString(dtostrf(paceint,1,0,tmp));
      oledWriteString(":");
      if (pacedec<10) oledWriteString("0");
      oledWriteString(dtostrf(pacedec,1,0,tmp));
      
      setline(0xb5);
      oledWriteString("Altitude:");
      oledWriteString(dtostrf(GPS.altitude,1,0,tmp));
      oledWriteString("m");

      setline(0xb6);
      oledWriteString("ElevationP:");
      oledWriteString(dtostrf(elevationpos,1,0,tmp));
      oledWriteString("m");
      
      setline(0xb7);
      oledWriteString(dtostrf(GPS.satellites,1,0,tmp));
      oledWriteString(" satellites");
      
      l = 1; // Start counter to display 10 sec
    }
  }
  if (timer > millis())  timer = millis();
  // ****************************************************************************************************************
  // ********************************************************  EVERY SECOND *****************************************
  // ****************************************************************************************************************
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the time
    
    // ****************************************** MODE RUN **********************************************************
    if (n==1 && GPS.fix){
      // define t0 time when runs for the first time and create the chronometer
      if (l==0) {
        filein = EEPROM.read(0);
        delay(10);
        String str = String(filein)+".txt";
        myFile = SD.open(str, O_CREAT | O_WRITE | O_APPEND);

        /*myFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        myFile.println("<gpx creator=\"strava.com Android\">");
        myFile.println(" <metadata>");
        myFile.print("  <time>20"); myFile.print(GPS.year,DEC); myFile.print('-'); if (GPS.month<10) myFile.print('0'); myFile.print(GPS.month,DEC); myFile.print("-");if (GPS.day<10) myFile.print('0'); myFile.print(GPS.day,DEC); myFile.print('T'); if (GPS.hour<10) myFile.print('0'); myFile.print(GPS.hour,DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
        myFile.println(" </metadata>");
        myFile.println(" <trk>");
        myFile.println("  <name>Run</name>");
        myFile.println("  <trkseg>");
        myFile.flush();*/
        
        t0 = millis();
        // FIRST TIME measure first lon and lat
        lon = convertDegMinToDecDeg(GPS.longitude);
        if (GPS.lon=='W') lon = (-1)*lon;
        lat = convertDegMinToDecDeg(GPS.latitude);
        if (GPS.lat=='S') lat = (-1)*lat;   
        altitude0 = GPS.altitude;
        setline(0xb4); 
        oledWriteString("  GOGOGOGOGO  ");
      }
      // Display counter
      l++;  
      
      if (l==10){
        setline(0xb0);
        lave();
        setline(0xb1);
        lave();
        setline(0xb2);
        lave();
        setline(0xb3);
        lave();
        setline(0xb4);
        lave();
        setline(0xb5);
        lave();
        setline(0xb6);
        lave();
        setline(0xb7);
        lave();
      }
      
      // Chronometer
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
      paceint = (int)(((int)((millis() -  t0)/1000 + 0.5))/60/distance);
      pacedec = (int)((((int)((millis() -  t0)/1000 + 0.5))/60/distance-paceint)*6/10*100);

      //Serial.print(lat2,7);  Serial.print(";"); Serial.print(lon2,7); Serial.print(";");
      //Serial.print(GPS.altitude,1); Serial.print(";");
      //Serial.print(GPS.year,DEC); Serial.print(";"); Serial.print(GPS.month,DEC); Serial.print(";"); Serial.print(GPS.day,DEC); Serial.print(";"); Serial.print(GPS.hour,DEC); Serial.print(";"); Serial.print(GPS.minute, DEC); Serial.print(";"); Serial.println(GPS.seconds, DEC);
      
     
      myFile.print(lat2,7); myFile.print(","); 
      myFile.print(lon2,7); myFile.print(","); 
      myFile.print(GPS.altitude,1); myFile.print(",");
      myFile.print(GPS.year,DEC); myFile.print(","); 
      myFile.print(GPS.month,DEC); myFile.print(",");
      myFile.print(GPS.day,DEC); myFile.print(","); 
      myFile.print(GPS.hour,DEC); myFile.print(","); 
      myFile.print(GPS.minute, DEC); myFile.print(","); 
      myFile.println(GPS.seconds, DEC);
      myFile.flush(); 

      /*myFile.print("   <trkpt lat=\""); myFile.print(lat2,7);  myFile.print("\" lon=\""); myFile.print(lon2,7); myFile.println("\">");
      myFile.print("    <ele>"); myFile.print(GPS.altitude,1); myFile.println("</ele>");
      myFile.print("    <time>20"); myFile.print(GPS.year,DEC); myFile.print('-'); if (GPS.month<10) myFile.print('0'); myFile.print(GPS.month,DEC); myFile.print("-");if (GPS.day<10) myFile.print('0'); myFile.print(GPS.day,DEC); myFile.print('T'); if (GPS.hour<10) myFile.print('0'); myFile.print(GPS.hour,DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
      myFile.println("   </trkpt>"); 
      myFile.flush();
      */
      // Reinitialize coordinates
      lon = lon2;
      lat = lat2; 

      elevation = altitude0 - GPS.altitude;
      if (elevation<0) elevation = 0;
      elevationpos = elevation + elevationpos;

      altitude0 = GPS.altitude;
    }

    // ************************ DISPLAY READY TO GO *********************
    else if (n == 0 && GPS.fix){
      setline(0xb4); 
      oledWriteString("       READY!      "); //add gps quality
      setline(0xb7);
      oledWriteString(dtostrf(GPS.satellites,1,0,tmp));
      oledWriteString(" satellites");
    }

    // ************************* DISPLAY PAUSE ***************************
    else if (m == 0 && n == 2){
      setline(0xb4);
      oledWriteString("       PAUSED     ");
    }

    // ************************* DISPLAY STOPED AND CLOSE FILE ***********
    else if (m == 1 && n == 2){
      
      setline(0xb4);
      oledWriteString("       STOPED     ");
      delay(10);
      /*myFile.println("  </trkseg>");   
      myFile.println(" </trk>");
      myFile.println("</gpx>");*/
      myFile.close();
      delay(2000);
      EEPROM.write(0, filein+1);
      n = 0;
      m = 0;
      l = 0;
      elevationpos = 0;
      distance = 0;
    }
  }
}

// ****************************************************************************************************************
// ************************************************** EXTRA FUNCTIONS *********************************************
// ****************************************************************************************************************
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
