#include <Adafruit_NeoPixel.h> // To drive neo Pixels
#include <OneButton.h> // To get button events https://github.com/mathertel/OneButton
#include "RTClib.h" // To get Real Time from timer led_module
#include "SPI.h" // SPI arduino-library-use
#include "DMD.h" // To deive DMD (Dot Matrix Display 32x16) https://github.com/freetronics/DMD
#include <TimerOne.h> // To activate interupt https://github.com/PaulStoffregen/TimerOne
#include "SystemFont5x7.h" // Display font // DMD Library 
#include "Arial_black_16.h" // Display font // DMD Library 

#define DMDRows 1 // Total Modules in Rows
#define DMDColumns 1 // Total Modules in Columns
#define DMDFont Arial_Black_16 // Font to be used to display

#define NeoPixelDataPIN  4 // DataPIN for Neo Pixel on Arduino Uno 7
#define NUMPIXELS 12 // Total Number of Pixels for Clock
#define ButtonPIN 2 // Tectile button pin number, PIN2 - Button - Ground
#define PIRPIN A1 // PIR pin for present sensor view pattern

int setState = 0; // Handle state of setting Hour, Minute and Seconds while Long Pressed ....
// setState = 0 - Hour, 1 = Minute and 2 = Second

// DMD Configuration for DMD 32x16 : 


DMD led_module(DMDRows, DMDColumns);

void scan_module()
{
  led_module.scanDisplayBySPI();
}

/**
   Initialize a new OneButton instance for a button
   connected to digital pin 4 and GND, which is active low
   and uses the internal pull-up resistor.
*/

OneButton btn = OneButton(
                  ButtonPIN,  // Input pin for the button
                  true,        // Button is active LOW Means attached from ButtonPIN (2) to Button to Ground
                  true         // Enable internal pull-up resistor
                );

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
// Define DS1307 RTC
/*
   https://lastminuteengineers.com/ds1307-rtc-arduino-tutorial/
   PINOUT : GND,5V,SCL=A5, SDA=A4 of arduino
*/

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Adafruit_NeoPixel pixels(NUMPIXELS, NeoPixelDataPIN, NEO_GRB + NEO_KHZ800);
// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

uint32_t magenta = pixels.Color(120, 255, 120); // Change colors here if you don't want to change color name, it's OK !
uint32_t green = pixels.Color(0, 80, 0);
uint32_t blue  = pixels.Color(0, 0, 120);
uint32_t red   = pixels.Color(80, 0, 0);
uint32_t white = pixels.Color(80, 80, 80);

int hr = 0; // Hour
int mn = 0; // Minute
int ss = 0; // Second
int yy = 0; // Year
int mm = 0; // Month
int dd = 0; // Day
int ss1 = 0; // To get Second from ButtonPIN when clicked after LongPress
int mn1 = 0; // To get Minute from ButtonPIN when clicked after LongPress
int hr1 = 0; // To get Hour from ButtonPIN when clicked after LongPress
const int HourFormat12 = 12;

int PIRPINState = 0; // Initialise PIR PIN

boolean IsDisplaySerial = true; // Set to true for Get/Set Date time in yymmddhhmmss format as input to serial communication ...

String hrs = ""; // String to get yymmddhhmmss from Serial terminal

// Starts main program setup section
void setup() {


  Timer1.initialize(2000);
  Timer1.attachInterrupt(scan_module);
  led_module.clearScreen( true );

  // put your setup code here, to run once:
  // Set  PIR PIN to input
  pinMode(PIRPIN, INPUT);

  PIRPINState = digitalRead(PIRPIN);

  pixels.begin(); // Run instance of pixels
  //pixels.fill(color, first, count);
  for ( int i = 0; i < 255; i++)
  {
    pixels.fill(pixels.Color(i, i, i), 0, i); // Fill random color to all pixels for visuals
    pixels.show();
    delay(10);
  }
  // Set brightness according to person present near to clock or not
  if (PIRPINState != 0)
  {
    // pixels.fill(white,0); // Fills all pixles with white color as indication of start of clock
    pixels.setBrightness(255); // Set brightness to full for all pixels, if there is someone near to clock
  }
  else
  {
    // pixels.fill(white,0); // Fills all pixles with white color as indication of start of clock
    pixels.setBrightness(65); // Set brightness to 1/4th for all pixels, this can be changed according to daylight ...
  }
  pixels.show();
  delay(1000);
  pixels.clear(); // Show all 'off' so that previous shown will be cleared
  delay(1000);
  // uint32_t color = strip.getPixelColor(11); // Get previously set color of pixel
  // uint16_t n = strip.numPixels();  // How many pixels is previously set
  // strip.setBrightness(64); // Set brightness to 1/4 from 0...255
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort(); // Because if there is no RTC, clock has no meaning
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

  btn.attachLongPressStart(longPressStart);
  btn.attachLongPressStop(longPressStop);
  btn.attachDuringLongPress(longPressDuring);
  btn.attachClick(handleClick);
  btn.setDebounceTicks(60); // Default 50, set as per your need
  btn.setClickTicks(700); // Default 500 set as per your need
  btn.setPressTicks(1500); // Default 800 set as per your need
}

// Handler function for a single click:
static void handleClick() {
  Serial.println("Clicked!"); // When button pressed once, just add 1 to current hour/minute/second
  Button_click();
}

// Single Click event attachment
//btn.attachClick(handleClick);

// Double Click event attachment with lambda
// btn.attachDoubleClick([]() {
//  Serial.println("Double Pressed!");
//});

// This is main loop from where you won't allowed to go out !!!
void loop()
{

  DateTime now = rtc.now();

  yy = now.year();
  mm = now.month();
  dd = now.day();

  hr = (now.hour() % HourFormat12) - 0; // this will be now.hour() if following if (hr != 12) is active
  mn = (((now.minute() + 4) / 5) % HourFormat12) - 0; // Turn into 5 minute interval (For 12 pixels) and to zero base
  ss = (((now.second() + 4) / 5) % HourFormat12) - 0; // Turn into 5 second interval (For 12 pixels) and to zero base
  mn1 = now.minute();
  hr1 = now.hour();
  ss1 = now.second();
  int hrsdisplay = (hr * 10000) + (mm * 100) +  ss;

  whiteColor(hr, mn, ss); // Show 12,3,6,9 as white indication color for better visual clock

  btn.tick();
  //hrsdisplay = hr * 10000 + mm * 100 +  ss;
  led_module.selectFont(DMDFont);
  led_module.drawMarquee("Hi VIPUL, Vismay and Miten....", 30, (32 * DMDRows), 0);
  //led_module.draw(2,10,hrsdisplay,GRAPHICS_NORMAL);
  //led_module.drawChar(1,3,hr,GRAPHICS_OR);
  long start = millis();
  long timming = start;
  boolean flag = false;

  while (!flag)
  {
    if ((timming + 10) < millis())
    {
      flag = led_module.stepMarquee(-1, 0);
      timming = millis();
    }
    //    }


    /*

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
    // whiteColor(hr,mn,ss); // Show 12,3,6,9 as white indication color for better visual clock

    pixels.show(); // Show pixels
    btn.tick();
    delay(10); // Delay for better view and time to show pixels
    btn.tick();
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
      if (Serial.available() > 0) {

        // Serial.print("Check - 02\n");

        hrs = Serial.readString(); // Read serial input and parse it to year, month, date, hour, minute and second each two character input in serial monitor
        //  hrs.trim(); // In case you may have entered spaces trailing or preceeding ...
        
        yy = hrs.substring(0, 2).toInt(); // Get 1st two characters as year
        mm = hrs.substring(2, 4).toInt(); // 3rd,4th two characters as month
        dd = hrs.substring(4, 6).toInt(); // 5th,6th two characters as day

        hr = hrs.substring(6, 8).toInt();   // 7th,8th two character as hour
        mn = hrs.substring(8, 10).toInt();  // 9th,10th two character as minute
        ss = hrs.substring(10, 12).toInt(); // 11th,12th two character as second
        Serial.print("Input String : " );
        Serial.println(hrs);

        rtc.adjust(DateTime(yy, mm, dd, hr, mn));

      }
      btn.tick();
      delay(10);
      btn.tick();
      Serial.flush();
      // Serial.print(" since midnight 1/1/1970 = ");
      // Serial.print(now.unixtime());
      // Serial.print("s = ");
      // Serial.print(now.unixtime() / 86400L);
      // Serial.println("d");
      //  pixels.clear();
    } // If serial display is turned on
  } // LED Display

  pixels.clear(); // Clear/Off all pixels
  btn.tick();
  delay(10); // Wait for better time for pixels
  btn.tick();
} // Main loop ends here ...

void whiteColor(int hr, int mn, int ss)
{
  // Show 12,3,6,9 in clock as white for indication

  uint32_t white = pixels.Color(30, 30, 30);
  pixels.clear();
  pixels.setPixelColor(0, white); // 12 hour sign
  pixels.setPixelColor(3, (hr = 3) ? white : green); // 3  hour sign
  pixels.setPixelColor(6, (hr = 6) ? white : green); // 6  hour sign
  pixels.setPixelColor(9, (hr = 9) ? white : green); // 9  hour sign

  pixels.setPixelColor(3, (mn = 3) ? white : blue); // 3  hour sign
  pixels.setPixelColor(6, (mn = 6) ? white : blue); // 6  hour sign
  pixels.setPixelColor(9, (mn = 9) ? white : blue); // 9  hour sign

  pixels.show();  // Show all hour visual sign

}

// Set Hour/Minute/Second when button pressed once or say clicked !
void Button_click() {
  if (setState == 0) {
    Serial.println("setting hour, look for green led.");
    Button_click_util();
  }
  else if (setState == 1) {
    Serial.println("setting minute, look for blue led..");
    Button_click_util();
  }
  else if (setState == 2) {
    Serial.println("setting second, look for red led...");
    Button_click_util();
  }


}
void Button_click_util() {

  DateTime now = rtc.now();

  yy = now.year();
  mm = now.month();
  dd = now.day();


  if (setState == 0) {
    hr1++;
    if (hr1 > 24)hr1 = 0;
    hr = (hr1 % HourFormat12) - 0; // this will be now.hour() if following if (hr != 12) is active
    Serial.println("current hour.");
    //if(hr > 24)hr = 0;
    Serial.println(hr1);
    pixels.setPixelColor(hr, green);
    pixels.fill(hr, green);
    pixels.show();
    //    hr++;

  }
  else if (setState == 1) {
    mn1++;
    if (mn1 > 60)mn1 = 0;
    mn = (((mn1 + 4) / 5) % HourFormat12) - 0; // Turn into 5 minute interval (For 12 pixels) and to zero base

    Serial.println("current minute");
    //if(mn > 12)mn = 0;
    Serial.println(mn1);
    pixels.setPixelColor(mn, blue);
    pixels.fill(mn, blue);
    pixels.show();
  }
  else if (setState == 2) {
    Serial.println("current second");
    ss1++;
    if (ss1 > 60)ss1 = 0;
    ss = (((ss1 + 4) / 5) % HourFormat12) - 0; // Turn into 5 second interval (For 12 pixels) and to zero base
    Serial.println(ss1);

    //if(ss > 12)ss  = 0;
    // Serial.println(ss);
    pixels.setPixelColor(ss, red);
    pixels.fill(ss, red);
    pixels.show();
    //    Serial.print("Second increasing  ");
    //    ss+=5;

  }
  rtc.adjust(DateTime(yy, mm, dd, hr1, mn1, ss1));
  pixels.show();
}

void longPressStart() {
  Serial.print("Button 1 longPress started. Start State is : ");
  Serial.print(setState);
}

void longPressStop() {
  Serial.print("Button 1 longPress stopped ");
  setState++;
  if (setState == 3) {
    setState = 0;
  }
  Serial.println("Stopped State is : ");
  Serial.println(setState);

} // longPressStop1

void longPressDuring() {
  Serial.print("Nothing to do during ...! During state is ");
  Serial.print(setState);
}
