#include <RTClib.h> //RTC Module for Date and Time function
#include <Wire.h> //Wire Module for connecting RTC module with controller
#include <LiquidCrystal.h> //LCD Module

RTC_DS1307 RTC; // defining instance RTC for RTC_DS1307 


/*
 * RS = 7, E = 8, D4 = 9, D5 = 10, D6 = 11, D7 = 12
 * LiquidCrystal lcd(7, 8,9, 10, 11, 12);
 */
LiquidCrystal lcd(7, 8,9, 10, 11, 12);

/*defining OUTPUT pins for relays associated with water pump 1, water valve 1, water valve 2 , water valve 3
 * const int pinNum[4] = {53,52,51,50, 49}; //pin numbers
 *  53 = water pump 1, realy 1
 *  52 = water valve 1, pot 1, relay 1
 *  51 = water valve 2, pot 2, relay 2
 *  50 = water valve 3, pot 3, relay 3
 *  49 = lamp 1 for plants
*/
const int pinNum[5] = {53, 52, 51, 50, 49}; //pin numbers

String pumpOnDay[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"}; // defining days

String pumpOnTime[7] = {"6:0 AM", "6:30 AM", "7:0 AM", "7:30 AM", "8:0 AM", "8:30 AM", "9:0 AM"}; // defining time 

String rtcVariable[10] = {"Year", "Month", "Day", "WeekDay", "Hr", "Min", "Sec", "WeekDayTime", "currentDate", "am_pm"}; // rtc var

String compareDateTime[7] = {pumpOnDay[0] + " " + pumpOnTime[3],
                             pumpOnDay[1] + " " + pumpOnTime[3],
                             pumpOnDay[2] + " " + pumpOnTime[3],
                             pumpOnDay[3] + " " + pumpOnTime[3],
                             pumpOnDay[4] + " " + pumpOnTime[3],
                             pumpOnDay[5] + " " + pumpOnTime[3],
                             pumpOnDay[6] + " " + pumpOnTime[3]}; //date selection 

unsigned long devicesDelay[6] = {10000, 30000, 30000, 30000, 20000, 3000};

String displayInfo[8] = {"                ",
                         "Water Sys Ready",
                         "Water Pump ON",
                         "WP & WV1 ON",
                         "WP & WV2 ON",
                         "WP & WV3 ON",
                         "Plant Light ON",
                         "Plant Light OFF"};

int tempPin = 0; // setting up temperature input pin number to 0

/* initial setup for different sequences of main program */
void setup() {
  // put your setup code here, to run once at begining of program scan:
   
  for (int i = 0; i<5; i++){
     pinMode(pinNum[i], OUTPUT); //assigning PINs to output mode
     digitalWrite(pinNum[i], LOW); ////turning OFF all device relays at inital start
  }
  
  //caling initial setup for RTC module
  rtcSetup();
  lcd.begin(16,2);
  
}


/* initial setup for rtc module */
void rtcSetup()
{
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  //RTC.adjust(DateTime(2020, 2, 8, 1, 21, 0)); 
  // This will set RTC clock to Year = 2020, Month = 2(Feb), Date = 7, Hours = 10, Minute = 58, Seconds = 0
  if(!RTC.isrunning())
  {
    Serial.println("RTC is NOT running");
    //following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(F(__DATE__),F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example
    // This will set RTC clock to Year = 2020, Month = 2(Feb), Date = 7, Hours = 10, Minute = 58, Seconds = 0
    //RTC.adjust(DateTime(2020, 2, 7, 10, 58, 0)); 
  }
}


/*
 * Temperature reading function
 */
void temperatureReading()
{
  int tempReading = analogRead(tempPin);
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  float tempC = tempK - 273.15;            // Convert Kelvin to Celcius
  float tempF = (tempC * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  /*  replaced
    float tempVolts = tempReading * 5.0 / 1024.0;
    float tempC = (tempVolts - 0.5) * 10.0;
    float tempF = tempC * 9.0 / 5.0 + 32.0;
  */
  // Display Temperature in C
  //lcd.setCursor(0, 0);
  //lcd.print("Temp         C  ");
  // Display Temperature in F
  //lcd.print("Temp         F  ");
  lcd.setCursor(15, 0);
  lcd.print("F");
  // Display Temperature in C
  //lcd.print(tempC);
  // Display Temperature in F
  lcd.setCursor(10, 0);
  lcd.print(tempF);
}
 

/* Reading current Date from rtc
 * Returns current date from RTC, date format mm/dd/yy 
 * rtcVariable[10] = {"Year", "Month", "Day", "WeekDay", "Hr", "Min", "Sec", "WeekDayTime", "currentDate", "am_pm"};
 */
void currentDateTime() 
{
  DateTime now = RTC.now(); // defining datetime instance now
  rtcVariable[0] = String(now.year(), DEC);
  rtcVariable[1] = String(now.month(), DEC);
  rtcVariable[2] = String(now.day(), DEC);
  rtcVariable[3] = String(pumpOnDay[now.dayOfTheWeek()]);
  rtcVariable[4] = String(now.hour(), DEC);
  rtcVariable[5] = String(now.minute(), DEC);
  rtcVariable[6] = String(now.second(), DEC);

  if(now.hour()<=12)
  {
    rtcVariable[9] = "AM";
  }
  else if (now.hour()>12 && now.hour()<=24)
  {
    rtcVariable[9] = "PM";
  }
  
  rtcVariable[7] =  rtcVariable[3] + " " +  rtcVariable[4] + ":" +  rtcVariable[5] + ":" +  rtcVariable[6]  + " " + rtcVariable[9];

  rtcVariable[8] = rtcVariable[0] + "/" + rtcVariable[1] + "/" + rtcVariable[2] + ", " + rtcVariable[7];

}

/*
 * display information data
 * calls specific display info
 * same function will be called from different functions
 */
 void LCDdisplayData(int index)
 {
  lcd.setCursor(0, 1);
  lcd.print(displayInfo[index]);
  
 }

/* Turning devices relay ON */
void devicesRelayON(int deviceNum)
{
  
  digitalWrite(pinNum[deviceNum], HIGH);
  if(deviceNum == 0)
  {
      
    Serial.println("Water Pump 1 ON");
    LCDdisplayData(0);
    LCDdisplayData(2);
    
  }
  else if (deviceNum == 1)
  {
    Serial.println("WP1 & WV1 ON");
    LCDdisplayData(0);
    LCDdisplayData(3);
   
  }
  else if (deviceNum == 2)
  {
    
    Serial.println("WP1 & WV2 ON");
    LCDdisplayData(0);
    LCDdisplayData(4);
    
  }
  else if (deviceNum == 3)
  {
    Serial.println("WP1 & WV3 ON");
    LCDdisplayData(0);
    LCDdisplayData(5);
    
  }
  
}

/* Turning devices relay OFF */
void devicesRelayOFF(int deviceNum)
{
  digitalWrite(pinNum[deviceNum], LOW);
}


/* Turing delay timer for devices */
void devicesDelayActive(int deviceNum)
{
  delay(devicesDelay[deviceNum]);
}

/*
 * calling devices realy ON/OFF functions
 * reading current clock time from rtc
 */
void irrigationSystem()
{
  String weekDayAndTime = "";
  String weekDayDisplay;
  currentDateTime();
  weekDayAndTime = rtcVariable[3] + " " +  rtcVariable[4] + ":" +  rtcVariable[5] + " " +  rtcVariable[9];
  weekDayDisplay = rtcVariable[3] + " " + rtcVariable[4] + ":" +  rtcVariable[5];
  lcd.setCursor(0, 0);
  lcd.print("         ");
  lcd.setCursor(0, 0);
  lcd.print(weekDayDisplay);
  Serial.println(rtcVariable[8]);
  Serial.println(weekDayAndTime);
  Serial.println(compareDateTime[0]);
  
  
  if((compareDateTime[0] == weekDayAndTime) || (compareDateTime[2] == weekDayAndTime) || (compareDateTime[4] == weekDayAndTime))
  {
    devicesRelayON(0);
    devicesDelayActive(0);
    
    devicesRelayON(1);
    devicesDelayActive(1);
    devicesRelayOFF(1);
    
    devicesRelayON(2);
    devicesDelayActive(2);
    devicesRelayOFF(2);
    

    if(compareDateTime[0] == weekDayAndTime)
    {
      devicesRelayON(3);
      devicesDelayActive(4);
      devicesRelayOFF(3);
     
    }

    /*
     * rtcVariable[10] = {"Year", "Month", "Day", "WeekDay", "Hr", "Min", "Sec", "WeekDayTime", "currentDate", "am_pm"};
     */
    devicesDelayActive(5);
    devicesRelayOFF(0);
    
    currentDateTime();
    String weekDayAndTime1 = rtcVariable[3] + " " +  rtcVariable[4] + ":" +  rtcVariable[5] + " " +  rtcVariable[9];
    do
    {
      LCDdisplayBlink(0,1);
      lightOnOff();
      Serial.println("inside while loop");
      weekDayAndTime1 = "";
      currentDateTime();
      weekDayAndTime1 = rtcVariable[3] + " " +  rtcVariable[4] + ":" +  rtcVariable[5] + " " +  rtcVariable[9];
    
      }while((compareDateTime[0] == weekDayAndTime1) || (compareDateTime[2] == weekDayAndTime1) || (compareDateTime[4] == weekDayAndTime1));

  }
  
  else if((compareDateTime[1] == weekDayAndTime) || (compareDateTime[3] == weekDayAndTime) || (compareDateTime[5] == weekDayAndTime) || (compareDateTime[6] == weekDayAndTime))
  {
    devicesRelayON(0);
    devicesDelayActive(0);
    
    devicesRelayON(1);
    devicesDelayActive(4);
    devicesRelayOFF(1);
    
    devicesRelayON(2);
    devicesDelayActive(4);
    devicesRelayOFF(2);
    

    if(compareDateTime[3] == weekDayAndTime)
    {
      devicesRelayON(3);
      devicesDelayActive(4);
      devicesRelayOFF(3);
     
    }

    /*
     * rtcVariable[10] = {"Year", "Month", "Day", "WeekDay", "Hr", "Min", "Sec", "WeekDayTime", "currentDate", "am_pm"};
     */
    devicesDelayActive(5); 
    devicesRelayOFF(0);
    
    currentDateTime();
    String weekDayAndTime1 = rtcVariable[3] + " " +  rtcVariable[4] + ":" +  rtcVariable[5] + " " +  rtcVariable[9];
    do
    {
      LCDdisplayBlink(0,1);
      lightOnOff();
      Serial.println("inside while loop");
      weekDayAndTime1 = "";
      currentDateTime();
      weekDayAndTime1 = rtcVariable[3] + " " +  rtcVariable[4] + ":" +  rtcVariable[5] + " " +  rtcVariable[9];
    
      }while((compareDateTime[1] == weekDayAndTime1) || (compareDateTime[3] == weekDayAndTime1) || (compareDateTime[5] == weekDayAndTime1)|| (compareDateTime[6] == weekDayAndTime1));
    
  }

}


/*
   * lcd.setCursor(0, 1);
   * lcd.print("WATER SYS. READY");
   * Serial.println("System Ready");
   */
void LCDdisplayBlink(int indexA, int indexB) 
{
  LCDdisplayData(indexA);
  delay(800);
  LCDdisplayData(indexB);
  delay(1300);
       
}


/*
 * light turns ON and OFF depending upon time settings
 * form morining set time to evening set time
 */

void lightOnOff()
{
  /*
   * rtcVariable[10] = {"Year", "Month", "Day", "WeekDay", "Hr", "Min", "Sec", "WeekDayTime", "currentDate", "am_pm"};
   */
   DateTime now = RTC.now(); // defining datetime instance now
   currentDateTime();
   if( now.hour() > 7 && now.hour() < 17)
   {
    digitalWrite(pinNum[4], HIGH);
    LCDdisplayBlink(0,6);
   }
   else
   {
    digitalWrite(pinNum[4], LOW);
    LCDdisplayBlink(0,7);
   }
   Serial.println(now.hour());
   
}


void loop() {
  // put your main code here, to run repeatedly:
  
  LCDdisplayBlink(0,1);
  temperatureReading();
  lightOnOff();
  irrigationSystem();
       
}
