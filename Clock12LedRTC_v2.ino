#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include "RTClib.h"

#define PIN        7 // DataPIN on Arduino Uno
#define NUMPIXELS 12 // Total Number of Pixels for Clock
#define ButtonPIN 4 // Tectile button pin number


/**
 * Initialize a new OneButton instance for a button
 * connected to digital pin 4 and GND, which is active low
 * and uses the internal pull-up resistor.
 */

OneButton btn = OneButton(
  ButtonPIN,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
// Define DS1307 RTC
/*
 * https://lastminuteengineers.com/ds1307-rtc-arduino-tutorial/
 * PINOUT : GND,5V,SCL=A5, SDA=A4 of arduino
 */

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

uint32_t magenta = pixels.Color(120,255, 120);
uint32_t green = pixels.Color(0, 80, 0); 
uint32_t blue  = pixels.Color(0,0,120); 
uint32_t red   = pixels.Color(80,0,0); 
uint32_t white = pixels.Color(80,80,80);

int hr = 0; // Hour
int mn = 0; // Minute
int ss = 0; // Second
int yy = 0; // Year
int mm = 0; // Month
int dd = 0; // Day
boolean IsDisplaySerial = true; // Set to true for Get/Set Date time in yymmddhhmmss format as input to serial communication ...

String hrs = "";

void setup() {


  // put your setup code here, to run once:
  pixels.begin();
  //pixels.fill(color, first, count);

  pixels.fill(white,0); // Fills all pixles with white color as indication of start of clock
  pixels.setBrightness(127); // Set brightness to half for all pixels, this can be changed according to daylight ...
  pixels.show(); 
  delay(100);
  pixels.clear(); // Show all 'off' so that previous shown will be cleared
  delay(100);
  // uint32_t color = strip.getPixelColor(11); // Get previously set color of pixel
  // uint16_t n = strip.numPixels();  // How many pixels is previously set
  // strip.setBrightness(64); // Set brightness to 1/4 from 0...255
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) 
  {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2021,2,18,18,32,0));
  }


}

// Handler function for a single click:
static void handleClick() {
  Serial.println("Clicked!");
}

// Single Click event attachment
btn.attachClick(handleClick);

// Double Click event attachment with lambda
btn.attachDoubleClick([]() {
  Serial.println("Double Pressed!");
});

void loop() 
{

const int HourFormat12 = 12;

DateTime now = rtc.now();

hr = (now.hour()%HourFormat12) - 0;  // this will be now.hour() if following if (hr != 12) is active 
mn = (((now.minute()+4)/5)%HourFormat12) - 0; // Turn into 5 minute interval (For 12 pixels) and to zero base
ss = (((now.second()+4)/5)%HourFormat12) - 0; // Turn into 5 second interval (For 12 pixels) and to zero base

whiteColor(hr,mn,ss); // Show 12,3,6,9 as white indication color for better visual clock

btn.tick();

/*
 * 
    // in case you need to change each 5 minute hands with style

    if (hr != 12)
      { hr %= 12;}
    else 
      { hr = 12; }

    // hr = hr - 1; // non zero based led display change according to your need;
    // mn = mn - 1;
    // ss = ss - 1;

 */

    if (hr == mn)
      {
        pixels.setPixelColor(hr, magenta); // 0,70,125 If hour and minute is on same pixel, show different color
        //pixels.show(); // Avoide multiple show
      }
    else
      {
        pixels.setPixelColor(hr, green); //  Show hour in green color
        pixels.setPixelColor(mn, blue); // Show minute in blue color 
        //pixels.show(); // Avoide multiple show
      }
    pixels.setPixelColor(ss, red); // Show second in red color
   // Turn on following line for any fancy effect you may want to change it to ...
   // pixels.fill(red,ss-1,3); // (color,first,count) fill red color with previous second, current second, next second
    pixels.show(); // Show pixels 
    delay(100); // Delay for better view and time to show pixels

if (IsDisplaySerial)
    {
        // Print current date and time before change of serial input
        Serial.print(now.year(), DEC);
        Serial.print('/');
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);
        Serial.print(" (");
        Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
        Serial.print(") ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.print(' hr:');
        Serial.print(hr);
        Serial.print(' mn:');
        Serial.print(mn);
        Serial.print(' ss:');
        Serial.print(ss);
        Serial.println();

        // Serial.println("Check - 01\n");
        if (Serial.available() > 0){
        
          // Serial.print("Check - 02\n");
        
              hrs = Serial.readString(); // Read serial input and parse it to year, month, date, hour, minute and second each two character input in serial monitor
          //  hrs.trim(); // In case you may have entered spaces trailing or preceeding ...
            
            yy = hrs.substring(0,2).toInt(); // Get 1st two characters as year
            mm = hrs.substring(2,4).toInt(); // 3rd,4th two characters as month
            dd = hrs.substring(4,6).toInt(); // 5th,6th two characters as day
    
            hr = hrs.substring(6,8).toInt();    // 7th,8th two character as hour
            mn = hrs.substring(8,10).toInt();   // 9th,10th two character as minute
            ss = hrs.substring(10,12).toInt();  // 11th,12th two character as second
            Serial.print("Input String : " );
            Serial.println(hrs);
            
            rtc.adjust(DateTime(yy,mm,dd,hr,mn));
  
          }
          delay(10);
          Serial.flush();
       // Serial.print(" since midnight 1/1/1970 = ");
       // Serial.print(now.unixtime());
       // Serial.print("s = ");
       // Serial.print(now.unixtime() / 86400L);
       // Serial.println("d");
       //  pixels.clear();
    } // If serial display is turned on
 
  pixels.clear(); // Clear/Off all pixels 
  delay(100); // Wait for better time for pixels
}

void whiteColor(int hr,int mn,int ss)
// Show 12,3,6,9 in clock as white for indication 
{
    uint32_t white = pixels.Color(30,30,30);
    pixels.clear();
    pixels.setPixelColor(0,white); // 12 hour sign
    pixels.setPixelColor(3,(hr=3)?white:green); // 3  hour sign
    pixels.setPixelColor(6,(hr=6)?white:green); // 6  hour sign
    pixels.setPixelColor(9,(hr=9)?white:green); // 9  hour sign
    pixels.show();  // Show all hour visual sign
  }

 
