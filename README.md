# coffee-meter

/* Coffee darkness meter - by V.Chan - Toronto, Canada, Oct 2021
 * for testing the darkness of whole bean and ground coffee
 * 1) uses an Adafruit TSL2561 breakout board (others can be
 * substituded
 * 2) an SSD1306 OLED for a display
 * 3) arduino nano
 * 
 * Note: I only use the IR sensor, the LUX sensor had a narrow range
 * although I left the code commented out
 * 
 * To enter calibration mode: press button while powering up.
 *    you need a cinnamon roast (beginning of first crack) this
 *    taken as eegtron = 70, and an Italian roast (just end of
 *    2nd crack) eegtron = 30.
 * 
 * To make measurements, just press button to start 4 avg readings
 *    Eggtron measurements are based off calibration above
 * 
 */
 
 Files to install in your Arduino library:

For the TSL2561 sensor
Adafruit_Sensor.h
Adafruit_TSL2561_U.h

For the OLED display
SSD1306Ascii.h
SSD1306AsciiWire.h
