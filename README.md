# Clock12LEDRTC

This is a simple 12 LED clock. For the list of material and step-by-step detailed guide on how to make it, please refer my instructables project from here https://www.instructables.com/12Digit-Clock-With-WS2812B-NeoPixel-LED-and-DS1307/

Circuit : For Data pin of WS2812b neo pixel, arduino pin 7 is defined into the sketch and for DS1307RTC, data pin SCL->A5 and SDA->A4, for Button Pin -> 2 -> Button -> Ground

Library included is 
1. Adafruit Neo Pixel for WS2812B neo pixel led.
2. RTCLib for DS1307RTC module
3. OneButton for Button press events

For Details about Neo Pixel library visit https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use , for DS1307RTC visit https://lastminuteengineers.com/ds1307-rtc-arduino-tutorial/ and for OneButton examples visit https://github.com/mathertel/OneButton github repository or http://www.mathertel.de/Arduino/OneButtonLibrary.aspx 

The code used in the turtorial is given in this repository and instructable is also there on instructables.com for which link is given in this file.

V1 = Simple clock with time setting by Serial Monitor only.
V2 = Simple clock with time setting by Serial Monitor and by button (Tectile Switch) attached to arduino pin 2.
V3 = V2 and DMD 32x16 Panel display with logo message, in this version we need to display date, time and display of date time setting through pin 2, is under implementation ...
