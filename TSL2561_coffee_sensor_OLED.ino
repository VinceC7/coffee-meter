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



#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>   //for the IR sensor
#include <EEPROM.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"     //for the OLED display
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiWire oled;

//for separate broadband and ir measurements
int broadband = 0;
int infrared = 0;

int buttonState = 0;         // variable for reading the pushbutton status


/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
   
   Connections
   ===========
   Connect SCL to I2C SCL Clock
   Connect SDA to I2C SDA Data
   Connect VCC/VDD to 3.3V or 5V (depends on sensor's logic level, check the datasheet)
   Connect GROUND to common ground

   I2C Address
   ===========
   The address will be different depending on whether you leave
   the ADDR pin floating (addr 0x39), or tie it to ground or vcc. 
   The default addess is 0x39, which assumes the ADDR pin is floating
   (not connected to anything).  If you set the ADDR pin high
   or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
   (0x29) respectively.
    
   History
   =======
   2013/JAN/31  - First version (KTOWN)
*/
   
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  oled.clear();
  sensor_t sensor;
  tsl.getSensor(&sensor);
  oled.println("Coffee Darkness Sensor");
  oled.println("------------------------------------");
  oled.print  ("Sensor:    "); oled.println(sensor.name);
  oled.print  ("Driver Ver:"); oled.println(sensor.version);
  oled.print  ("Unique ID: "); oled.println(sensor.sensor_id);
  oled.print  ("Max Value: "); oled.print(sensor.max_value); oled.println(" lux");
  oled.print  ("Min Value: "); oled.print(sensor.min_value); oled.println(" lux");
  oled.print  ("Resolution:"); oled.print(sensor.resolution); oled.println(" lux");  
  oled.println("------------------------------------");
  oled.println("");
  delay(1000);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  
  oled.clear();
  oled.println("------------------------------------");
  oled.print  ("Gain:     "); oled.println("Auto");
  oled.print  ("Timing     "); oled.println("13 ms");
  oled.println("------------------------------------");
  delay (1000);
  
}

/**************************************************************************/
/*
    functions to take average of 4 measurements
*/
/**************************************************************************/
int average4reads(int farts)
{
  int total = 0;
  for (int i = 0; i < 4; i++)  {
    oled.clear();
    oled.print("taking measure= "); oled.println(i+1);
    digitalWrite(4, HIGH);  //red led
    digitalWrite(5, HIGH);  //red led
    delay (100);
    digitalWrite(4, LOW);  //red led
    digitalWrite(5, LOW);  //red led
    delay (100);
    digitalWrite(6, HIGH);  //IR led
    digitalWrite(7, HIGH);  //IR led
  
    tsl.getLuminosity (&broadband, &infrared);
    oled.println("");
    oled.print("infrared= "); oled.println(infrared);
    oled.println("");
    digitalWrite(6, LOW);  //IR led
    digitalWrite(7, LOW);  //IR led

    delay (250);
    if (i < 3)  {
      oled.print("rotate to pos= "); oled.println(i+2);
    }
    delay (4000);
    total = total + infrared;
  }
  
  
  int averageReading = total / 4;
  oled.print("average reading = "); oled.println(averageReading);
  delay(2000);
  return averageReading ;
}



/**************************************************************************/
/*
    calibrates the sensor between 1st crack and French roast
*/
/**************************************************************************/
void calibrateSensor(void)
{
  oled.clear();
  oled.println("*** CALIBRATION ***");

  delay(3000);
  
  oled.println("take cinnamon roast ");
  oled.println("measurement - NOW");
  oled.println("");

  delay(3000);
  
  /* Get a new sensor event */ 
  //sensors_event_t event;
  //tsl.getEvent(&event);

  //digitalWrite(6, HIGH);  //IR led
  //digitalWrite(7, HIGH);  //IR led
  
  //tsl.getLuminosity (&broadband, &infrared);
  infrared = average4reads(1);
  oled.print("Cinnamon IR= "); oled.println(infrared);

  //digitalWrite(6, LOW);  //IR led
  //digitalWrite(7, LOW);  //IR led
  
  oled.println("");
  oled.print("writing to EEPROM..... ");
  writeIntIntoEEPROM(0, infrared);
  
  delay(10000);

  oled.clear();
  oled.println("take Italian roast ");
  oled.println("measurement - NOW");
  oled.println("");
  delay(3000);
  
  /* Get a new sensor event */ 
  //sensors_event_t event;
  //tsl.getEvent(&event);

  //digitalWrite(6, HIGH);  //IR led
  //digitalWrite(7, HIGH);  //IR led
  
  //tsl.getLuminosity (&broadband, &infrared);
  infrared = average4reads(1);
  oled.print("Italian IR= "); oled.println(infrared);

  //digitalWrite(6, LOW);  //IR led
  //digitalWrite(7, LOW);  //IR led

  oled.println("");
  oled.print("writing to EEPROM..... ");
  writeIntIntoEEPROM(2, infrared);
  oled.println("");
  oled.println("FINISHED CALIBRATION");
  
  delay(5000);
  oled.clear();


  
}




/**************************************************************************/
/*
    functions to allow to write an int to EEPROM
*/
/**************************************************************************/

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}
int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}




/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Light Sensor Test"); Serial.println("");

  Wire.begin();
  Wire.setClock(400000L);
  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif // RST_PIN >= 0
  oled.setFont(Adafruit5x7);

  oled.clear();
  oled.println("Coffee Darkness Sensor");
  oled.println();
  


  sensors_event_t event;

  //set up the LED's
  pinMode(4, OUTPUT); //red LED
  pinMode(5, OUTPUT); //red LED 
  pinMode(6, OUTPUT); //IR LED
  pinMode(7, OUTPUT); //IR LED
  pinMode(2, INPUT); //calibrate button
  
  /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    oled.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  /* We're ready to go! */
  oled.println("setup done");

  //check to see if the calibrate button is pushed
  // read the state of the pushbutton value:
  buttonState = digitalRead(2);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // go to calibrate routine
    oled.println("going to calibrations routine");
    calibrateSensor();
  }
  oled.println("");
  oled.println("Press button to");
  oled.println("take reading");
  oled.println("");
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) 
{  
  
  //turn on LEDs
  //digitalWrite(4, HIGH);  //red led
  //digitalWrite(5, HIGH);  //red led
  //digitalWrite(6, HIGH);  //IR led
  //digitalWrite(7, HIGH);  //IR led
  //delay(250);

  
  /* Get a new sensor event */ 
  //sensors_event_t event;
  //tsl.getEvent(&event);

  
 
  /* Display the results (light is measured in lux) */
  //if (event.light)
  //{
   // Serial.print(event.light); Serial.println(" lux");
  //}
  //else
  //{
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
   // Serial.println("Sensor overload");
  //}

  //digitalWrite(6, LOW);  //IR led
  //digitalWrite(7, LOW);  //IR led
  //delay(250);
  
  //tsl.getLuminosity (&broadband, &infrared);
  //Serial.print("broadband= "); Serial.println(broadband);

  //digitalWrite(4, LOW);  //red led
  //digitalWrite(5, LOW);  //red led
  //digitalWrite(6, HIGH);  //IR led
  //digitalWrite(7, HIGH);  //IR led
  //delay(250);
  
  //tsl.getLuminosity (&broadband, &infrared);
  //Serial.print("infrared= "); Serial.println(infrared);

  ////turn off LEDs
  //digitalWrite(6, LOW);  //IR led
  //digitalWrite(7, LOW);  //IR led

  // read the state of the pushbutton value:
  buttonState = digitalRead(2);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // measure the IR value
    oled.clear();
    oled.println("taking IR measurements");
    infrared = average4reads(1);
    oled.clear();
    oled.print("Average=  "); oled.println(infrared);
    int darkNumber = readIntFromEEPROM(2);
    int lightNumber = readIntFromEEPROM(0);
    oled.print("Italian (Egg30)=  "); oled.println(darkNumber);
    oled.print("Cinnamon(Egg70)=  "); oled.println(lightNumber);
    float eggtron = ((infrared-darkNumber)*10.0 / (lightNumber - darkNumber))*4.0+30.0;
    
    oled.println("EGGTRON scale value"); 
    oled.set2X();
    oled.print("   ");
    oled.println(eggtron);
    oled.set1X();
    if (eggtron < 35.0) {
      oled.println("Italian 30-35");
      oled.println("Shiny, past 2nd crack");
    }
    if ((eggtron >= 35.0) && (eggtron <45)) {
      oled.println("French 35-45 ");
      oled.println("Some oil 2nd crack");
    }
    if ((eggtron >= 45.0) && (eggtron <55)) {
      oled.println("City+ Vienna 45-55");
      oled.println("begin 2nd crack");
    }
    if ((eggtron >= 55.0) && (eggtron <60)) {
      oled.println("City medium 55-60");
      oled.println("medium brown");
    }
    if ((eggtron >= 60.0) && (eggtron <70)) {
      oled.println("American med/li 60-70");
      oled.println("end 1st crack");
    }
    if (eggtron >= 70.0) {
      oled.println("Cinnamon > 70");
      oled.println("begin 1st crack");
    }
  }
  
  
  
  delay(100);
}
