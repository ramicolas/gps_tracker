/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98
This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface
Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!
Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   4
#define OLED_CLK   5
#define OLED_DC    6
#define OLED_CS    7
#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/


int ModePin = 2; 
int StartPin = 3;


void setup()   {        

  pinMode(ModePin, INPUT);  
  pinMode(StartPin, INPUT);  
 
          
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  // Clear the buffer.
  display.clearDisplay();


  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.println("Menu Navigation V1");
  display.display();
  delay(2000);
  display.clearDisplay();
  // text display tests
}


void loop() {

  int m = 0;

  while (digitalRead(StartPin) == LOW) {
    
    if (digitalRead(ModePin) == HIGH) {   
      m++;
      if (m==2) {
        m=0;
      }
      delay(300);
    }
    if (m == 0) {   
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Run");
      display.display();
      display.clearDisplay();
    } else {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Bike");
      display.display();
      display.clearDisplay();
    }
  }
  delay(200);
  switch (m) {
    case 0:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Start Runing");
      display.display();
      delay(2000);
      display.clearDisplay();
      break;
    case 1:
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

