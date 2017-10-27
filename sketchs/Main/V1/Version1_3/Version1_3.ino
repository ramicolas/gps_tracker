// Version 1.3 includes SD card libraries. Modified to run on Arduino Leonardo/Micro. Saving coordinates in txt files as a gpx file (have to then modifies the name)


#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>
#include <SD.h>

//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>



SoftwareSerial mySerial(7, 9);
Adafruit_GPS GPS(&mySerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
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

int utc;

File myFile;

void setup()   {        

  Serial.begin(115200);
  delay(5000);


  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  //mySerial.println(PMTK_Q_RELEASE);

  pinMode(ModePin, INPUT);  
  pinMode(StartPin, INPUT);  


  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Clear the buffer.
  display.clearDisplay();


  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.println(F("GPS Version 1.3"));
  display.display();
  delay(3000);
  display.clearDisplay();


  if (digitalRead(ModePin) == HIGH){
    utctime();
  }

  delay(2000);
  utc = EEPROM.read(0);
  
  myFile = SD.open("test.txt", FILE_WRITE);
  myFile.println(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
  myFile.println(F("<gpx creator=\"strava.com Android\">"));

}

//int m = 0; // Mode
int n = 0; // Start
//int k = 0; // Counter mode
int l = 0;

double lon;
double lat;
double lon2;
double lat2;


float distance = 0;
int paceint = 0;
int pacedec = 0;
float paceinst = 0;

// define chrono
int sc = 0;
int mc = 0;
int hc = 0;

//int* date;

uint32_t t0;
uint32_t timer = millis();


// ********************* LOOP *************************

void loop() {

// ******* GPS READ *******  
  char c = GPS.read();


  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c); 

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  /*
  // Change mode pressing Mode
  if (digitalRead(ModePin) == HIGH && n == 0) {   
    m++;
    if (m==3) 
      m=0;  
    delay(300);
  }
  // Counter for other display mode
  if (digitalRead(ModePin) == HIGH && n == 1){
    k = 0;
  }

  */
  
  // Select mode pressing start
  if (digitalRead(StartPin) == HIGH && GPS.fix) {   
    n++; //Pause
    if (n==3) //Resume
      n=1;  
    delay(300);
  }

  if (digitalRead(ModePin) == HIGH && n == 2) {
    myFile.println(F("  </trkseg>"));   
    myFile.println(F(" </trk>"));
    myFile.println(F("</gpx>"));
    myFile.close();
    n = 0;
    delay(300);
  }  
  
  
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();
  
// ****** EVERY SECOND *******
  // approximately every 1 seconds or so, print out the current stats
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the time

    
    int* date = dateutc(GPS.year,GPS.month,GPS.day,GPS.hour,utc);
    
    // add definition of new time with the zones and convert coordinates 
    if (GPS.fix){
    // do stuffs
    }

    // **** MODE MENU **** with display of hour and status of GPS with quality
    if (n == 0 || n == 2){

      
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);

      display.print(F("Time: "));
      if (date[3]<10) display.print('0'); display.print(date[3], DEC); display.print(':');
      if (GPS.minute <10) display.print('0'); display.print(GPS.minute, DEC); display.print(':');
      if (GPS.seconds <10) display.print('0'); display.println(GPS.seconds, DEC);
      if (n==0) display.println(F("Mode Run"));
      if (n==2) display.println(F("PAUSE"));

      if (GPS.fix){
        display.print(F("Ready! Quality:")); display.println((int)GPS.fixquality); 
        display.display();
        display.clearDisplay();
      }
      else{
        display.print(F("Wait for fix..."));  
        display.display();
        display.clearDisplay();
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

        myFile.println(F(" <metadata>"));
        myFile.print(F("  <time>")); myFile.print(date[0],DEC); myFile.print("-"); if (date[1]<10) myFile.print('0'); myFile.print(date[1],DEC); myFile.print("-");if (date[2]<10) myFile.print('0'); myFile.print(date[2],DEC); myFile.print('T'); if (date[3]<10) myFile.print('0'); myFile.print(date[3],DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
        myFile.println(F(" </metadata>"));
        myFile.println(F(" <trk>"));
        myFile.println(F("  <name>Run</name>"));
        myFile.println(F("  <trkseg>"));        
        l++;
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
      paceint = (int)(((int)((millis() -  t0)/1000 + 0.5))/60/distance);
      pacedec = (int)((((int)((millis() -  t0)/1000 + 0.5))/60/distance-paceint)*6/10*100);


      // Reinitialize new coordinates
      lon = lon2;
      lat = lat2;
      
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      if(hc<10)display.print('0'); display.print(hc, DEC); display.print(':');
      if(mc<10)display.print('0');display.print(mc, DEC); display.print(':');
      if(sc<10)display.print('0');display.println(sc, DEC);

      display.setCursor(0,15);
      display.print(distance,2); display.println("K"); //DIST

      display.setTextSize(1);
      display.setCursor(95,0);
      display.print((int)GPS.satellites);display.println("sat");

      display.fillRect(102, 9, 15, 4,WHITE);
      display.fillRect(117, 10, 2, 2,WHITE);


      display.setTextSize(2);
      display.setCursor(75,15);
      display.print(paceint); display.print(':');
      if(pacedec<10)display.print('0');display.print(pacedec);

  
      display.display();
      display.clearDisplay();

      myFile.print(F("   <trkpt lat=\"")); myFile.print(lat2,7);  myFile.print(F("\" lon=\"")); myFile.print(lon2,7); myFile.println(F("\">"));
      myFile.print(F("    <ele>")); myFile.print(GPS.altitude/100,1); myFile.println(F("</ele>"));
      myFile.print(F("    <time>")); myFile.print(date[0],DEC); myFile.print("-"); if (date[1]<10) myFile.print('0'); myFile.print(date[1],DEC); myFile.print("-");if (date[2]<10) myFile.print('0'); myFile.print(date[2],DEC); myFile.print('T'); if (date[3]<10) myFile.print('0'); myFile.print(date[3],DEC); myFile.print(':'); if (GPS.minute <10) myFile.print('0'); myFile.print(GPS.minute, DEC); myFile.print(':'); if (GPS.seconds <10) myFile.print('0'); myFile.print(GPS.seconds, DEC); myFile.println("Z</time>");
      myFile.println(F("   </trkpt>"));
      
      lon = convertDegMinToDecDeg(GPS.longitude);
      if (GPS.lon=='W') lon = (-1)*lon;
      lat = convertDegMinToDecDeg(GPS.latitude);
      if (GPS.lat=='S') lat = (-1)*lat;
      
    }


  }
}



void utctime(){
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("UTC");
  display.display();
  display.clearDisplay(); 
  delay(1000);
  int n = 0;
  int i = 0;
  while (digitalRead(StartPin) == LOW) {
    if (digitalRead(ModePin) == HIGH) {
      n++;
      i++;
      if (n == 27) n = 0;
      if (i == 15) i = -12;
      delay(300);   
    }
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("UTC  ");
    if (i>0) display.print("+");
    display.println(i);
    display.display();
    display.clearDisplay(); 
  }
  EEPROM.update(0, n);
  delay(300);
  display.display();
  display.clearDisplay(); 
}


// function input hour day month year utc time and return corrected time and date
int * dateutc(int year,int month,int day,int hour,int utc) {
  int y, m, d, h;
  static int val[4];
  if (utc>14){
    utc = utc -12-15;
  }
  y = year;
  m = month;
  d = day;
  h = hour+utc;
  if(h<0){
    h=h+24;
    d = d-1;
    if (d<1){
      d = 31;
      m = m-1;
      if (m<1){
        m=12;
        y=y-1;
      }
    }
  }
  if(h>23){
    h=h-24;
    d = d+1;
    if (d>31){
      d = 1;
      m = m+1;
      if (m>12){
        m=1;
        y=y+1;
      }
    }
  }
  val[0] = y;
  val[1] = m;
  val[2] = d;
  val[3] = h;
  return val;
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

// function input two coordinates and return distance between the two in km

float distancebtw(double lon1, double lat1, double lon2, double lat2) {
  float d = sqrt( sq((lon2-lon1)*1000/57296*cos((lat1+lat2)*1000/57296/2)) + sq((lat2-lat1)*1000/57296) ) * 6371;
  return d;
}

