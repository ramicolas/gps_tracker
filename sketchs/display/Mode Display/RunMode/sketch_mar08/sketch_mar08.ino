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
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   4
#define OLED_CLK   5
#define OLED_DC    6
#define OLED_CS    0
#define OLED_RESET 7
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2



//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif


int ModePin = 6; 
//int StartPin = ;
int modeval = 0; 

void setup()   {        

  pinMode(ModePin, INPUT);  
//  pinMode(StartPin, INPUT);  
 
          
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
  display.println("Test Mode Display");
  display.display();
  delay(2000);
  display.clearDisplay();
  // text display tests
}


void loop() {
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("02:13:56");

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,15);
  display.println("36.2K");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(95,0);
  display.println("10sat");

  display.fillRect(102, 9, 15, 4,WHITE);
  display.fillRect(117, 10, 2, 2,WHITE);


  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(75,15);
  display.println("5:24");


  
  display.display();
  delay(1000);
  display.clearDisplay();



}



