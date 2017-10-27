
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
  mySerial.println(PMTK_Q_RELEASE);

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
  delay(2000);
  display.clearDisplay();
  // text display tests
}

int m = 0;

void loop() {

  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
 

  while (digitalRead(StartPin) == LOW) {
    if (digitalRead(ModePin) == HIGH) {   
      m++;
      if (m==3) 
        m=0;
      
      delay(300);
    }
    if (m == 1) {   
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Run");
      display.display();
      display.clearDisplay();
    } 
    else if (m == 2) {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Bike");
      display.display();
      display.clearDisplay();
    }
    else if (m == 0){
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);

      display.print("Time: ");
      display.print(GPS.hour, DEC); display.print(':');
      display.print(GPS.minute, DEC); display.print(':');
      display.print(GPS.seconds, DEC); display.print('.');
      display.println(GPS.milliseconds);
      display.print("Date: ");
      display.print(GPS.day, DEC); display.print('/');
      display.print(GPS.month, DEC); display.print("/20");
      display.println(GPS.year, DEC);
      display.print("Fix: "); display.print((int)GPS.fix);
      display.print(" quality: "); display.println((int)GPS.fixquality); 
      display.display();
      display.clearDisplay();
        }
  }
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

}


void run()
{
  uint32_t timer = millis();
//  if (timer > millis())  timer = millis();
  while (digitalRead(StartPin) == LOW){
    char c = GPS.read();
  // if you want to debug, this is a good time to do it!
    if ((c) && (GPSECHO))
      Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
      if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
        return;  // we can fail to parse a sentence in which case we should just wait for another
    }

  // if millis() or timer wraps around, we'll just reset it
    if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
    if (millis() - timer > 1500) { 
      timer = millis(); // reset the timer

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);

      display.print("Time: ");
      display.print(GPS.hour, DEC); display.print(':');
      display.print(GPS.minute, DEC); display.print(':');
      display.print(GPS.seconds, DEC); display.print('.');
      display.println(GPS.milliseconds);
      display.print("Date: ");
      display.print(GPS.day, DEC); display.print('/');
      display.print(GPS.month, DEC); display.print("/20");
      display.println(GPS.year, DEC);
      display.print("Fix: "); display.print((int)GPS.fix);
      display.print(" quality: "); display.println((int)GPS.fixquality); 
      display.display();
      display.clearDisplay();
      if (GPS.fix) {

        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
      
      //display.print("Lat: ");
     // display.print(GPS.latitude, 4); //display.print(GPS.lat);
     // display.println("Lon:"); 
      //display.println(GPS.longitude, 4); display.print(GPS.lon);     
        display.print("Speed: "); display.println(GPS.speed*1.852);
        display.print("Alti(m):"); display.println(GPS.altitude/100);
        display.print("Sat:"); display.println((int)GPS.satellites);
        display.display();
        display.clearDisplay();
      }
    }
  }
 
/*  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("02:13:56");

  display.setCursor(0,15);
  display.println("36.2K");

  display.setTextSize(1);
  display.setCursor(95,0);
  display.println("10sat");

  display.fillRect(102, 9, 15, 4,WHITE);
  display.fillRect(117, 10, 2, 2,WHITE);


  display.setTextSize(2);
  display.setCursor(75,15);
  display.println("5:24");

  
  display.display();
  delay(1000);
  display.clearDisplay();
*/

}
void fixi()
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
 
}

