
#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
#define OLED_CS    10
#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Buttons
int ModePin = 2; 
int StartPin = 3;

int utc;
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
  display.println("GPS Version 1");
  display.display();
  delay(3000);
  display.clearDisplay();

  if (digitalRead(ModePin) == HIGH){
    utctime();
  }
  delay(2000);
  utc = EEPROM.read(0);
}

int m = 0; // Mode
int n = 0; // Start
int k = 0; // Counter mode
int l = 0;

// define hour with corrected utc
int sec;
int minu;
int hour;
int day;
int month;
int year;

// define chrono
int sc = 0;
int mc = 0;
int hc = 0;

int *date;

uint32_t t0;
uint32_t timer = millis();


void loop() {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  
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
  
  // Select mode pressing start
  if (digitalRead(StartPin) == HIGH) {   
    n++;
    if (n==2) 
      n=0;  
    delay(300);
  }
  
  
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();
  

  // approximately every 1 seconds or so, print out the current stats
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the time


    year = GPS.year;  //Serial.print("year"); Serial.println(year);
    month = GPS.month;// Serial.print("month"); Serial.println(month);
    day = GPS.day; //Serial.print("day"); Serial.println(day);
    hour = GPS.hour; //Serial.print("hour"); Serial.println(hour);
    minu = GPS.minute; 
    sec = GPS.seconds;


    hour = dateutc(year,month,day,hour,utc);
    //date = dateutc(16,03,17,hour,utc);
    
    // add definition of new time with the zones and convert coordinates 
    if (GPS.fix){
// do stuffs
    }

    // Mode Menu with display of hour and status of GPS with quality
    if (n == 0){
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);

      display.print("Time: ");
      if (hour<10) display.print('0'); display.print(hour, DEC); display.print(':');
      if (minu <10) display.print('0'); display.print(minu, DEC); display.print(':');
      if (sec <10) display.print('0'); display.println(sec, DEC);
      if (m == 0){
        display.println("Mode Run");
      }
      if (m == 1){
        display.println("Mode Cycling");
      }
      if (m == 2){
        display.println("Mode Test");
      }
      if (GPS.fix){
        display.print("Ready! Quality:"); display.println((int)GPS.fixquality); 
        display.display();
        display.clearDisplay();
      }
      else{
        display.print("Wait for fix...");  
        display.display();
        display.clearDisplay();
      }
    }
    // Mode Run starts if gps fix
    if (n==1 && m==0 && GPS.fix){
    
    
    
      // define t0 time when runs for the first time and create the chronometer
      if (l==0) t0 = millis();
      l++;
      sc = (int)((millis() -  t0)/1000 + 0.5)-mc*60-hc*60*60;
      if (sc == 60){
        sc = 0;
        mc++;
      }
      if (mc == 60){
        mc = 0;
        hc++;
      }
      // save coordinates and calculate distance with the previous one


      
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      if(hc<10)display.print('0'); display.print(hc, DEC); display.print(':');
      if(mc<10)display.print('0');display.print(mc, DEC); display.print(':');
      if(sc<10)display.print('0');display.println(sc, DEC);

      display.setCursor(0,15);
      display.println("36.2K");

      display.setTextSize(1);
      display.setCursor(95,0);
      display.print((int)GPS.satellites);display.println("sat");

      display.fillRect(102, 9, 15, 4,WHITE);
      display.fillRect(117, 10, 2, 2,WHITE);


      display.setTextSize(2);
      display.setCursor(75,15);
      display.println(60/(GPS.speed*1.852));

  
      display.display();
      display.clearDisplay();

      // save date and coordinates


      
    }
    if (n==1 && m==2 && GPS.fix){
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      
      Serial.print("Lat: ");
      Serial.print(GPS.latitude, 4); Serial.println(GPS.lat);
      Serial.print("Lon:"); 
      Serial.println(GPS.longitude, 4); Serial.println(GPS.lon);     
      display.print("Speed: "); display.println(GPS.speed*1.852);
      display.print("Alti(m):"); display.println(GPS.altitude/100);
      display.print("Sat:"); display.println((int)GPS.satellites);
      display.display();
      display.clearDisplay();
  }


    
/*        
  delay(200);
  switch (m) {
    case 1:
    
      while (digitalRead(ModePin) == LOW) {  
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("...GPS...");
        display.display();
        display.clearDisplay();  
        if (GPS.fix){
          while (digitalRead(StartPin) == LOW) {
            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(0,0);
            display.println("Ready?");
            display.display();
            display.clearDisplay();
          }
          delay(200);
          run();     
        }
      }
      break;
    case 2:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Start Cycling");
      display.display();
      delay(2000);
      display.clearDisplay();
      break;    
  }
*/
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
int dateutc(int year,int month,int day,int hour,int utc) {
  int y, m, d, h;
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
  static int val [4] = {y,m,d,h};
  return h;
}
// function input two coordinates and return distance between the two
