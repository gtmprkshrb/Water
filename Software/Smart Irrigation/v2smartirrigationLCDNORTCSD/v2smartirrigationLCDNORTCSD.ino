/*
Sources:
Liquid flow rate sensor -DIYhacking.com Arvind Sanjeev

Project: Water usage tracking
Application: Irrigation, Home audit etc
Coder: GP

Board Used: Arduin Nano
Sensors: Flow Sensor YF-S201

Objective: Measure the liquid/water flow rate using this code. 

Pin Allocation:
A0 - 
A4 - 
A5 - 


Libraries needed:
SD Card Library
RTC Library

Customisations:
KD Home - soil drains fast, hence upper limit set to 700, lower to 475
*/

#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x3F);

//const int chipSelect = 10;
const int moistpin = 6;
//int valve = 7;
int valve = 6;
int led    = 13;

//RTC_DS1307 rtc;
//RTC_DS3231 rtc;

//char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 7.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

void setup()
{
  
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);

   /*if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");*/
  
  // Set up the status LED line as an output
  //pinMode(statusLed, OUTPUT);
  //digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode (valve, OUTPUT);
  pinMode (led, OUTPUT);
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  
   lcd.begin(16, 2);
   lcd.setBacklight(255);
    lcd.home(); 
    lcd.clear();
    lcd.print("SmartIrrigation");
    delay(1000);
 
}

//Main program loop

void loop()
{

    //DateTime now = rtc.now();
    int moisture = analogRead(moistpin);    
    /*Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    //Serial.print(" (");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(", ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();*/
    Serial.println(" Soil moisture is at ");
    Serial.print(moisture);
    //Serial.print(" Valve is ");
    //Serial.print(valve);
    
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
    
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    oldTime = millis();
    
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dryness:");
    lcd.print(moisture);
    lcd.setCursor(0, 1);
    lcd.print("ml used ");
    lcd.print(totalMilliLitres);
  }

delay (1000);
  
  //SD Card starts
 /* File dataFile = SD.open("office.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("RBAVRS ");
    dataFile.print(",");
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    //dataFile.print(now.day(), DEC);
    //dataFile.print(" (");
    //dataFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
    dataFile.print(", ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(",");
    dataFile.print(flowMilliLitres);
    dataFile.print("ml ");
    dataFile.print(",");
    dataFile.print(totalMilliLitres/1000);
    dataFile.print("L ");
    dataFile.print(",");
    dataFile.print("Moisture");
    dataFile.print(",");
    dataFile.print(moisture);
    dataFile.print(",");
    dataFile.close();
    delay(1000);// added to make polling frequency per one second
    //Serial.println("data logged");
}
else {
  Serial.println("error opening dis.csv");
}*/

if(moisture>=500){//Modify value to turn on water
  digitalWrite(valve, HIGH);
  digitalWrite(led, HIGH);

    
}
else if(moisture<=400){//turn off water
  digitalWrite(valve, LOW);
  digitalWrite(led, LOW);

}
}
/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

